#ifndef COMMON_H
#define COMMON_H

#include "includes.h"

struct Shape
{
    v2f position = 0.0f;
    Color color = {0, 0, 0, 255};
    float currentAngle = 0.0f;
    virtual void Draw(Window& window, DrawMode drawMode = DrawMode::Normal) { return; }
    virtual void Rotate(float angle) { return; }
    virtual void SetRotation(float angle)
    {
        if(currentAngle != angle)
            Rotate(angle - currentAngle);
    }
};

struct Rect : Shape
{
    v2f size;
    Rect() = default;
    Rect(float x, float y, float w, float h, Color color) : size({w, h}) 
    {
        position = {x, y};
        this->color = color;
    }
    void Rotate(float angle) override 
    {
        currentAngle += angle;
    }
    void Draw(Window& window, DrawMode drawMode = DrawMode::Normal) override
    {
        const DrawMode currDrawMode = window.GetDrawMode();
        window.SetDrawMode(drawMode);
        const v2f half_size = size * 0.5f;
        if(currentAngle == 0.0f)
        {
            window.DrawRect(position.x - half_size.w, position.y - half_size.h, position.x + half_size.w, position.y + half_size.y, color);
            return;
        }
        const v2f pos1 = rotate(currentAngle, -half_size);
        const v2f pos2 = rotate(currentAngle, {half_size.w, -half_size.h});
        const v2f pos3 = rotate(currentAngle, {-half_size.w, half_size.h});
        const v2f pos4 = rotate(currentAngle, half_size);
        window.DrawTriangle(pos1.x + position.x, pos1.y + position.y, pos2.x + position.x, position.y + pos2.y, position.x + pos3.x, position.y + pos3.y, color);
        window.DrawTriangle(pos2.x + position.x, position.y + pos2.y, pos3.x + position.x, pos3.y + position.y, pos4.x + position.x, pos4.y + position.y, color);
        window.SetDrawMode(currDrawMode);
    }
};

struct Circle : Shape
{
    float radius;
    Circle() = default;
    Circle(float x, float y, float radius, Color color) : radius(radius) 
    {
        position = {x, y};
        this->color = color;
    }
    void Rotate(float angle) override
    {
        return;
    }
    void Draw(Window& window, DrawMode drawMode = DrawMode::Normal) override
    {
        const DrawMode currDrawMode = window.GetDrawMode();
        window.SetDrawMode(drawMode);
        window.DrawCircle(position.x, position.y, radius, color);
        window.SetDrawMode(currDrawMode);
    }
    void SetRotation(float angle) override
    {
        return;
    }
};

struct Triangle : Shape
{
    v2f vertices[3], rotated[3];
    Triangle() = default;
    Triangle(const v2f v1, const v2f v2, const v2f v3, v2f pos, Color color) 
    {
        rotated[0] = vertices[0] = v1;
        rotated[1] = vertices[1] = v2;
        rotated[2] = vertices[2] = v3;
        position = pos;
        this->color = color;
    }
    void Draw(Window& window, DrawMode drawMode = DrawMode::Normal) override
    {
        const DrawMode currDrawMode = window.GetDrawMode();
        window.SetDrawMode(drawMode);
        window.DrawTriangle(rotated[0].x + position.x,
        rotated[0].y + position.y, rotated[1].x + position.x,
        rotated[1].y + position.y, rotated[2].x + position.x,
        rotated[2].y + position.y, color);
        window.SetDrawMode(currDrawMode);
    }
    void Rotate(float angle) override
    {
        currentAngle += angle;
        rotated[0] = rotate(currentAngle, vertices[0]);
        rotated[1] = rotate(currentAngle, vertices[1]);
        rotated[2] = rotate(currentAngle, vertices[2]);
    }
};

enum class pShape
{
    Rect,
    Circle,
    Triangle,
    Pixel
};

enum class pMode
{
    Normal,
    Replay
};

enum class pBehaviour
{
    Sinusoidal,
    Directional
};

struct particle
{
    Color color;
    float rotation;
    float size;
    float velocity;
    float gravity;
    float maxDistance;
    int currentFrame = 0;
    int maxFrame;
    bool dead;
    pMode mode;
    pShape shape;
    pBehaviour behaviour;
    v2f startPos;
    v2f currentPos;
};

struct pData
{
    rect rect;
    float maxSize;
    float minSize;
    float maxAngle;
    float minAngle;
    float maxSpeed;
    float minSpeed;
    std::vector<Color> colors;
};

