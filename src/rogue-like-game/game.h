#ifndef GAME_H
#define GAME_H

#include "../headers/includes.h"

const rect mapBound = 
{
    90.0f, 160.0f, 920.0f, 650.0f
};

inline float Distance(v2f v1, v2f v2)
{
    return (v1 - v2).mag();
}

enum class pPowerupType
{
    Speed,
    Health,
    Money,
    Shield,
    None
};

inline bool InBounds(v2f position, rect rc)
{
    return position.x < rc.ex && position.x > rc.sx && position.y < rc.ey && position.y > rc.sx;
}

struct Character
{
    pPowerupType currPowerup = pPowerupType::None;
    float velocity = 150.0f;
    int health, maxHealth;
    int coins, coinMultiplier;
    v2f position;
    Horizontal direction;
    StateMachine states;
    Character()
    {
        direction = Horizontal::Norm;
        states.currentState = "Idle";

        states.states["Walking"].animator.frames.push_back(new Sprite("assets\\character\\move\\frame-1.png"));
        states.states["Walking"].animator.frames.push_back(new Sprite("assets\\character\\move\\frame-2.png"));
        states.states["Walking"].animator.frames.push_back(new Sprite("assets\\character\\move\\frame-3.png"));
        states.states["Walking"].animator.frames.push_back(new Sprite("assets\\character\\move\\frame-4.png"));
        states.states["Walking"].animator.data.duration = 0.2f;
        states.states["Walking"].animator.data.update = aUpdate::Loop;
        states.states["Walking"].keys[GLFW_KEY_W] = Key::Held;
        states.states["Walking"].keys[GLFW_KEY_A] = Key::Held;
        states.states["Walking"].keys[GLFW_KEY_S] = Key::Held;
        states.states["Walking"].keys[GLFW_KEY_D] = Key::Held;

        states.states["Idle"].animator.frames.push_back(new Sprite("assets\\character\\idle\\frame-1.png"));
        states.states["Idle"].animator.frames.push_back(new Sprite("assets\\character\\idle\\frame-2.png"));
        states.states["Idle"].animator.frames.push_back(new Sprite("assets\\character\\idle\\frame-3.png"));
        states.states["Idle"].animator.frames.push_back(new Sprite("assets\\character\\idle\\frame-4.png"));
        states.states["Idle"].animator.data.duration = 0.2f;
        states.states["Idle"].animator.data.update = aUpdate::Loop;

        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-1.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-2.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-3.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-4.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-5.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-6.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-7.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-8.png"));
        states.states["Dash"].animator.frames.push_back(new Sprite("assets\\character\\dash\\frame-9.png"));
        states.states["Dash"].animator.data.duration = 0.04f;
        states.states["Dash"].animator.data.update = aUpdate::Once;
        states.states["Dash"].keys[GLFW_KEY_LEFT_SHIFT] = Key::Pressed;
        
        states.states["Attack"].animator.frames.push_back(new Sprite("assets\\character\\attack\\frame-1.png"));
        states.states["Attack"].animator.frames.push_back(new Sprite("assets\\character\\attack\\frame-2.png"));
        states.states["Attack"].animator.frames.push_back(new Sprite("assets\\character\\attack\\frame-3.png"));
        states.states["Attack"].animator.frames.push_back(new Sprite("assets\\character\\attack\\frame-4.png"));
        states.states["Attack"].animator.data.duration = 0.1f;
        states.states["Attack"].animator.data.update = aUpdate::Once;
        states.states["Attack"].mouse[GLFW_MOUSE_BUTTON_1] = Key::Pressed;
    
        coins = 0;
    }
    inline void Movement(Window& window, float deltaTime, float speed)
    {
        if(currPowerup == pPowerupType::Speed) speed *= 2.0f;
        if(window.GetKey(GLFW_KEY_W) == Key::Held) position.y -= speed * deltaTime;
        if(window.GetKey(GLFW_KEY_S) == Key::Held) position.y += speed * deltaTime;
        if(window.GetKey(GLFW_KEY_A) == Key::Held) 
        {
            position.x -= speed * deltaTime;
            if(direction == Horizontal::Norm) direction = Horizontal::Flip;
        }
        if(window.GetKey(GLFW_KEY_D) == Key::Held) 
        {
            position.x += speed * deltaTime;
            if(direction == Horizontal::Flip) direction = Horizontal::Norm;
        }
        if(position.x < mapBound.sx) position.x += speed * deltaTime;
        if(position.x > mapBound.ex) position.x -= speed * deltaTime;
        if(position.y < mapBound.sy) position.y += speed * deltaTime;
        if(position.y > mapBound.ey) position.y -= speed * deltaTime;
    }
    inline void Dash(Window& window, float deltaTime)
    {
        float dx = 600.0f * deltaTime * (direction == Horizontal::Flip ? -1 : 1);
        if((position.x + dx) > mapBound.sx && (position.x + dx) < mapBound.ex) position.x += dx;
    }
    inline void Update(Window& window, float deltaTime)
    {
        if(currPowerup == pPowerupType::Health) health = maxHealth;
        if(states.currentState == "Walking") Movement(window, deltaTime, velocity);
        else if(states.currentState == "Dash") Dash(window, deltaTime);
        states.Update(window, deltaTime);
    }
    inline void Draw(Window& window)
    {
        states.Draw(position.x, position.y, 3.5f, window, direction, Vertical::Norm);
        window.DrawText(32, 35, "HEALTH:" + std::to_string(health), 2.0f, {255, 255, 255, 255});
        window.DrawText(32, 63, "COINS:" + std::to_string(coins), 2.0f, {255, 255, 255, 255});
    }
    inline void Serialize(std::reference_wrapper<DataNode> datanode)
    {
        datanode.get()["character"]["data"]["coins"].SetData<int>(coins, 0);
    }
    inline void Deserialize(DataNode& datanode)
    {
        coins = GetData<int>(datanode["character"]["data"]["coins"], 0).value();
    }
    inline void SetDefault()
    {
        position = 200.0f;
        health = maxHealth;
        states.currentState = "Idle";
        direction = Horizontal::Norm;
        currPowerup = pPowerupType::None;
    }
    ~Character() {}
};

