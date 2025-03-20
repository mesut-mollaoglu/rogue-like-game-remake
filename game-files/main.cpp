#define STB_IMAGE_IMPLEMENTATION
#define NO_COLLISIONS
#define VERTEX_COLOR
#include "game.h"

enum class GameState
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
    DataNode config;
    MenuManager<GameState> menuManager;
    GameState currGameState = GameState::MainMenu;
    Decal menuBackgroundSprite;
    SpriteBatch batch;
    Menu<GameState> menu;
    Market market;
    Menu<GameState> gameOverMenu;
    Menu<GameState> pauseMenu;
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

        menu["Start"].SetId(GameState::Main);
        menu["Market"].SetId(GameState::Market);
        menu["Exit"]["No"].SetId(GameState::MainMenu);
        menu["Exit"]["Yes"].SetId(GameState::Exit);
        menu["Exit"].SetTableSize(2, 1);
        menu.SetPos(30.0f, 250.0f);
        menu.SetTableSize(1, 3);
        menu.SetScale(4.0f);
        menu.BuildMenu();

        market.pos = GetScreenSize() * 0.5f;
        market.size = 5.0f;
        market.Deserialize(config);

        gameOverMenu["Retry"].SetId(GameState::Main);
        gameOverMenu["Main Menu"].SetId(GameState::MainMenu);
        gameOverMenu.SetPos(GetScreenSize() * 0.5f);
        gameOverMenu.SetOrigin(0.5f, 0.0f);
        gameOverMenu.SetTableSize(1, 2);
        gameOverMenu.SetScale(4.0f);
        gameOverMenu.BuildMenu();

        pauseMenu["Main Menu"].SetId(GameState::MainMenu);
        pauseMenu["Resume"].SetId(GameState::Main);
        pauseMenu.SetPos(GetScreenSize() * 0.5f);
        pauseMenu.SetOrigin(0.5f, 0.0f);
        pauseMenu.SetTableSize(1, 2);
        pauseMenu.SetScale(4.0f);
        pauseMenu.BuildMenu();

        waveController.Reset();

        menuManager.SetWindowHandle(this);
        menuManager.Close();

        Restart();
    }
    inline void Restart()
    {
        character.SetDefault();

        waveController.Reset();

        chest.Reset();

        market.ResetCharacter(character);
    }
    inline void UserUpdate() override
    {
        switch(currGameState)
        {
            case GameState::MainMenu: MenuDrawAndUpdate(); break;
            case GameState::Main: MainDrawAndUpdate(); break;
            case GameState::Market: MarketDrawAndUpdate(); break;
            case GameState::EndFail: EndFailDrawAndUpdate(); break;
            case GameState::Pause: PauseDrawAndUpdate(); break;
        }
        batch.Flush();
    }
    inline void MenuDrawAndUpdate()
    {
        if(menuManager.Empty()) menuManager.Open(menu);

        std::optional<GameState> state = menuManager.Update();

        if(state.has_value())
            switch(state.value())
            {
                case GameState::Main:
                {
                    Restart();
                    menuManager.Close();
                    currGameState = GameState::Main;
                }
                break;
                case GameState::Exit:
                {
                    glfwSetWindowShouldClose(GetHandle(), GL_TRUE);
                }
                break;
                case GameState::Market:
                {
                    menuManager.Close();
                    currGameState = GameState::Market;
                }
                break;
                case GameState::MainMenu:
                {
                    menuManager.MoveBack();
                }
                break;
                default: break;
            }
        
        Clear(Colors::Black);

        menuManager.Draw();
        
        batch.Draw(menuBackgroundSprite, GetViewport()); 
    }
    inline void MarketDrawAndUpdate()
    {
        if(GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = GameState::MainMenu;

        market.Update(character, this);

        Clear({0, 0, 0, 255});

        SetPixelMode(PixelMode::Alpha);

        market.Draw(character, this);

        SetPixelMode(PixelMode::Normal);
    }
    inline void MainDrawAndUpdate()
    {
        if(GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = GameState::Pause;

        if(character.health <= 0) currGameState = GameState::EndFail;
        
        character.Update(this);

        waveController.Update(this, character);
        
        chest.Update(character, this);

        Clear(Colors::Transparent);

        batch.Draw(mapSprite, GetViewport());

        SetPixelMode(PixelMode::Alpha);
 
        chest.Draw(character, this);

        character.Draw(this);

        waveController.Draw(this);

        SetPixelMode(PixelMode::Normal);
    }
    inline void PauseDrawAndUpdate()
    {
        if(menuManager.Empty()) menuManager.Open(pauseMenu);

        std::optional<GameState> state = menuManager.Update();

        if(state.has_value())
            switch(state.value())
            {
                case GameState::Main: case GameState::MainMenu:
                {
                    menuManager.Close();
                    currGameState = state.value();
                }
                break;
                default: break;
            }

        Clear({0, 0, 0, 255});

        DrawText(GetWidth() * 0.5f, 100, "PAUSED", 4.0f, {255, 255, 255, 255}, 0.5f);

        menuManager.Draw();
    }
    inline void EndFailDrawAndUpdate()
    {
        if(menuManager.Empty()) menuManager.Open(gameOverMenu);

        std::optional<GameState> state = menuManager.Update();

        if(state.has_value())
            switch(state.value())
            {
                case GameState::Main:
                {
                    Restart();
                    menuManager.Close();
                    currGameState = GameState::Main;
                }
                break;
                case GameState::MainMenu:
                {
                    menuManager.Close();
                    currGameState = GameState::MainMenu;
                }
                break;
                default: break;
            }

        Clear({0, 0, 0, 255});

        DrawText(GetWidth() * 0.5, 100, "You lost", 6.0f, {255, 255, 255, 255}, 0.5f);

        menuManager.Draw();
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
    instance.Start(1024, 768, "Rogue-like-game");
    instance.Terminate();
    return 0;
}