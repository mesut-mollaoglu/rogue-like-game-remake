#define STB_IMAGE_IMPLEMENTATION
#define NO_COLLISIONS
#define VERTEX_COLOR
#include "game.h"

class Game : public Window
{
private:
    enum class State
    {
        InGame,
        MainMenu,
        EndFail,
        Market,
        PauseMenu,
        QuitGame
    };
    Character character;
    WaveSystem waveController;
    Decal mapDecal;
    Chest chest;
    DataNode config;
    MenuManager<Game::State> menuManager;
    Game::State currGameState = Game::State::MainMenu;
    Decal menuBgDecal;
    SpriteBatch sprBatch;
    Market market;
    Menu<Game::State> mainMenu;
    Menu<Game::State> gameOverMenu;
    Menu<Game::State> pauseMenu;
public:
    inline void UserStart() override
    {
        srand(time(0));
        sprBatch = SpriteBatch(this);
        Deserialize(config, "datafile.txt");
        character = Character();
        character.Deserialize(config);
        mapDecal = Decal("assets\\misc\\map.png");
        menuBgDecal = Decal("assets\\UI\\menu\\background.png");
        mainMenu["Start"].SetId(Game::State::InGame);
        mainMenu["Market"].SetId(Game::State::Market);
        mainMenu["Quit"]["No"].SetId(Game::State::MainMenu);
        mainMenu["Quit"]["Yes"].SetId(Game::State::QuitGame);
        mainMenu["Quit"].SetTableSize(2, 1);
        mainMenu.SetPos(30.0f, 250.0f);
        mainMenu.SetTableSize(1, 3);
        mainMenu.SetScale(4.0f);
        mainMenu.Build();
        market.pos = GetScreenSize() * 0.5f;
        market.size = 5.0f;
        market.Deserialize(config);
        gameOverMenu["Retry"].SetId(Game::State::InGame);
        gameOverMenu["Main Menu"].SetId(Game::State::MainMenu);
        gameOverMenu.SetPos(GetScreenSize() * 0.5f);
        gameOverMenu.SetOrigin(0.5f, 0.0f);
        gameOverMenu.SetTableSize(1, 2);
        gameOverMenu.SetScale(4.0f);
        gameOverMenu.Build();
        pauseMenu["Main Menu"].SetId(Game::State::MainMenu);
        pauseMenu["Resume"].SetId(Game::State::InGame);
        pauseMenu.SetPos(GetScreenSize() * 0.5f);
        pauseMenu.SetOrigin(0.5f, 0.0f);
        pauseMenu.SetTableSize(1, 2);
        pauseMenu.SetScale(4.0f);
        pauseMenu.Build();
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
            case Game::State::MainMenu: MenuDrawAndUpdate(); break;
            case Game::State::InGame: MainDrawAndUpdate(); break;
            case Game::State::Market: MarketDrawAndUpdate(); break;
            case Game::State::EndFail: EndFailDrawAndUpdate(); break;
            case Game::State::PauseMenu: PauseDrawAndUpdate(); break;
        }
        sprBatch.Flush();
    }
    inline void MenuDrawAndUpdate()
    {
//Update
        if(menuManager.Empty()) menuManager.Open(mainMenu);
        std::optional<Game::State> state = menuManager.Update();
        if(state.has_value())
            switch(state.value())
            {
                case Game::State::InGame:
                {
                    Restart();
                    menuManager.Close();
                    currGameState = Game::State::InGame;
                }
                break;
                case Game::State::QuitGame:
                {
                    glfwSetWindowShouldClose(GetHandle(), GL_TRUE);
                }
                break;
                case Game::State::Market:
                {
                    menuManager.Close();
                    currGameState = Game::State::Market;
                }
                break;
                case Game::State::MainMenu:
                {
                    menuManager.MoveBack();
                }
                break;
                default: break;
            }
//Draw
        Clear(Colors::Black);
        menuManager.Draw();
        sprBatch.Draw(menuBgDecal, GetViewport());
    }
    inline void MarketDrawAndUpdate()
    {
//Update
        if(GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = Game::State::MainMenu;
        market.Update(character, this);
//Draw
        Clear(Colors::Black);
        SetPixelMode(PixelMode::Alpha);
        market.Draw(character, this);
        SetPixelMode(PixelMode::Normal);
    }
    inline void MainDrawAndUpdate()
    {
//Update
        if(GetKey(GLFW_KEY_ESCAPE) == Key::Pressed) currGameState = Game::State::PauseMenu;
        if(character.health <= 0) currGameState = Game::State::EndFail;
        character.Update(this);
        waveController.Update(this, character);
        chest.Update(character, this);
//Draw
        Clear(Colors::Transparent);
        sprBatch.Draw(mapDecal, GetViewport());
        SetPixelMode(PixelMode::Alpha);
        chest.Draw(character, this);
        character.Draw(this);
        waveController.Draw(this);
        SetPixelMode(PixelMode::Normal);
    }
    inline void PauseDrawAndUpdate()
    {
//Update
        if(menuManager.Empty()) menuManager.Open(pauseMenu);
        std::optional<Game::State> state = menuManager.Update();
        if(state.has_value())
            switch(state.value())
            {
                case Game::State::InGame: case Game::State::MainMenu:
                {
                    menuManager.Close();
                    currGameState = state.value();
                }
                break;
                default: break;
            }
//Draw
        Clear(Colors::Black);
        DrawText(GetWidth() * 0.5f, 100, "PAUSED", 4.0f, Colors::White, 0.5f);
        menuManager.Draw();
    }
    inline void EndFailDrawAndUpdate()
    {
//Update
        if(menuManager.Empty()) menuManager.Open(gameOverMenu);
        std::optional<Game::State> state = menuManager.Update();
        if(state.has_value())
            switch(state.value())
            {
                case Game::State::InGame:
                {
                    Restart();
                    menuManager.Close();
                    currGameState = Game::State::InGame;
                }
                break;
                case Game::State::MainMenu:
                {
                    menuManager.Close();
                    currGameState = Game::State::MainMenu;
                }
                break;
                default: break;
            }
//Draw
        Clear(Colors::Black);
        DrawText(GetWidth() * 0.5, 100, "Defeat", 6.0f, Colors::White, 0.5f);
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