struct eDef
{
    std::unordered_map<std::string, std::vector<Sprite*>> animMap;
    float size, healthBarOffset;
};

struct eGhostDef : eDef
{
    eGhostDef()
    {
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-1.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-2.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-3.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-4.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-5.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-6.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-7.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-8.png"));
        animMap["Attack"].push_back(new Sprite("assets\\enemy\\attack\\frame-9.png"));
        animMap["Idle"].push_back(new Sprite("assets\\enemy\\idle\\frame-1.png"));
        animMap["Idle"].push_back(new Sprite("assets\\enemy\\idle\\frame-2.png"));
        animMap["Move"].push_back(new Sprite("assets\\enemy\\move\\frame-1.png"));
        animMap["Move"].push_back(new Sprite("assets\\enemy\\move\\frame-2.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-1.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-2.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-3.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-4.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-5.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-6.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-7.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-8.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-9.png"));
        animMap["Dead"].push_back(new Sprite("assets\\enemy\\dead\\frame-10.png"));
        healthBarOffset = 100.0f;
        size = 4.5f;
    }
};

struct eRangedDef : eDef
{
    eRangedDef()
    {
        animMap["Attack"].push_back(new Sprite("assets\\ranged-enemy\\attack\\frame-0.png"));
        animMap["Attack"].push_back(new Sprite("assets\\ranged-enemy\\attack\\frame-1.png"));
        animMap["Attack"].push_back(new Sprite("assets\\ranged-enemy\\attack\\frame-2.png"));
        animMap["Idle"].push_back(new Sprite("assets\\ranged-enemy\\idle\\frame-1.png"));
        animMap["Idle"].push_back(new Sprite("assets\\ranged-enemy\\idle\\frame-2.png"));
        animMap["Dead"].push_back(new Sprite("assets\\ranged-enemy\\dead\\frame-0.png"));
        animMap["Dead"].push_back(new Sprite("assets\\ranged-enemy\\dead\\frame-1.png"));
        animMap["Dead"].push_back(new Sprite("assets\\ranged-enemy\\dead\\frame-2.png"));
        animMap["Dead"].push_back(new Sprite("assets\\ranged-enemy\\dead\\frame-3.png"));
        animMap["Dead"].push_back(new Sprite("assets\\ranged-enemy\\dead\\frame-4.png"));
        animMap["Dead"].push_back(new Sprite("assets\\ranged-enemy\\dead\\frame-5.png"));
        animMap["Spawn"].push_back(new Sprite("assets\\ranged-enemy\\spawn\\frame-0.png"));
        animMap["Spawn"].push_back(new Sprite("assets\\ranged-enemy\\spawn\\frame-1.png"));
        animMap["Spawn"].push_back(new Sprite("assets\\ranged-enemy\\spawn\\frame-2.png"));
        animMap["Spawn"].push_back(new Sprite("assets\\ranged-enemy\\spawn\\frame-3.png"));
        animMap["Spawn"].push_back(new Sprite("assets\\ranged-enemy\\spawn\\frame-4.png"));
        animMap["Spawn"].push_back(new Sprite("assets\\ranged-enemy\\spawn\\frame-5.png"));
        animMap["Energy"].push_back(new Sprite("assets\\ranged-enemy\\energy-ball.png"));
        healthBarOffset = 80.0f;
        size = 3.5f;
    }
};

