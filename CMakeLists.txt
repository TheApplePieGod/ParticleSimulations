cmake_minimum_required(VERSION 3.19)

project(
    ParticleSimulations
    VERSION 0.1
    DESCRIPTION "Various particle simulations"
)

include_directories("include")
include_directories("lib/Diamond/include")
include_directories("lib/glfw-3.3.2.bin.WIN64/include")
include_directories("lib/glm")
include_directories("C:/VulkanSDK/1.2.162.1/Include")

add_compile_definitions(DEBUG_FLAG)

# build shaders as well
add_custom_command(
    OUTPUT CompileShaders
    COMMAND "CompileShaders.bat"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

file(GLOB SOURCES "src/*.cpp")
add_executable(ParticleSimulations ${SOURCES} "src/entry/main.cpp" CompileShaders)

# debug
find_library(Diamond Diamond "lib/Diamond")
target_link_libraries(ParticleSimulations "${Diamond}")

#release
# find_library(Diamond-release Diamond-release "lib/Diamond")
# target_link_libraries(ParticleSimulations "${Diamond-release}")

# dependency of diamond
find_library(glfw3 glfw3 "lib/glfw-3.3.2.bin.WIN64/lib-vc2019")
find_library(vulkan-1 vulkan-1 "C:/VulkanSDK/1.2.162.1/Lib")
target_link_libraries(ParticleSimulations "${glfw3}")
target_link_libraries(ParticleSimulations "${vulkan-1}")

# Tell C++ compiler to use C++20 features.
target_compile_features(ParticleSimulations PUBLIC cxx_std_20)