#include "LoginRegister.h"
#include "lss.h"
#include <thread>
#include "json/json.hpp"
#include "HighsManager.h"
#include "RuntimeData.h"

auto& runtime = RuntimeData::getInstance();
static bool loginOpen = false;
static bool registerOpen = false;

void LoginRegister::Login(bool& loginWindow) 
{
    if (loginWindow || loginOpen) {
        loginWindow = false;
        loginOpen = true;
        if (Lss::Modal("Login Page", &loginOpen, ImVec2(30 * Lss::VW, 60 * Lss::VH), Centered))
        {
            static char usernameText[200];
            Lss::Text("yo", 4 * Lss::VH);
            if (registerOpen)
            {
                Lss::InputText("Username", usernameText, sizeof(usernameText), ImVec2(20 * Lss::VW, 5 * Lss::VH), Centered);
            }
            static char emailText[200];
            Lss::InputText("Email", emailText, sizeof(emailText), ImVec2(20 * Lss::VW, 5 * Lss::VH), Centered);
            static char passText[100];
            Lss::InputText("Password", passText, sizeof(passText), ImVec2(20 * Lss::VW, 5 * Lss::VH), Centered);
            std::string name = (registerOpen) ? "Sign In" : "Login";
            if (Lss::Button("want to register?", ImVec2(10*Lss::VH, 2*Lss::VH),1.5f*Lss::VH))
            {
                registerOpen = !registerOpen;
            }
            if (Lss::Button(name, ImVec2(10 * Lss::VW, 6 * Lss::VH), 4 * Lss::VH, Centered)) {
                nlohmann::json body;
                if (registerOpen) body["username"] = usernameText;
                body["email"] = emailText;
                body["password"] = passText;
                nlohmann::json res;
                if(registerOpen) res = HManager::Request("users", body.dump(), POST);
                else res = HManager::Request("users/login", body.dump(), POST);

                if (res.contains("access_token") && res.contains("username")) {
                    std::cout << "got this JSON: " << res["access_token"] << std::endl;
                    runtime.token = res["access_token"];
                    runtime.username = res["username"];
                    runtime.password = passText;
                    runtime.logedIn = true;
                    nlohmann::json result = HManager::Request("users", "", GET);
                    runtime.id = result["id"];
                    loginOpen = false;
                    std::vector<uint8_t> imageData = HManager::ImageRequest(("users/" + std::to_string(runtime.id) + " / pfp").c_str());
                    float ratioStuff = 0.0f;
                    runtime.pfpTexture = HManager::ImageFromRequest(imageData, ratioStuff);
                    ImGui::CloseCurrentPopup();
                }
            }
            Lss::End();
            ImGui::EndPopup();
        }

    }
}