enum class eType : uint8_t
{
    Ghost,
    Ranged
};

enum class eSpawnType : uint8_t
{
    Inside,
    Outside
};

inline void DrawHealth(float x, float y, Window& window, float width, float height, float health, float min = 0.0f, float max = 100.0f)
{
    float finalWidth = width * health / (max - min);
    window.DrawRect(x + finalWidth - width * 0.5f, y - height * 0.5f, x + width * 0.5f, y + height * 0.5f, {0, 0, 0, 255});
    window.DrawRect(x - width * 0.5f, y - height * 0.5f, x + finalWidth - width * 0.5f, y + height * 0.5f, {255, 255, 255, 255});
}

struct eCommon
{
    std::unordered_map<std::string, aData> states;
    Horizontal direction = Horizontal::Norm;
    std::string currentState;
    float health = 100.0f;
    v2f position = 0.0f;
    eType type;
    virtual void Update(eDef* def, Character& character, float deltaTime) = 0;
    virtual void Draw(eDef* def, Window& window, float deltaTime) = 0;
    virtual void GiveDamage(Character& character, float deltaTime) = 0;
    virtual void SetSpawnData(v2f position) = 0;
    inline void DrawSelf(eDef* def, Window& window, float deltaTime)
    {
        window.DrawSprite(position.x, position.y, *def->animMap[currentState][states[currentState].index], def->size, direction);
        DrawHealth(position.x, position.y - def->healthBarOffset, window, 50.0f, 10.0f, health);
    }
    inline void UpdateSelf(Character& character, eDef* def, float deltaTime)
    {
        TakeDamage(character, deltaTime);
        if(health <= 0.0f) SetState(def, "Dead");
        const std::size_t size = def->animMap[currentState].size();
        states[currentState].Update(size, deltaTime);
    }
    inline float GetDistance(Character& character)
    {
        return Distance(character.position, position);
    }
    inline void SetState(eDef* def, std::string state)
    {
        if(states[currentState].update == aUpdate::Once)
        {
            if(states[currentState].played)
            {
                state = "Idle";
                goto set;
            }
            else return;
        }
set:
        if(currentState == state || states.count(state) == 0) return;
        auto& data = states[currentState];
        const std::size_t size = def->animMap[currentState].size() - 1;
        data.totalTime = (data.reverse ? data.duration * size : 0.0f);
        data.index = (data.reverse ? size : 0);
        data.played = false;
        currentState = state;
        return;
    }
    inline void DefineState(const std::string& name, const aUpdate& update, const float duration)
    {
        if(states.count(name) != 0) return;
        states[name].duration = duration;
        states[name].update = update;
    }
    inline void TakeDamage(Character& character, float deltaTime)
    {
        if(GetDistance(character) < 100.0f)
        {
            if(character.states.currentState == "Attack") health -= 10.0f * deltaTime;
            else if(character.states.currentState == "Dash") health = 0.0f;
        }
    }
};