inline void equilateral(Triangle& triangle, float size)
{
    const float m = 0.577350269f;
    triangle.rotated[0] = triangle.vertices[0] = v2f(0.0f, m * size);
    triangle.rotated[1] = triangle.vertices[1] = v2f(size * 0.5f, -m * size);
    triangle.rotated[2] = triangle.vertices[2] = v2f(-size * 0.5f, -m * size);
};

struct pSystem
{
    std::vector<particle> particles;
    bool pause = false;
    v2f position;
    pSystem() = default;
    pSystem(float x, float y) 
    {
        pause = true;
        position = {x, y};
    }
    inline void Generate(pData& data, int size, pMode mode, pShape shape, pBehaviour behaviour, float gravity, float distance, int frame)
    {
        for(int i = 0; i < size; i++)
        {
            particles.push_back({
                data.colors[rand(0, (int)data.colors.size())],
                rand(data.minAngle, data.maxAngle),
                rand(data.minSize, data.maxSize),
                rand(data.minSpeed, data.maxSpeed),
                gravity, distance, 0, frame,
                false, mode, shape, behaviour,
                {
                    rand(data.rect.sx, data.rect.ex) + position.x,
                    rand(data.rect.sy, data.rect.ey) + position.y
                },
            });
            particles.back().currentPos = particles.back().startPos;
        }
    }
    inline void Update(int replayAmount)
    {
        if(pause) return;
        for(auto& p : particles)
        {
            switch(p.mode)
            {
                case pMode::Replay:
                {
                    if(p.maxDistance <= 0.0f && p.currentFrame % p.maxFrame == 0)
                        p.currentPos = p.startPos;
                    p.dead = (p.currentFrame == p.maxFrame * replayAmount);
                }
                break;
                case pMode::Normal:
                {
                    p.dead = p.currentFrame > p.maxFrame;
                }
                break;
            }

            switch(p.behaviour)
            {
                case pBehaviour::Directional:
                {
                    p.currentPos.x += cos(p.rotation) * p.velocity;
                    p.currentPos.y += sin(p.rotation) * p.velocity;
                }
                break;
                case pBehaviour::Sinusoidal:
                {
                    p.currentPos.x += cos(p.rotation) * p.velocity;
                    p.currentPos.y += sin(p.rotation) * p.velocity;
                    p.currentPos.x += cos(p.currentFrame * 0.2f) * p.velocity;
                    p.currentPos.y += sin(p.currentFrame * 0.2f) * p.velocity;
                }
                break;
            }

            p.currentFrame++;

            float x = p.currentPos.x - p.startPos.x;
            float y = p.currentPos.y - p.startPos.y;

            if(p.maxDistance > 0.0f && std::hypot(x, y) > p.maxDistance)
            {
                if(p.mode != pMode::Normal)
                    p.currentPos = p.startPos;
                else
                    p.dead = true;
            }   
                          
            p.currentPos.y += p.gravity;
        }

        particles.erase(std::remove_if(particles.begin(), particles.end(), [&](particle& p){return p.dead;}), particles.end());
    }
    inline void Draw(Window& window, DrawMode drawMode = DrawMode::Normal)
    {
        if(!pause)
            for(auto& p : particles)
                switch(p.shape)
                {
                    case pShape::Rect:
                    {
                        Rect rect;
                        rect.color = p.color;
                        rect.size.h = rect.size.w = p.size;
                        rect.position = p.currentPos;
                        rect.Rotate(p.velocity);
                        rect.Draw(window, drawMode);
                    }
                    break;
                    case pShape::Circle:
                    {
                        Circle circle;
                        circle.color = p.color;
                        circle.radius = p.size;
                        circle.position = p.currentPos;
                        circle.Draw(window, drawMode);
                    }
                    break;
                    case pShape::Triangle:
                    {
                        Triangle triangle;
                        triangle.color = p.color;
                        triangle.position = p.currentPos;
                        equilateral(triangle, p.size);
                        triangle.Rotate(p.velocity);
                        triangle.Draw(window, drawMode);
                    }
                    case pShape::Pixel: 
                    {
                        window.SetPixel(p.currentPos.x, p.currentPos.y, p.color);
                    }
                    break;
                }
    }
};

struct Timer
{
    time_point now;
    float deltaTime;
    inline Timer()
    {
        now = steady_clock::now();
    }
    inline void Update()
    {
        deltaTime = std::chrono::duration<float>(steady_clock::now() - now).count();
        now = steady_clock::now();
    }
};

#endif