cmake_minimum_required(VERSION 3.17)
project(TestPlace)

set(CMAKE_CXX_STANDARD 17)

add_executable(TestPlace main.cpp)

find_package(re2)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
find_package(ImGui-SFML REQUIRED)
target_link_libraries(${PROJECT_NAME}
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
    ImGui-SFML::ImGui-SFML
#    re2::re2
)
