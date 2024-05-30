#define STB_IMAGE_IMPLEMENTATION
#define NO_COLLISIONS
#define VERTEX_COLOR
#include "../headers/includes.h"

class Game
{
private:
    Timer timer;
    GeometryBatch geo_batch;
    SpriteBatch spr_batch;
    float totalTime = 0.0f;
    int32_t frameCount = 0;
    int32_t lastFps = 60;
public:
    inline Game() = default;
    inline Game(Window& window)
    {
        srand(time(0));
        timer = Timer();
        geo_batch = GeometryBatch(&window);
        spr_batch = SpriteBatch(&window);
    }
    inline void MainDrawAndUpdate(Window& window)
    {
        window.Clear({125, 255, 255, 255});
        
        window.DrawText(0, 0, "FPS:" + std::to_string(lastFps), 1.5f);
    }
    inline void DrawAndUpdate(Window& window)
    {
        timer.Update();
        MainDrawAndUpdate(window);
        window.SwapBuffers();
        geo_batch.Flush();
        spr_batch.Flush();
        totalTime += timer.deltaTime;
		frameCount++;
        if (totalTime >= 1.0f)
		{
			lastFps = frameCount;
			totalTime -= 1.0f;
			frameCount = 0;
		}
    }
    inline void Terminate()
    {
        return;
    }
};

int main()
{
    Window window = Window(1024, 768);
    Game instance = Game(window);
    while(!glfwWindowShouldClose(window.handle))
    {
        window.Begin();
        instance.DrawAndUpdate(window);
        window.End();
    }
    instance.Terminate();
    return 0;
}