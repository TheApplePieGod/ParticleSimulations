#include <ParticleSimulations/input.h>
#include <ParticleSimulations/util/basic.h>
#include <Diamond/diamond.h>
#include <string>
#include <iostream>

extern diamond* Engine;
extern input_manager* InputManager;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        InputManager->SetKeyPressed(key, true);
    else if (action == GLFW_RELEASE)
        InputManager->SetKeyPressed(key, false);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
        InputManager->SetMouseButtonPressed(button + 1, true);
    else if (action == GLFW_RELEASE)
        InputManager->SetMouseButtonPressed(button + 1, false);
}

void input_manager::Initialize()
{
    glfwSetKeyCallback(Engine->Window(), key_callback);
    glfwSetMouseButtonCallback(Engine->Window(), mouse_button_callback);
}

void input_manager::Tick()
{
    glfwPollEvents();
}

glm::vec2 input_manager::GetMouseScreenPosition()
{
    double xpos, ypos;
    glfwGetCursorPos(Engine->Window(), &xpos, &ypos);
    return glm::vec2{ xpos, ypos };
}

glm::vec2 input_manager::GetMouseWorldPosition()
{
    glm::vec2 screenSize = Engine->GetWindowSize();
    glm::vec2 screenPos = GetMouseScreenPosition();
    glm::vec2 cameraPos = glm::vec2(0.f, 0.f);
    screenPos = { 2.0f * screenPos.x / screenSize.x - 1.f, -(2.f * screenPos.y / screenSize.y - 1.f) };
    glm::vec2 worldPos = { screenPos.x * MetersToPixels(ScreenMetersX) * 0.5f + cameraPos.x, screenPos.y * MetersToPixels(ScreenMetersX) / Engine->GetAspectRatio() * 0.5f + cameraPos.y };
    return worldPos;
}

void input_manager::SetKeyPressed(int keyIndex, bool value)
{
    if (keyIndex <= 96)
    {
        int scancode = glfwGetKeyScancode(keyIndex);
        const char* name = glfwGetKeyName(keyIndex, scancode);
        if (name != nullptr)
        {
            keys[name[0]][0] = value;
            keys[name[0]][1] = value;
        }
    }
    else // non printable keys
    {
        nonPrintableKeys[keyIndex][0] = value;
        nonPrintableKeys[keyIndex][1] = value;
    }
}

void input_manager::SetMouseButtonPressed(int buttonIndex, bool value)
{
    if (buttonIndex == 1)
    {
        lmb[0] = value;
        lmb[1] = value;
    }
    else if (buttonIndex == 2)
    {
        rmb[0] = value;
        rmb[1] = value;
    }
}

bool input_manager::IsKeyPressedInternal(const char* key, int flagIndex)
{
    std::string keyName = key;
    if (keyName.length() == 1) // one element so printable characters
        return keys[key[0]][flagIndex];
    else
    {
        if (keyName == "ESCAPE")
            return nonPrintableKeys[256][flagIndex];
        if (keyName == "ENTER")
            return nonPrintableKeys[257][flagIndex];
        if (keyName == "F1")
            return nonPrintableKeys[290][flagIndex];
        if (keyName == "F2")
            return nonPrintableKeys[291][flagIndex];
        if (keyName == "F3")
            return nonPrintableKeys[292][flagIndex];
        if (keyName == "SHIFT")
            return nonPrintableKeys[340][flagIndex];
        if (keyName == "CONTROL")
            return nonPrintableKeys[341][flagIndex];
    }
    return false;
}

bool input_manager::IsKeyPressed(const char* key)
{
    return IsKeyPressedInternal(key, 0);
}

bool input_manager::IsMouseDown(int buttonIndex)
{
    if (buttonIndex == 1)
        return lmb[0];
    else if (buttonIndex == 2)
        return rmb[0];
    return false;
}

bool input_manager::WasKeyJustPressed(const char* key)
{
    return IsKeyPressedInternal(key, 1);
}

bool input_manager::WasMouseJustPressed(int buttonIndex)
{
    if (buttonIndex == 1)
        return lmb[1];
    else if (buttonIndex == 2)
        return rmb[1];
    return false;
}

void input_manager::ClearJustPressedFlags()
{
    for (const auto &p : keys)
    {
        keys[p.first][1] = false;
    }
    for (const auto &p : nonPrintableKeys)
    {
        nonPrintableKeys[p.first][1] = false;
    }

    lmb[1] = false;
    rmb[1] = false;
}