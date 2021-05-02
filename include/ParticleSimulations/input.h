#pragma once
#include <map>
#include <glm/vec2.hpp>

struct GLFWwindow;
class input_manager
{
public:
    void Initialize();
    void Tick(); // called every frame
    void SetKeyPressed(int asciiIndex, bool value);
    void SetMouseButtonPressed(int buttonIndex, bool value);  // lmb is 1, rmb is 2, etc
    bool IsKeyPressed(const char* key);
    bool IsMouseDown(int buttonIndex);
    bool WasKeyJustPressed(const char* key);
    bool WasMouseJustPressed(int buttonIndex);
    void ClearJustPressedFlags(); // call at the end of every frame before buffer swap
    glm::vec2 GetMouseScreenPosition(); // [0, 0] top left, [1, 1] bottom right
    glm::vec2 GetMouseWorldPosition();

private:
    // bool[]: pressed, justPressed
    std::map<char, bool[2]> keys;
    std::map<int, bool[2]> nonPrintableKeys;

    bool lmb[2];
    bool rmb[2];
    
    bool IsKeyPressedInternal(const char* key, int flagIndex);
};