struct eGhost : eCommon
{
    eGhost()
    {
        type = eType::Ghost;
        DefineState("Idle", aUpdate::Loop, 0.2f);
        DefineState("Attack", aUpdate::Once, 0.2f);
        DefineState("Move", aUpdate::Loop, 0.2f);
        DefineState("Dead", aUpdate::Once, 0.2f);
        currentState = "Idle";
    }
    inline void Update(eDef* def, Character& character, float deltaTime) override
    {
        float dist = GetDistance(character);
        if(dist <= 100.0f) SetState(def, "Attack");
        else SetState(def, (!InBounds(position, mapBound) || dist < 1000.0f) ? "Move" : "Idle");
        if(currentState == "Move") Movement(character, deltaTime);
        UpdateSelf(character, def, deltaTime);
        GiveDamage(character, deltaTime);
    }
    inline void Movement(Character& character, float deltaTime, float speed = 150.0f)
    {
        direction = (character.position.x < position.x) ? Horizontal::Flip : Horizontal::Norm;
        float angle = atan2(character.position.y - position.y, character.position.x - position.x);
        position.x += cos(angle) * speed * deltaTime;
        position.y += sin(angle) * speed * deltaTime;
    }
    inline void GiveDamage(Character& character, float deltaTime) override
    {
        if(character.currPowerup == pPowerupType::Shield) return;
        if(GetDistance(character) < 100.0f && currentState == "Attack") character.health -= deltaTime;
    }
    inline void SetSpawnData(v2f pos) override
    {
        position = pos;
    }
    inline void Draw(eDef* def, Window& window, float deltaTime) override
    {
        DrawSelf(def, window, deltaTime);
    }
};

struct eBall
{
    v2f position;
    float distanceCovered = 0.0f;
    bool remove = false;
    inline void Update(Character& character, float deltaTime, float speed = 150.0f)
    {
        remove = distanceCovered > 300.0f;
        float angle = atan2(character.position.y - position.y, character.position.x - position.x);
        position.x += cos(angle) * speed * deltaTime;
        position.y += sin(angle) * speed * deltaTime;
        distanceCovered += speed * deltaTime;
    }
};

struct eRanged : eCommon
{
    eRanged()
    {
        type = eType::Ranged;
        timeSinceAttack = 0.0f;
        DefineState("Idle", aUpdate::Loop, 0.2f);
        DefineState("Spawn", aUpdate::Once, 0.2f);
        DefineState("Attack", aUpdate::Once, 0.2f);
        DefineState("Dead", aUpdate::Once, 0.2f);
        currentState = "Spawn";
        ball.remove = true;
    }
    inline void Update(eDef* def, Character& character, float deltaTime) override
    {
        timeSinceAttack += deltaTime;
        SetState(def, timeSinceAttack < 5.0f ? "Idle" : "Attack");
        if(currentState == "Attack") Attack();
        UpdateSelf(character, def, deltaTime);
        UpdateEnergyBall(character, deltaTime);
        GiveDamage(character, deltaTime);
    }
    inline void Attack()
    {
        ball.position = position;
        ball.distanceCovered = 0.0f;
        ball.remove = false;
        currentState = "Idle";
        timeSinceAttack = 0.0f;
    }
    inline void Draw(eDef* def, Window& window, float deltaTime) override
    {
        DrawSelf(def, window, deltaTime);
        DrawEnergyBall(window, def);
    }
    inline void GiveDamage(Character& character, float deltaTime) override
    {
        if(!ball.remove && Distance(ball.position, character.position) < 50.0f)
        {
            if(character.currPowerup != pPowerupType::Shield)
                character.health -= deltaTime * 3.0f;
            ball.remove = true;
        }
    }
    inline void SetSpawnData(v2f pos) override
    {
        position = pos;
        ball.position = pos;
    }
    inline void DrawEnergyBall(Window& window, eDef* def) 
    {
        if(!ball.remove) window.DrawSprite(ball.position.x, ball.position.y, *def->animMap["Energy"][0], 5.0f);
    }
    inline void UpdateEnergyBall(Character& character, float deltaTime)
    {
        if(!ball.remove) ball.Update(character, deltaTime);
    }
private:
    eBall ball;
    float timeSinceAttack;
};

