cmake_minimum_required(VERSION 3.19)

project(
    ParticleSimulations
    VERSION 0.1
    DESCRIPTION "Various particle simulations"
)

include_directories("include")

add_compile_definitions(DEBUG_FLAG)

# build shaders as well
add_custom_command(
    OUTPUT CompileShaders
    COMMAND "CompileShaders.bat"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

file(GLOB SOURCES "src/*.cpp")
add_executable(ParticleSimulations ${SOURCES} "src/entry/main.cpp" CompileShaders)

add_compile_definitions(DIAMOND_IMGUI)
set(VulkanBasePath "C:/VulkanSDK/1.2.162.1")
find_package(Diamond REQUIRED)
include_directories(${Diamond_DIR}/../../include)
include_directories(${Diamond_DIR}/../../include/lib)
include_directories(${Diamond_DIR}/../../include/lib/glfw/include)
include_directories("${VulkanBasePath}/Include")

target_link_libraries(ParticleSimulations Diamond::Diamond)

# Tell C++ compiler to use C++20 features.
target_compile_features(ParticleSimulations PUBLIC cxx_std_20)