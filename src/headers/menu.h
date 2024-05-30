#ifndef MENU_H
#define MENU_H

#include "includes.h"

constexpr v2f menuElementPadding = {5.0f, 3.0f};
constexpr v2f subMenuOffset = {5.0f, 4.0f};

struct Menu
{
    TextRenderMode renderMode = TextRenderMode::Right;
    std::unordered_map<std::string, Menu> subMenuMap;
    std::vector<std::string> menuNamesVec;
    v2i cursorPosition;
    v2f menuBackgroundSize;
    v2i tableSize;
    v2f position;
    int32_t id = 0;
    v2f size = 1.0f;
    bool enabled = true;
    inline void BuildMenu();
    inline void Draw(Window& window);
    inline std::reference_wrapper<Menu> CurrentNode();
    inline Menu& operator[](const std::string& str);
};

struct MenuManager
{
    std::list<std::reference_wrapper<Menu>> subMenuList;
    inline void Open(std::reference_wrapper<Menu> menu);
    inline void Draw(Window& window);
    inline int32_t Update(Window& window);
    inline void MoveRight();
    inline void MoveLeft();
    inline void MoveDown();
    inline void MoveUp();
    inline void Clamp();
    inline void Close();
    inline int32_t MoveForward();
    inline void MoveBack();
    inline bool Empty();
};

#endif

#ifdef MENU_H
#undef MENU_H

inline Menu& Menu::operator[](const std::string& str)
{
    if(subMenuMap.count(str) == 0)
    {
        menuNamesVec.push_back(str);
        subMenuMap[str] = Menu();
    }
    return subMenuMap[str];
}

inline std::reference_wrapper<Menu> Menu::CurrentNode()
{
    return subMenuMap[menuNamesVec[cursorPosition.x * tableSize.y + cursorPosition.y]];
}

inline void Menu::Draw(Window& window)
{
    float buffer = 0.0f;
    rect backgroundRect;
    v2f currDrawPos;
    currDrawPos.x = position.x - menuBackgroundSize.x * (1.0f - textModeMap.at(renderMode));
    currDrawPos.y = position.y;
    backgroundRect.sx = currDrawPos.x - menuElementPadding.x * size.w;
    backgroundRect.ex = currDrawPos.x + menuBackgroundSize.x;
    backgroundRect.sy = currDrawPos.y - menuElementPadding.y * size.h;
    backgroundRect.ey = currDrawPos.y + menuBackgroundSize.y;
    window.DrawRect(backgroundRect.sx, backgroundRect.sy, backgroundRect.ex, backgroundRect.ey, {0, 0, 0, 255});
    window.DrawRectOutline(backgroundRect.sx, backgroundRect.sy, backgroundRect.ex, backgroundRect.ey, {255, 255, 255, 255});
    for(int i = 0; i < tableSize.x; i++)
    {
        for(int j = 0; j < tableSize.y; j++)
        {
            const int index = i * tableSize.y + j;
            if(index < menuNamesVec.size())
            {
                v2f stringSize = StringSize(menuNamesVec[index], size);
                const bool enabled = subMenuMap[menuNamesVec[index]].enabled;
                const float offset = (1.0f - textModeMap.at(renderMode)) * stringSize.x;
                const int currIndex = cursorPosition.x * tableSize.y + cursorPosition.y;
                Color color = enabled ? (index == currIndex ? Color{255, 0, 0, 255} : Color{255, 255, 255, 255}) : Color{125, 125, 125, 255};
                window.DrawText(currDrawPos.x + offset, currDrawPos.y, menuNamesVec[index], size, color, renderMode);
                currDrawPos.y += stringSize.y + menuElementPadding.y * size.h;
                buffer = std::max(buffer, stringSize.x);
            }
        }
        currDrawPos.x += buffer + menuElementPadding.x * size.w;
        currDrawPos.y = position.y;
        buffer = 0.0f;
    }
}

