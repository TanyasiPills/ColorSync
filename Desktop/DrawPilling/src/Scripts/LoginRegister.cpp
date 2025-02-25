#include "LoginRegister.h"
#include "lss.h"
#include <thread>
#include "json/json.hpp"
#include "HighsManager.h"
#include "RuntimeData.h"

auto& runtime = RuntimeData::getInstance();

void LoginRegister::Login(bool& loginWindow) 
{
    static bool loginOpen = false;
    if (loginWindow || loginOpen) {
        loginWindow = false;
        loginOpen = true;
        if (Lss::Modal("Login Page", &loginOpen, ImVec2(30 * Lss::VW, 60 * Lss::VH), Centered))
        {
            Lss::Text("yo", 4 * Lss::VH);
            static char userText[200];
            Lss::InputText("Username", userText, sizeof(userText), ImVec2(20 * Lss::VW, 5 * Lss::VH), Centered);
            static char passText[100];
            Lss::InputText("Password", passText, sizeof(passText), ImVec2(20 * Lss::VW, 5 * Lss::VH), Centered);
            if (Lss::Button("closeLogin", ImVec2(10 * Lss::VW, 6 * Lss::VH), 4 * Lss::VH, Centered)) {
                std::thread([]() {
                    nlohmann::json body;
                    body["email"] = userText;
                    body["password"] = passText;

                    nlohmann::json res = HManager::Request((runtime.ip + ":3000/users/login").c_str(), body.dump(), POST);

                    if (res.contains("access_token") && res.contains("username")) {
                        std::cout << "got this JSON: " << res["access_token"] << std::endl;
                        runtime.token = res["access_token"];
                        runtime.username = res["username"];
                        runtime.logedIn = true;
                        loginOpen = false;
                        ImGui::CloseCurrentPopup();
                    }
                    }).detach();
            }
            Lss::End();
            ImGui::EndPopup();
        }

    }
}