struct eWrapper
{
    eCommon* enemy;
    bool remove = false;
    void Update(eDef* def, Character& character, float deltaTime)
    {
        if(enemy->currentState == "Dead") remove = enemy->states["Dead"].played;
        int coinInc = remove ? (character.currPowerup == pPowerupType::Money ? 3 : 1) : 0;
        character.coins += character.coinMultiplier * coinInc;
        enemy->Update(def, character, deltaTime);
    }
    void Draw(eDef* def, Window& window, float deltaTime)
    {
        enemy->Draw(def, window, deltaTime);
    }
};

const std::unordered_map<eType, eSpawnType> spawnMap
{
    {eType::Ghost, eSpawnType::Outside},
    {eType::Ranged, eSpawnType::Inside}
};

std::unordered_map<eType, eDef*> defMap
{
    {eType::Ghost, new eGhostDef()},
    {eType::Ranged, new eRangedDef()}
};

inline void SpawnEnemy(std::vector<eWrapper>& enemies, eType enemyType, Window& window)
{
    switch(enemyType)
    {
        case eType::Ghost: enemies.push_back({new eGhost()}); break;
        case eType::Ranged: enemies.push_back({new eRanged()}); break;
    }
    const float w = (float)window.GetWidth();
    const float h = (float)window.GetHeight();
    float x = rand(mapBound.sx, mapBound.ex); 
    float y = rand(mapBound.sy, mapBound.ey);
    switch(spawnMap.at(enemyType))
    {
        case eSpawnType::Inside: break;
        case eSpawnType::Outside:
        {
            x += rand(0, 2) ? w : -w;
            y += rand(0, 2) ? 0 : h;
        }
        break;
    }
    enemies.back().enemy->SetSpawnData({x, y});
}

enum class wSpawnSystem
{
    Spawning,
    Cooldown
};

struct eWaves
{
    float timeSinceSpawn;
    std::vector<eWrapper> enemies;
    wSpawnSystem spawnSystem;
    int currentWave, enemiesSpawned;
    inline void Reset()
    {
        timeSinceSpawn = 0.0f;
        currentWave = 1;
        enemiesSpawned = 0;
        spawnSystem = wSpawnSystem::Cooldown;
        for(auto& enemy : enemies)
        {
            delete enemy.enemy;
            enemy.enemy = nullptr;
        }
        enemies.clear();
    }
    inline void Update(Window& window, Character& character, float deltaTime)
    {
        timeSinceSpawn += deltaTime;

        switch(spawnSystem)
        {
            case wSpawnSystem::Cooldown:
            {
                if(timeSinceSpawn > 20.0f) 
                {
                    currentWave++;
                    timeSinceSpawn = 0.0f;
                    spawnSystem = wSpawnSystem::Spawning;
                }
            }
            break;
            case wSpawnSystem::Spawning:
            {
                if(timeSinceSpawn > 5.0f)
                {
                    if(enemiesSpawned < currentWave) 
                    {
                        SpawnEnemy(enemies, (eType)rand(0, 2), window);
                        timeSinceSpawn = 0.0f;
                        enemiesSpawned++;
                    }
                }
                if(enemies.empty() && enemiesSpawned == currentWave)
                {
                    timeSinceSpawn = 0.0f;
                    enemiesSpawned = 0;
                    spawnSystem = wSpawnSystem::Cooldown;
                }
            }
            break;
        }

        for(auto& enemy : enemies) enemy.Update(defMap[enemy.enemy->type], character, deltaTime);

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](eWrapper& wrapper){return wrapper.remove;}), enemies.end());
    }
    inline void Draw(Window& window, float deltaTime)
    {
        window.DrawText(window.GetWidth() * 0.5f, 30, "WAVE " + std::to_string(currentWave), 3.0f,
            (spawnSystem == wSpawnSystem::Cooldown) ? Color{255, 255, 255, 255} : Color{255, 0, 0, 255}, TextRenderMode::Middle);
    
        for(auto& enemy : enemies) enemy.Draw(defMap[enemy.enemy->type], window, deltaTime);
    }
};

