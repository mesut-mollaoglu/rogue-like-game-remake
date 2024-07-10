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

class Game
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
    inline Game() = default;
    inline Game(Window& window)
    {
        srand(time(0));

        batch = SpriteBatch(&window);

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

        market.pos = window.GetScrSize() * 0.5f;
        market.size = 5.0f;
        market.Deserialize(config);

        ps.pause = false;

        gameOverMenu["Retry"].id = (int32_t)Gamestate::Main;
        gameOverMenu["Main Menu"].id = (int32_t)Gamestate::MainMenu;
        gameOverMenu.position = window.GetScrSize() * 0.5f;
        gameOverMenu.textOffset = 0.5f;
        gameOverMenu.tableSize = {1, 2};
        gameOverMenu.size = 4.0f;
        gameOverMenu.BuildMenu();

        pauseMenu["Main Menu"].id = (int32_t)Gamestate::MainMenu;
        pauseMenu["Resume"].id = (int32_t)Gamestate::Main;
        pauseMenu.position = window.GetScrSize() * 0.5f;
        pauseMenu.textOffset = 0.5f;
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
    inline void DrawAndUpdate(Window& window)
    {
        window.Begin();
        switch(currGameState)
        {
            case Gamestate::MainMenu: MenuDrawAndUpdate(window); break;
            case Gamestate::Main: MainDrawAndUpdate(window); break;
            case Gamestate::Market: MarketDrawAndUpdate(window); break;
            case Gamestate::EndFail: EndFailDrawAndUpdate(window); break;
            case Gamestate::Pause: PauseDrawAndUpdate(window); break;
        }
        batch.Flush();
        window.End();
    }
    inline void MenuDrawAndUpdate(Window& window)
    {
        if(menuManager.Empty()) menuManager.Open(menu);

        Gamestate id = (Gamestate)menuManager.Update(window);

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
                glfwSetWindowShouldClose(window.handle, GL_TRUE);
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
        
        window.Clear({0, 0, 0, 255});

        menuManager.Draw(window);
        
        batch.Draw(menuBackgroundSprite, window.GetViewport()); 
    }
    inline void MarketDrawAndUpdate(Window& window)
    {
        if(window.GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = Gamestate::MainMenu;

        market.Update(character, window);

        window.Clear({0, 0, 0, 255});

        window.pixelMode = PixelMode::Mask;

        market.Draw(character, window);

        window.pixelMode = PixelMode::Normal;
    }
    inline void MainDrawAndUpdate(Window& window)
    {
        if(window.GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = Gamestate::Pause;

        if(character.health <= 0) currGameState = Gamestate::EndFail;
        
        character.Update(window);

        waveController.Update(window, character);
        
        chest.Update(character, window);

        window.Clear({0, 0, 0, 0});

        batch.Draw(mapSprite, window.GetViewport());

        window.pixelMode = PixelMode::Mask;
 
        chest.Draw(character, window);

        character.Draw(window);

        waveController.Draw(window);

        window.pixelMode = PixelMode::Normal;
    }
    inline void PauseDrawAndUpdate(Window& window)
    {
        if(menuManager.Empty()) menuManager.Open(pauseMenu);

        Gamestate id = (Gamestate)menuManager.Update(window);

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

        window.Clear({0, 0, 0, 255});

        window.DrawText(window.GetWidth() * 0.5f, 100, "PAUSED", 4.0f, {255, 255, 255, 255}, 0.5f);

        menuManager.Draw(window);
    }
    inline void EndFailDrawAndUpdate(Window& window)
    {
        if(menuManager.Empty()) menuManager.Open(gameOverMenu);

        Gamestate id = (Gamestate)menuManager.Update(window);

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

        window.Clear({0, 0, 0, 255});

        window.DrawText(window.GetWidth() * 0.5, 100, "You lost", 6.0f, {255, 255, 255, 255}, 0.5f);

        menuManager.Draw(window);
    }
    inline void End()
    {
        character.Serialize(config);
        market.Serialize(config);
        Serialize(config, "datafile.txt");
    }
};

int main()
{
    Window window = Window(1024, 768);
    Game instance = Game(window);
    while(!glfwWindowShouldClose(window.handle))
        instance.DrawAndUpdate(window);
    instance.End();
    return 0;
}