inline void Menu::BuildMenu()
{
    v2f buffer;
    menuBackgroundSize = 0.0f;
    for(int i = 0; i < tableSize.x; i++)
    {
        for(int j = 0; j < tableSize.y; j++)
        {
            const int index = i * tableSize.y + j;
            if(index < menuNamesVec.size())
            {
                v2f stringSize = StringSize(menuNamesVec[index], size);
                buffer.x = std::max(stringSize.x, buffer.x);
                buffer.y += stringSize.y + menuElementPadding.y * size.h;
            }
        }
        menuBackgroundSize.x += buffer.x + menuElementPadding.x * size.w;
        menuBackgroundSize.y = std::max(buffer.y, menuBackgroundSize.y);
        buffer = 0.0f;
    }
    for(auto& subMenu : subMenuMap)
        if(!subMenu.second.subMenuMap.empty())
        {
            subMenu.second.position = position + subMenuOffset * size;
            subMenu.second.position.x += (1.0f - textModeMap.at(renderMode)) * menuBackgroundSize.x;
            subMenu.second.renderMode = renderMode;
            subMenu.second.size = size;
            subMenu.second.BuildMenu();
        }
}

inline void MenuManager::Draw(Window& window)
{
    for(auto& menu : subMenuList) menu.get().Draw(window);
}

inline int32_t MenuManager::Update(Window& window)
{
    int32_t res = -1;
    if(window.GetKey(GLFW_KEY_W) == Key::Pressed) MoveUp();
    if(window.GetKey(GLFW_KEY_A) == Key::Pressed) MoveLeft();
    if(window.GetKey(GLFW_KEY_S) == Key::Pressed) MoveDown();
    if(window.GetKey(GLFW_KEY_D) == Key::Pressed) MoveRight();
    if(window.GetKey(GLFW_KEY_ENTER) == Key::Pressed) res = MoveForward();
    if(window.GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) MoveBack();
    return res;
}

inline void MenuManager::Close()
{
    subMenuList.clear();
}

inline void MenuManager::Open(std::reference_wrapper<Menu> menu)
{
    Close();
    subMenuList.push_back(menu);
}

inline int32_t MenuManager::MoveForward()
{
    if(!subMenuList.empty())
    {
        auto& currMenu = subMenuList.back().get();
        auto& currNode = currMenu.CurrentNode().get();
        if(!currNode.subMenuMap.empty() && currNode.enabled) subMenuList.push_back(currNode);
        else return currNode.id;
    }
    return -1;
}

inline void MenuManager::MoveRight()
{
    subMenuList.back().get().cursorPosition.x++;
    Clamp();
}

inline void MenuManager::MoveLeft()
{
    subMenuList.back().get().cursorPosition.x--;
    Clamp();
} 

inline void MenuManager::MoveDown()
{
    subMenuList.back().get().cursorPosition.y++;
    Clamp();
}

inline void MenuManager::MoveUp()
{
    subMenuList.back().get().cursorPosition.y--;
    Clamp();
}

inline void MenuManager::MoveBack()
{
    if(subMenuList.size() > 1) subMenuList.pop_back();
}

inline void MenuManager::Clamp()
{
    auto& currMenu = subMenuList.back().get();
    const std::size_t size = currMenu.menuNamesVec.size();
    currMenu.cursorPosition.x = std::clamp(currMenu.cursorPosition.x, 0, currMenu.tableSize.x - 1);
    currMenu.cursorPosition.y = std::clamp(currMenu.cursorPosition.y, 0, currMenu.tableSize.y - 1);
    if(currMenu.cursorPosition.y * currMenu.tableSize.x + currMenu.cursorPosition.x >= size)
    {
        currMenu.cursorPosition.x = (size - 1) / currMenu.tableSize.x;
        currMenu.cursorPosition.y = (size - 1) % currMenu.tableSize.x;
    }
}

inline bool MenuManager::Empty()
{
    return subMenuList.empty();
}

#endif