struct pChest
{
    std::unordered_map<pPowerupType, Sprite*> powerups;
    v2f position = v2f(900.0f, 170.0f);
    Animator animator;
    float elapsedTime;
    enum class cUpdate
    {
        Opening,
        Closed,
        Open
    } cUpdateType = cUpdate::Closed;
    pChest()
    {
        animator.frames.push_back(new Sprite("assets\\chest\\frames\\frame-0.png"));
        animator.frames.push_back(new Sprite("assets\\chest\\frames\\frame-1.png"));
        animator.frames.push_back(new Sprite("assets\\chest\\frames\\frame-2.png"));
        powerups[pPowerupType::Health] = new Sprite("assets\\chest\\powerups\\health.png");
        powerups[pPowerupType::Speed] = new Sprite("assets\\chest\\powerups\\fast-run.png");
        powerups[pPowerupType::Shield] = new Sprite("assets\\chest\\powerups\\shield.png");
        powerups[pPowerupType::Money] = new Sprite("assets\\chest\\powerups\\money-icon.png");
        animator.data.update = aUpdate::Once;
        animator.data.duration = 0.2f;
        animator.data.Reverse(true);
        elapsedTime = 5.0f;
    }
    inline void Reset()
    {
        elapsedTime = 5.0f;
        animator.data.index = 0;
        animator.data.totalTime = 0.0f;
        animator.data.Reverse(true);
        cUpdateType = cUpdate::Closed;
    }
    inline void DrawPowerup(Character& character, Window& window, float deltaTime)
    {
        if(character.currPowerup == pPowerupType::None) return;
        float y = position.y - std::clamp(elapsedTime, 0.0f, 4.0f) * 10.0f;
        window.DrawSprite(position.x, y, *powerups[character.currPowerup], 3.0f);
    }
    inline void Draw(Character& character, Window& window, float deltaTime)
    {
        if(cUpdateType == cUpdate::Closed && Distance(character.position, position) < 100.0f && elapsedTime > 5.0f)
            window.DrawText(position.x - 100.0f, position.y - 60.0f, "Press E to open.", 1.5f, {255, 255, 255, 255});
        
        animator.Draw(position.x, position.y, 6.0f, window, Horizontal::Norm, Vertical::Norm);
        
        DrawPowerup(character, window, deltaTime);
    }
    inline void Update(Character& character, float deltaTime, bool keypressed)
    {
        switch(cUpdateType)
        {
            case cUpdate::Opening: 
            {
                animator.Update(deltaTime);
                if(animator.data.played)
                {
                    character.currPowerup = (pPowerupType)rand(0, 4);
                    cUpdateType = cUpdate::Open;
                } 
            }
            break;
            case cUpdate::Closed: 
            {
                animator.Update(deltaTime);
                elapsedTime += deltaTime;
                if(elapsedTime > 5.0f && keypressed && Distance(character.position, position) < 100.0f) 
                {
                    elapsedTime = 0.0f;
                    animator.data.Reverse(false);
                    cUpdateType = cUpdate::Opening;
                }
            }
            break;
            case cUpdate::Open:
            {
                elapsedTime += deltaTime;
                if(elapsedTime > 10.0f) 
                {
                    elapsedTime = 0.0f;
                    animator.data.Reverse(true);
                    character.currPowerup = pPowerupType::None;
                    cUpdateType = cUpdate::Closed;
                }
            } 
            break;
        }
    }
};

