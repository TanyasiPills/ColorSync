#include "LoginRegister.h"
#include "lss.h"
#include <thread>
#include "json/json.hpp"
#include "HighsManager.h"
#include "RuntimeData.h"

static auto& runtime = RuntimeData::getInstance();
static bool loginOpen = false;
static bool registerOpen = false;

void LoginRegister::Login(bool& loginWindow) 
{
    if (loginWindow || loginOpen) {
        loginWindow = false;
        loginOpen = true;

        static float modalSize = 0.0f;

        if (Lss::Modal("Login Page", &loginOpen, ImVec2(30 * Lss::VW, modalSize), Centered | Rounded | Bordering, ImGuiWindowFlags_NoDecoration))
        {
            float startPos = ImGui::GetCursorPosY();
            ImVec2 valid = ImGui::GetContentRegionAvail();
            ImVec2 def = ImGui::GetStyle().FramePadding;
            ImGui::GetStyle().FramePadding = ImVec2(0.0f, 0.0f);


            Lss::SetColor(ContainerBackground, LowHighlight);
            Lss::Child("##Postheader", ImVec2(valid.x, 9 * Lss::VH), false, Rounded);
            Lss::LeftTop(Lss::VW, Lss::VH);
            if(registerOpen) Lss::Text("Sign Up", 5 * Lss::VH);
            else Lss::Text("Sign In", 5 * Lss::VH);
            Lss::LeftTop(Lss::VW, -0.75f * Lss::VH);
            if (registerOpen) Lss::Text("Create a new account", 2 * Lss::VH);
            else Lss::Text("Please enter your credentials to log in", 2*Lss::VH);
            Lss::End();
            ImGui::EndChild();
            Lss::SetColor(ContainerBackground, ContainerBackground);
            ImGui::GetStyle().FramePadding = def;


            Lss::Top(0.2 * Lss::VH);
            Lss::Separator(1.0f, 30 * Lss::VW, 4, Centered);

            static char usernameText[200];
            if (registerOpen)
            {
                Lss::LeftTop(Lss::VW, Lss::VH);
                Lss::Text("Username", 2 * Lss::VH);
                Lss::LeftTop(1.2 * Lss::VW, Lss::VH);
                Lss::InputText("usernameInput", usernameText, sizeof(usernameText), ImVec2(26.6 * Lss::VW, 4 * Lss::VH), Rounded, 0, 0, "Username");
            }

            Lss::LeftTop(Lss::VW, Lss::VH);
            Lss::Text("Email address", 2 * Lss::VH);
            static char emailText[128] = "";
            Lss::LeftTop(1.2 * Lss::VW, Lss::VH);
            Lss::InputText("emailInput", emailText, sizeof(emailText), ImVec2(26.6 * Lss::VW, 4 * Lss::VH), Rounded, 0, 0, "example@example.com");

            Lss::LeftTop(Lss::VW, Lss::VH);
            Lss::Text("Password", 2 * Lss::VH);
            static char passwordText[128] = "";
            Lss::LeftTop(1.2 * Lss::VW, Lss::VH);
            Lss::InputText("passwordInput", passwordText, sizeof(passwordText), ImVec2(26.6 * Lss::VW, 4 * Lss::VH), Rounded, ImGuiInputTextFlags_Password, 0, "Password");

            Lss::LeftTop(Lss::VW, 1.5f*Lss::VH);
            std::string name = (!registerOpen) ? "Sign In" : "Sign Up";
            if (Lss::Button(name, ImVec2(26.6 * Lss::VW, 4 * Lss::VH), 3 * Lss::VH, Centered | Rounded)) {
                nlohmann::json body;
                if (registerOpen) body["username"] = usernameText;
                body["email"] = emailText;
                body["password"] = passwordText;
                nlohmann::json res;
                if(registerOpen) res = HManager::Request("users", body.dump(), POST);
                else res = HManager::Request("users/login", body.dump(), POST);

                if (res.contains("access_token") && res.contains("username")) {
                    std::cout << "got this JSON: " << res["access_token"] << std::endl;
                    runtime.token = res["access_token"];
                    runtime.username = res["username"];
                    runtime.email = res["email"];
                    runtime.password = passwordText;
                    runtime.logedIn = true;
                    nlohmann::json result = HManager::Request("users", "", GET);
                    runtime.id = result["id"];
                    loginOpen = false;
                    std::vector<uint8_t> imageData = HManager::ImageRequest(("users/" + std::to_string(runtime.id) + "/pfp").c_str());
                    float ratioStuff = 0.0f;
                    runtime.pfpTexture = HManager::ImageFromRequest(imageData, ratioStuff);
                    ImGui::CloseCurrentPopup();

                    res = HManager::Request("users/likes","", GET);
                    if (!res.is_null() && !result.empty())for (auto& item : res) runtime.liked.insert((int)item);
                }
            }

            Lss::LeftTop(Lss::VW, Lss::VH);
            std::string text = (registerOpen) ? "Already sign up? Sign in" : "New here? Sign up";
            Lss::SetFontSize(1.5f * Lss::VH);
            float sizeX = ImGui::CalcTextSize(text.c_str()).x+Lss::VH;
            if (Lss::Button(text, ImVec2(sizeX, 2 * Lss::VH), 1.5f * Lss::VH, Centered | Rounded | Invisible))
            {
                registerOpen = !registerOpen;
            }

            float endPos = ImGui::GetCursorPosY();
            modalSize = endPos - startPos + Lss::VH * 3;

            if (ImGui::IsMouseClicked(0))
            {
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 cursorPos = ImGui::GetMousePos();
                ImVec2 size = ImGui::GetWindowSize();
                if (!Lss::InBound(cursorPos, pos, size)) {
                    ImGui::CloseCurrentPopup();
                    loginOpen = false;
                }
            }

            Lss::End();
            ImGui::EndPopup();
        }

    }
}