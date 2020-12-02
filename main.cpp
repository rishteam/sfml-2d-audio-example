#include <bits/stdc++.h>
using namespace std;

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "2D Sound");
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;
    bool running = true;
    bool followMouse = false;

    sf::SoundBuffer soundBuffer;
    if(!soundBuffer.loadFromFile("assets/peko_mono.ogg"))
    {
        return 1;
    }

    // Listener
    sf::Listener::setPosition(0.f, 0.f, 0.f);
//    sf::Listener::setDirection(0.f, 1.f, 0.f);
    sf::Listener::setUpVector(0.f, 0.f, 1.f);
    sf::Listener::setGlobalVolume(50.f);

    float sound_distance = 5.f;
    float sound_attenuation = 1.f;
    // Sound
    sf::Sound sound;
    sound.setBuffer(soundBuffer);
    sound.setMinDistance(sound_distance);
    sound.setAttenuation(sound_attenuation);
    sound.setLoop(true);
    sound.setRelativeToListener(true);
    sound.play();

    sf::Vector3f pos;
    float radius = 10.f;
    float t = 0;
    float angular_velocity = 1.0f;
    sf::Clock clk;

    auto GetSoundFactor = [&](float dis)
    {
        return sound_distance / (sound_distance + sound_attenuation * (max(dis, sound_distance) - sound_distance));
    };

    // run the program as long as the window is open
    while (running)
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                running = false;
            else if(event.type == sf::Event::KeyPressed)
            {
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Debug");
        {
            ImGui::Checkbox("Follow mouse", &followMouse);
            ImGui::Text("Sound Pos = %.2f %.2f %.2f", pos.x, pos.y, pos.z);
            ImGui::DragFloat("Radius", &radius);
            ImGui::DragFloat("Vel", &angular_velocity);
            if (ImGui::DragFloat("MinDistance", &sound_distance))
            {
                if (sound_distance <= 0.f)
                    sound_distance = 0.001f;
                sound.pause();
                sound.setMinDistance(sound_distance);
                sound.play();
            }
            if (ImGui::DragFloat("Attenuation", &sound_attenuation, 0.001))
            {
                if(sound_attenuation <= 0.f)
                    sound_attenuation = 0.f;
                sound.pause();
                sound.setAttenuation(sound_attenuation);
                sound.play();
            }
            ImGui::Separator();
            float dis = sqrt(pow(pos.x,2)+pow(pos.y,2));
            ImGui::Text("Volume = %.2f", GetSoundFactor(dis) );
        }
        ImGui::End();

        // Update
        {
            sf::Time dt = clk.restart();

            if(!followMouse)
            {
                // Update rotation
                t += dt.asSeconds() * angular_velocity;

                pos.x = radius * std::cos(t);
                pos.y = radius * std::sin(t);
                pos.z = 0.f;
            }
            else
            {
                // Follow the mouse
                auto m = sf::Mouse::getPosition(window);
                auto siz = window.getSize();
                // Convert origin to center
                m.x = m.x - siz.x / 2.f;
                m.y = m.y - siz.y / 2.f;

                pos.x = m.x;
                pos.y = m.y;
            }

            sound.setPosition(pos);
        }

        // Draw
        {
            window.clear();

            sf::Vector2f centerPos = {window.getSize().x / 2.f, window.getSize().y / 2.f};
            // Center
            sf::CircleShape center{1.f};
            center.setOrigin(center.getRadius(), center.getRadius());
            center.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
            center.setFillColor(sf::Color::Red);
            window.draw(center);

            // Draw the Min Distance circle
            sf::CircleShape minDistanceCir{sound_distance};
            minDistanceCir.setOrigin(minDistanceCir.getRadius(), minDistanceCir.getRadius());
            minDistanceCir.setPosition(centerPos.x, centerPos.y);
            minDistanceCir.setFillColor(sf::Color::Transparent);
            minDistanceCir.setOutlineThickness(1.f);
            minDistanceCir.setOutlineColor(sf::Color::Blue);
            window.draw(minDistanceCir);

            // Draw the attenuation circles
            for (float i = 1000.f; i >= 0; i -= 1.f)
            {
                float radius = sound_distance + i;
                sf::CircleShape attenuationCir{radius};
                float factor = GetSoundFactor(radius);
                attenuationCir.setOrigin(attenuationCir.getRadius(), attenuationCir.getRadius());
                attenuationCir.setPosition(centerPos.x, centerPos.y);
                attenuationCir.setFillColor(sf::Color(factor * 255, factor * 255, factor * 255));
                window.draw(attenuationCir);
            }

            // Sound shape
            sf::CircleShape soundCir{1.f};
            soundCir.setOrigin(soundCir.getRadius(), soundCir.getRadius());
            soundCir.setPosition(centerPos.x + pos.x, centerPos.y + pos.y);
            soundCir.setFillColor(sf::Color::Green);
            window.draw(soundCir);

            window.pushGLStates();
            ImGui::SFML::Render(window);
            window.popGLStates();

            window.display();
        }
    }
}