struct Item
{
    std::string desc;
    Sprite* sprite;
    int currLevel;
    std::vector<std::pair<int, float>> data;
    inline const float GetPower() const
    {
        return data[currLevel - 1].second;
    }
};

struct Market
{
    std::unordered_map<std::string, Item> items;
    std::vector<std::string> itemIndices;
    int currItemIndex = 0;
    v2f position;
    float size = 1.0f;
    inline void Deserialize(DataNode& datanode)
    {
        datanode["items"].nodes_foreach([&](std::pair<std::string, DataNode> p){
            itemIndices.push_back(p.first);
            items[p.first].desc = GetString(p.second["desc"], 0).value();
            items[p.first].currLevel = GetData<int>(p.second["current index"], 0).value();
            items[p.first].sprite = new Sprite(GetString(p.second["directory"], 0).value());
            p.second["price list"].data_foreach([&](Container container){
                items[p.first].data.push_back(std::make_pair(container.get<int>().value(), 0));
            });
            p.second["power"].data_indexed_for([&](Container container, std::size_t index){
                items[p.first].data[index].second = container.get<float>().value();
            });
        });
    }
    inline void Serialize(std::reference_wrapper<DataNode> datanode)
    {
        for(auto& item : items)
            datanode.get()["items"][item.first]["current index"].SetData<int>(item.second.currLevel, 0);
    }
    inline void Update(Character& character, Window& window)
    {
        if(window.GetKey(GLFW_KEY_A) == Key::Pressed) currItemIndex += (--currItemIndex < 0) ? items.size() : 0;
        if(window.GetKey(GLFW_KEY_D) == Key::Pressed) currItemIndex = (++currItemIndex) % items.size();

        if(window.GetKey(GLFW_KEY_ENTER) == Key::Pressed)
        {
            const int price = GetPrice();
            if(price != 0 && character.coins >= price)
            {
                items[itemIndices[currItemIndex]].currLevel++;
                character.coins -= price;
            }
        }
    }
    inline void Draw(Character& character, Window& window)
    {
        auto& sprite = items[itemIndices[currItemIndex]].sprite;
        const float y = position.y + sprite->height * size * 1.5f;
        window.DrawText(position.x, y, GetItemDesc(), size * 0.5f, {255, 255, 255, 255}, TextRenderMode::Middle);
        window.DrawText(10, 10, "COINS:" + std::to_string(character.coins), 2.0f, {255, 255, 255, 255});
        window.DrawSprite(position.x, position.y, *sprite, size * 1.5f);
    }
    inline std::string GetItemDesc()
    {
        std::string text;
        for(auto& c : itemIndices[currItemIndex]) text += std::toupper(c);
        text += "\nLevel " + std::to_string(items[itemIndices[currItemIndex]].currLevel) + '\n';
        text += StringifyPrice() + '\n';
        text += items[itemIndices[currItemIndex]].desc;
        return text;
    }
    inline int GetPrice()
    {
        auto& currItem = items[itemIndices[currItemIndex]];
        const std::size_t index = currItem.currLevel;
        const std::size_t size = currItem.data.size();
        return index < size ? currItem.data[index].first : 0;
    }
    inline std::string StringifyPrice()
    {
        const int price = GetPrice();
        return price == 0 ? "Max" : "Price: " + std::to_string(price);
    }
    inline void ResetCharacter(Character& character)
    {
        character.velocity = items["speed"].GetPower();
        character.maxHealth = items["health"].GetPower();
        character.coinMultiplier = items["money"].GetPower();
    }
};

#endif