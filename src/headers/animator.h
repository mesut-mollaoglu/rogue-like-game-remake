#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "includes.h"

using steady_clock = std::chrono::steady_clock;
typedef std::chrono::time_point<steady_clock> time_point;

enum class aUpdate
{
    Once,
    Loop
};

struct aData
{
    std::size_t index = 0;
    bool played = false;
    bool reverse = false;
	float totalTime = 0.0f;
    aUpdate update = aUpdate::Loop;
    float duration;
    inline void Update(const std::size_t& size, float deltaTime)
    {
        switch(update)
        {
            case aUpdate::Loop: 
            {
                totalTime += (reverse ? -1.0f : 1.0f) * deltaTime;
                index = (std::size_t)(totalTime / duration) % size;
                index += (index < 0) ? size : 0;
            }
            break;
            case aUpdate::Once: 
            {
                if(!played) 
                {
                    totalTime += deltaTime * (reverse ? -1.0f : 1.0f);
                    index = (std::size_t)(totalTime / duration);
                    played = index == (reverse ? 0 : size - 1);
                }
            }
            break;
        }
    }
    inline void Reverse(bool rev)
    {
        if(reverse != rev)
        {
            reverse = rev;
            played = false;
        }
    }
};

struct Animator 
{
	inline void Update(float deltaTime) 
    {
		data.Update(frames.size(), deltaTime);
	}
    inline void Draw(float x, float y, v2f size, Window& window, Horizontal hor, Vertical ver)
    {
        window.DrawSprite(x, y, *frames[data.index], size, hor, ver);
    }
	std::vector<Sprite*> frames;
    aData data;
};

#endif