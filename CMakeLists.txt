cmake_minimum_required(VERSION 3.20.0)
set (CMAKE_CXX_STANDARD 20)

project(audio_visualizer)
add_compile_definitions(CRT_SECURE_NO_WARNINGS)

# CONFIG INFORMATION
set(vkIncDir "") # SET VULKAN INCLUDE DIRECTORY E.G. .../VulkanSDK/1.3.239.0/Include
set(vkLibDir "") # SET VULKAN LIBRARY DIRECTORY 
set(glfwLibDir "") #SET GLFW LIBRARY DIRECTORY

list(APPEND LibsList "${vkLibDir}/vulkan-1.lib" "${glfwLibDir}/glfw3.lib")

set(CMAKE_SOURCE_DIR "${MY_ROOT}/src")
include_directories("${MY_ROOT}/src")
include_directories("${vkIncDir}")
include_directories("${glmIncDir}/glm")
include_directories("${glmIncDir}/vk_video")
list(APPEND sourcesList "src/main.cpp" "src/renderer.cpp" "src/renderer.hpp" "src/external/glfw3.h" "${vkIncDir}/vulkan/vulkan.h" "src/external/miniaudio.h" "${vkIncDir}/glm/glm.hpp")
add_executable(${PROJECT_NAME} ${sourcesList})
list(APPEND libsDirList ${vkLibDir} ${glfwLibDir})
link_directories(${PROJECT_NAME} ${libsDirList})
target_link_libraries(${PROJECT_NAME} ${LibsList})