#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <Xinput.h>
#include <ViGEmClient.h>


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "ViGEm: Dracula X", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    auto client = vigem_alloc();

    PVIGEM_TARGET targets[XUSER_MAX_COUNT];

    for (auto i = 0; i < _countof(targets); i++)
    {
        targets[i] = vigem_target_x360_alloc();
    }

    auto xInputMod = LoadLibrary(L"XInput1_3.dll");

    auto pXInputEnable = reinterpret_cast<VOID(WINAPI*)(BOOL)>(GetProcAddress(xInputMod, "XInputEnable"));
    auto pXInputGetState = reinterpret_cast<DWORD(WINAPI*)(DWORD, XINPUT_STATE*)>(GetProcAddress(xInputMod, "XInputGetState"));

    vigem_connect(client);
    pXInputEnable(TRUE);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        window.clear();

        ImGui::SFML::Render(window);
        window.display();
    }

    vigem_disconnect(client);
    vigem_free(client);

    ImGui::SFML::Shutdown();
}
