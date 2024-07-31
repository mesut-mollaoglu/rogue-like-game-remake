#define STB_IMAGE_IMPLEMENTATION
#define NO_COLLISIONS
#define VERTEX_COLOR
#include "game.h"

enum class Gamestate
{
    Main,
    MainMenu,
    EndFail,
    Market,
    Pause,
    Exit
};

class Game : public Window
{
private:
    Character character;
    WaveSystem waveController;
    Decal mapSprite;
    Chest chest;
    ParticleSystem ps;
    DataNode config;
    MenuManager menuManager;
    Gamestate currGameState = Gamestate::MainMenu;
    Decal menuBackgroundSprite;
    SpriteBatch batch;
    Menu menu;
    Market market;
    Menu gameOverMenu;
    Menu pauseMenu;
public:
    inline void UserStart() override
    {
        srand(time(0));

        batch = SpriteBatch(this);

        Deserialize(config, "datafile.txt");

        character = Character();
        character.Deserialize(config);

        mapSprite = Decal("assets\\misc\\map.png");
        menuBackgroundSprite = Decal("assets\\UI\\menu\\background.png");

        menu["Start"].id = (int32_t)Gamestate::Main;
        menu["Market"].id = (int32_t)Gamestate::Market;
        menu["Exit"]["No"].id = (int32_t)Gamestate::MainMenu;
        menu["Exit"]["Yes"].id = (int32_t)Gamestate::Exit;
        menu["Exit"].tableSize = {2, 1};
        menu.position = {30.0f, 250.0f};
        menu.tableSize = {1, 3};
        menu.size = 4.0f;
        menu.BuildMenu();

        market.pos = GetScrSize() * 0.5f;
        market.size = 5.0f;
        market.Deserialize(config);

        ps.pause = false;

        gameOverMenu["Retry"].id = (int32_t)Gamestate::Main;
        gameOverMenu["Main Menu"].id = (int32_t)Gamestate::MainMenu;
        gameOverMenu.position = GetScrSize() * 0.5f;
        gameOverMenu.textOrigin = {0.5f, 0.0f};
        gameOverMenu.tableSize = {1, 2};
        gameOverMenu.size = 4.0f;
        gameOverMenu.BuildMenu();

        pauseMenu["Main Menu"].id = (int32_t)Gamestate::MainMenu;
        pauseMenu["Resume"].id = (int32_t)Gamestate::Main;
        pauseMenu.position = GetScrSize() * 0.5f;
        pauseMenu.textOrigin = {0.5f, 0.0f};
        pauseMenu.tableSize = {1, 2};
        pauseMenu.size = 4.0f;
        pauseMenu.BuildMenu();

        waveController.Reset();

        menuManager.Close();

        Restart();
    }
    inline void Restart()
    {
        character.SetDefault();

        waveController.Reset();

        chest.Reset();

        market.ResetCharacter(character);

        ps.Clear();
    }
    inline void UserUpdate() override
    {
        switch(currGameState)
        {
            case Gamestate::MainMenu: MenuDrawAndUpdate(); break;
            case Gamestate::Main: MainDrawAndUpdate(); break;
            case Gamestate::Market: MarketDrawAndUpdate(); break;
            case Gamestate::EndFail: EndFailDrawAndUpdate(); break;
            case Gamestate::Pause: PauseDrawAndUpdate(); break;
        }
        batch.Flush();
    }
    inline void MenuDrawAndUpdate()
    {
        if(menuManager.Empty()) menuManager.Open(menu);

        Gamestate id = (Gamestate)menuManager.Update(*this);

        switch(id)
        {
            case Gamestate::Main:
            {
                Restart();
                menuManager.Close();
                currGameState = id;
            }
            break;
            case Gamestate::Exit:
            {
                glfwSetWindowShouldClose(handle, GL_TRUE);
            }
            break;
            case Gamestate::Market:
            {
                menuManager.Close();
                currGameState = id;
            }
            break;
            case Gamestate::MainMenu:
            {
                menuManager.MoveBack();
            }
            break;
            default: break;
        }
        
        Clear({0, 0, 0, 255});

        menuManager.Draw(*this);
        
        batch.Draw(menuBackgroundSprite, GetViewport()); 
    }
    inline void MarketDrawAndUpdate()
    {
        if(GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = Gamestate::MainMenu;

        market.Update(character, *this);

        Clear({0, 0, 0, 255});

        pixelMode = PixelMode::Mask;

        market.Draw(character, *this);

        pixelMode = PixelMode::Normal;
    }
    inline void MainDrawAndUpdate()
    {
        if(GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = Gamestate::Pause;

        if(character.health <= 0) currGameState = Gamestate::EndFail;
        
        character.Update(*this);

        waveController.Update(*this, character);
        
        chest.Update(character, *this);

        Clear({0, 0, 0, 0});

        batch.Draw(mapSprite, GetViewport());

        pixelMode = PixelMode::Mask;
 
        chest.Draw(character, *this);

        character.Draw(*this);

        waveController.Draw(*this);

        pixelMode = PixelMode::Normal;
    }
    inline void PauseDrawAndUpdate()
    {
        if(menuManager.Empty()) menuManager.Open(pauseMenu);

        Gamestate id = (Gamestate)menuManager.Update(*this);

        switch(id)
        {
            case Gamestate::Main: case Gamestate::MainMenu:
            {
                menuManager.Close();
                currGameState = id;
            }
            break;
            default: break;
        }

        Clear({0, 0, 0, 255});

        DrawText(GetWidth() * 0.5f, 100, "PAUSED", 4.0f, {255, 255, 255, 255}, 0.5f);

        menuManager.Draw(*this);
    }
    inline void EndFailDrawAndUpdate()
    {
        if(menuManager.Empty()) menuManager.Open(gameOverMenu);

        Gamestate id = (Gamestate)menuManager.Update(*this);

        switch(id)
        {
            case Gamestate::Main:
            {
                Restart();
                menuManager.Close();
                currGameState = id;
            }
            break;
            case Gamestate::MainMenu:
            {
                menuManager.Close();
                currGameState = id;
            }
            break;
            default: break;
        }

        Clear({0, 0, 0, 255});

        DrawText(GetWidth() * 0.5, 100, "You lost", 6.0f, {255, 255, 255, 255}, 0.5f);

        menuManager.Draw(*this);
    }
    inline void Terminate()
    {
        character.Serialize(config);
        market.Serialize(config);
        Serialize(config, "datafile.txt");
    }
};

int main()
{
    Game instance;
    instance.Start(1024, 768);
    instance.Terminate();
    return 0;
}