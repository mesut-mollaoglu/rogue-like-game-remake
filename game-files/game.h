#ifndef GAME_H
#define GAME_H

#include "../custom-game-engine/headers/includes.h"

const Rect mapBound = 
{
    90.0f, 160.0f, 920.0f, 650.0f
};

inline float Distance(v2f v1, v2f v2)
{
    return (v1 - v2).mag();
}

enum class PowerupType
{
    Speed,
    Health,
    Money,
    Shield,
    None
};

inline bool InBounds(v2f pos, Rect rc)
{
    return rc.Contains(pos);
}

struct Character
{
    PowerupType currPowerup = PowerupType::None;
    float velocity = 150.0f;
    int health, maxHealth;
    int coins, coinMultiplier;
    v2f pos;
    Horizontal direction;
    StateMachine stateMachine;
    Character()
    {
        direction = Horizontal::Norm;
        stateMachine.currState = "Idle";

        stateMachine["Walking"].animator.AddFrame("assets\\character\\move\\frame-1.png");
        stateMachine["Walking"].animator.AddFrame("assets\\character\\move\\frame-2.png");
        stateMachine["Walking"].animator.AddFrame("assets\\character\\move\\frame-3.png");
        stateMachine["Walking"].animator.AddFrame("assets\\character\\move\\frame-4.png");
        stateMachine["Walking"].animator.data.duration = 0.2f;
        stateMachine["Walking"].animator.data.update = AnimUpdate::Loop;
        stateMachine["Walking"].keys[GLFW_KEY_W] = Key::Held;
        stateMachine["Walking"].keys[GLFW_KEY_A] = Key::Held;
        stateMachine["Walking"].keys[GLFW_KEY_S] = Key::Held;
        stateMachine["Walking"].keys[GLFW_KEY_D] = Key::Held;

        stateMachine["Idle"].animator.AddFrame("assets\\character\\idle\\frame-1.png");
        stateMachine["Idle"].animator.AddFrame("assets\\character\\idle\\frame-2.png");
        stateMachine["Idle"].animator.AddFrame("assets\\character\\idle\\frame-3.png");
        stateMachine["Idle"].animator.AddFrame("assets\\character\\idle\\frame-4.png");
        stateMachine["Idle"].animator.data.duration = 0.2f;
        stateMachine["Idle"].animator.data.update = AnimUpdate::Loop;

        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-1.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-2.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-3.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-4.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-5.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-6.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-7.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-8.png");
        stateMachine["Dash"].animator.AddFrame("assets\\character\\dash\\frame-9.png");
        stateMachine["Dash"].animator.data.duration = 0.04f;
        stateMachine["Dash"].animator.data.update = AnimUpdate::Once;
        stateMachine["Dash"].keys[GLFW_KEY_LEFT_SHIFT] = Key::Pressed;
        
        stateMachine["Attack"].animator.AddFrame("assets\\character\\attack\\frame-1.png");
        stateMachine["Attack"].animator.AddFrame("assets\\character\\attack\\frame-2.png");
        stateMachine["Attack"].animator.AddFrame("assets\\character\\attack\\frame-3.png");
        stateMachine["Attack"].animator.AddFrame("assets\\character\\attack\\frame-4.png");
        stateMachine["Attack"].animator.data.duration = 0.1f;
        stateMachine["Attack"].animator.data.update = AnimUpdate::Once;
        stateMachine["Attack"].mouse[GLFW_MOUSE_BUTTON_1] = Key::Pressed;
    
        coins = 0;
    }
    inline void Movement(Window& window, float deltaTime, float speed)
    {
        if(currPowerup == PowerupType::Speed) speed *= 2.0f;
        if(window.GetKey(GLFW_KEY_W) == Key::Held) pos.y -= speed * deltaTime;
        if(window.GetKey(GLFW_KEY_S) == Key::Held) pos.y += speed * deltaTime;
        if(window.GetKey(GLFW_KEY_A) == Key::Held) 
        {
            pos.x -= speed * deltaTime;
            if(direction == Horizontal::Norm) direction = Horizontal::Flip;
        }
        if(window.GetKey(GLFW_KEY_D) == Key::Held) 
        {
            pos.x += speed * deltaTime;
            if(direction == Horizontal::Flip) direction = Horizontal::Norm;
        }
        if(pos.x < mapBound.sx) pos.x += speed * deltaTime;
        if(pos.x > mapBound.ex) pos.x -= speed * deltaTime;
        if(pos.y < mapBound.sy) pos.y += speed * deltaTime;
        if(pos.y > mapBound.ey) pos.y -= speed * deltaTime;
    }
    inline void Dash(Window& window, float deltaTime)
    {
        float dx = 600.0f * deltaTime * (direction == Horizontal::Flip ? -1 : 1);
        if((pos.x + dx) > mapBound.sx && (pos.x + dx) < mapBound.ex) pos.x += dx;
    }
    inline void Update(Window& window, float deltaTime)
    {
        if(currPowerup == PowerupType::Health) health = maxHealth;
        if(stateMachine.currState == "Walking") Movement(window, deltaTime, velocity);
        else if(stateMachine.currState == "Dash") Dash(window, deltaTime);
        stateMachine.Update(window, deltaTime);
    }
    inline void Draw(Window& window)
    {
        stateMachine.Draw(window, pos.x, pos.y, 3.5f, direction);
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
        pos = 200.0f;
        health = maxHealth;
        stateMachine.currState = "Idle";
        direction = Horizontal::Norm;
        currPowerup = PowerupType::None;
    }
    ~Character() {}
};

struct EnemyDef
{
    EntityDef enemyDef;
    float size, healthBarOffset;
};

struct eGhostDef : EnemyDef
{
    eGhostDef()
    {
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-1.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-2.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-3.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-4.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-5.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-6.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-7.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-8.png");
        enemyDef["Attack"].AddFrame("assets\\enemy\\attack\\frame-9.png");
        enemyDef["Idle"].AddFrame("assets\\enemy\\idle\\frame-1.png");
        enemyDef["Idle"].AddFrame("assets\\enemy\\idle\\frame-2.png");
        enemyDef["Move"].AddFrame("assets\\enemy\\move\\frame-1.png");
        enemyDef["Move"].AddFrame("assets\\enemy\\move\\frame-2.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-1.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-2.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-3.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-4.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-5.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-6.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-7.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-8.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-9.png");
        enemyDef["Dead"].AddFrame("assets\\enemy\\dead\\frame-10.png");
        healthBarOffset = 100.0f;
        size = 4.5f;
    }
};

struct eRangedDef : EnemyDef
{
    eRangedDef()
    {
        enemyDef["Attack"].AddFrame("assets\\ranged-enemy\\attack\\frame-0.png");
        enemyDef["Attack"].AddFrame("assets\\ranged-enemy\\attack\\frame-1.png");
        enemyDef["Attack"].AddFrame("assets\\ranged-enemy\\attack\\frame-2.png");
        enemyDef["Idle"].AddFrame("assets\\ranged-enemy\\idle\\frame-1.png");
        enemyDef["Idle"].AddFrame("assets\\ranged-enemy\\idle\\frame-2.png");
        enemyDef["Dead"].AddFrame("assets\\ranged-enemy\\dead\\frame-0.png");
        enemyDef["Dead"].AddFrame("assets\\ranged-enemy\\dead\\frame-1.png");
        enemyDef["Dead"].AddFrame("assets\\ranged-enemy\\dead\\frame-2.png");
        enemyDef["Dead"].AddFrame("assets\\ranged-enemy\\dead\\frame-3.png");
        enemyDef["Dead"].AddFrame("assets\\ranged-enemy\\dead\\frame-4.png");
        enemyDef["Dead"].AddFrame("assets\\ranged-enemy\\dead\\frame-5.png");
        enemyDef["Spawn"].AddFrame("assets\\ranged-enemy\\spawn\\frame-0.png");
        enemyDef["Spawn"].AddFrame("assets\\ranged-enemy\\spawn\\frame-1.png");
        enemyDef["Spawn"].AddFrame("assets\\ranged-enemy\\spawn\\frame-2.png");
        enemyDef["Spawn"].AddFrame("assets\\ranged-enemy\\spawn\\frame-3.png");
        enemyDef["Spawn"].AddFrame("assets\\ranged-enemy\\spawn\\frame-4.png");
        enemyDef["Spawn"].AddFrame("assets\\ranged-enemy\\spawn\\frame-5.png");
        enemyDef["Energy"].AddFrame("assets\\ranged-enemy\\energy-ball.png");
        healthBarOffset = 80.0f;
        size = 3.5f;
    }
};

enum class EnemyType : uint8_t
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
    window.DrawRect(x + finalWidth - width * 0.5f, y - height * 0.5f, width, height, {0, 0, 0, 255});
    window.DrawRect(x - width * 0.5f, y - height * 0.5f, finalWidth - width, height, {255, 255, 255, 255});
}

std::unordered_map<EnemyType, EnemyDef*> defMap
{
    {EnemyType::Ghost, new eGhostDef()},
    {EnemyType::Ranged, new eRangedDef()}
};

struct EnemyBase
{
    EntityStateMachine stateMachine;
    Horizontal direction = Horizontal::Norm;
    float health = 100.0f;
    v2f pos = 0.0f;
    EnemyType type;
    virtual void Update(Character& character, float deltaTime) = 0;
    virtual void Draw(Window& window, float deltaTime) = 0;
    virtual void GiveDamage(Character& character, float deltaTime) = 0;
    virtual void SetSpawnData(const v2f& pos) = 0;
    inline void DrawSelf(Window& window, float deltaTime)
    {
        stateMachine.Draw(window, pos.x, pos.y, defMap.at(type)->size, direction);
        DrawHealth(pos.x, pos.y - defMap.at(type)->healthBarOffset, window, 50.0f, 10.0f, health);
    }
    inline void UpdateSelf(Character& character, float deltaTime)
    {
        TakeDamage(character, deltaTime);
        if(health <= 0.0f) stateMachine.SetState("Dead");
        stateMachine.Update(deltaTime);
    }
    inline float GetDistance(Character& character)
    {
        return Distance(character.pos, pos);
    }
    inline void TakeDamage(Character& character, float deltaTime)
    {
        if(GetDistance(character) < 100.0f)
        {
            if(character.stateMachine.currState == "Attack") health -= 10.0f * deltaTime;
            else if(character.stateMachine.currState == "Dash") health = 0.0f;
        }
    }
};

struct Ghost : EnemyBase
{
    Ghost()
    {
        type = EnemyType::Ghost;
        stateMachine.DefineState("Idle", AnimUpdate::Loop, 0.2f);
        stateMachine.DefineState("Attack", AnimUpdate::Once, 0.2f);
        stateMachine.DefineState("Move", AnimUpdate::Loop, 0.2f);
        stateMachine.DefineState("Dead", AnimUpdate::Once, 0.2f);
        stateMachine.def = &defMap.at(type)->enemyDef;
        stateMachine.currState = "Idle";
    }
    inline void Update(Character& character, float deltaTime) override
    {
        float dist = GetDistance(character);
        if(dist <= 100.0f) stateMachine.SetState("Attack");
        else stateMachine.SetState((!InBounds(pos, mapBound) || dist < 1000.0f) ? "Move" : "Idle");
        if(stateMachine.currState == "Move") Movement(character, deltaTime);
        UpdateSelf(character, deltaTime);
        GiveDamage(character, deltaTime);
    }
    inline void Movement(Character& character, float deltaTime, float speed = 150.0f)
    {
        direction = (character.pos.x < pos.x) ? Horizontal::Flip : Horizontal::Norm;
        float angle = std::atan2(character.pos.y - pos.y, character.pos.x - pos.x);
        pos.x += std::cos(angle) * speed * deltaTime;
        pos.y += std::sin(angle) * speed * deltaTime;
    }
    inline void GiveDamage(Character& character, float deltaTime) override
    {
        if(character.currPowerup == PowerupType::Shield) return;
        if(GetDistance(character) < 100.0f &&  stateMachine.currState == "Attack") character.health -= deltaTime;
    }
    inline void SetSpawnData(const v2f& pos) override
    {
        this->pos = pos;
    }
    inline void Draw(Window& window, float deltaTime) override
    {
        DrawSelf(window, deltaTime);
    }
};

struct EnergyBall
{
    v2f pos;
    float distanceCovered = 0.0f;
    bool remove = false;
    inline void Update(Character& character, float deltaTime, float speed = 150.0f)
    {
        remove = distanceCovered > 300.0f;
        float angle = std::atan2(character.pos.y - pos.y, character.pos.x - pos.x);
        pos.x += std::cos(angle) * speed * deltaTime;
        pos.y += std::sin(angle) * speed * deltaTime;
        distanceCovered += speed * deltaTime;
    }
};

struct Ranged : EnemyBase
{
    Ranged()
    {
        type = EnemyType::Ranged;
        timeSinceAttack = 0.0f;
        stateMachine.DefineState("Idle", AnimUpdate::Loop, 0.2f);
        stateMachine.DefineState("Spawn", AnimUpdate::Once, 0.2f);
        stateMachine.DefineState("Attack", AnimUpdate::Once, 0.2f);
        stateMachine.DefineState("Dead", AnimUpdate::Once, 0.2f);
        stateMachine.def = &defMap.at(type)->enemyDef;
        stateMachine.currState = "Spawn";
        ball.remove = true;
    }
    inline void Update(Character& character, float deltaTime) override
    {
        timeSinceAttack += deltaTime;
        stateMachine.SetState(timeSinceAttack < 5.0f ? "Idle" : "Attack");
        if(stateMachine.currState == "Attack") Attack();
        UpdateSelf(character, deltaTime);
        UpdateEnergyBall(character, deltaTime);
        GiveDamage(character, deltaTime);
    }
    inline void Attack()
    {
        ball.pos = pos;
        ball.distanceCovered = 0.0f;
        ball.remove = false;
        stateMachine.currState = "Idle";
        timeSinceAttack = 0.0f;
    }
    inline void Draw(Window& window, float deltaTime) override
    {
        DrawSelf(window, deltaTime);
        DrawEnergyBall(window);
    }
    inline void GiveDamage(Character& character, float deltaTime) override
    {
        if(!ball.remove && Distance(ball.pos, character.pos) < 50.0f)
        {
            if(character.currPowerup != PowerupType::Shield)
                character.health -= deltaTime * 3.0f;
            ball.remove = true;
        }
    }
    inline void SetSpawnData(const v2f& pos) override
    {
        this->pos = ball.pos = pos;
    }
    inline void DrawEnergyBall(Window& window) 
    {
        if(!ball.remove) 
            window.DrawSprite(ball.pos.x, ball.pos.y, stateMachine.GetFrameList("Energy")[0], 5.0f);
    }
    inline void UpdateEnergyBall(Character& character, float deltaTime)
    {
        if(!ball.remove) 
            ball.Update(character, deltaTime);
    }
private:
    EnergyBall ball;
    float timeSinceAttack;
};

struct EnemyWrapper
{
    EnemyBase* enemy;
    bool remove = false;
    void Update(Character& character, float deltaTime)
    {
        if(enemy->stateMachine.currState == "Dead") remove = enemy->stateMachine["Dead"].played;
        int coinInc = remove ? (character.currPowerup == PowerupType::Money ? 3 : 1) : 0;
        character.coins += character.coinMultiplier * coinInc;
        enemy->Update(character, deltaTime);
    }
    void Draw(Window& window, float deltaTime)
    {
        enemy->Draw(window, deltaTime);
    }
};

const std::unordered_map<EnemyType, eSpawnType> spawnMap
{
    {EnemyType::Ghost, eSpawnType::Outside},
    {EnemyType::Ranged, eSpawnType::Inside}
};

inline void SpawnEnemy(std::vector<EnemyWrapper>& enemies, EnemyType enemyType, Window& window)
{
    switch(enemyType)
    {
        case EnemyType::Ghost: enemies.push_back({new Ghost()}); break;
        case EnemyType::Ranged: enemies.push_back({new Ranged()}); break;
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

enum class SpawnSystemState
{
    Spawning,
    Cooldown
};

struct WaveSystem
{
    float timeSinceSpawn;
    std::vector<EnemyWrapper> enemies;
    SpawnSystemState spawnSystem;
    int currentWave, enemiesSpawned;
    inline void Reset()
    {
        timeSinceSpawn = 0.0f;
        currentWave = 1;
        enemiesSpawned = 0;
        spawnSystem = SpawnSystemState::Cooldown;
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
            case SpawnSystemState::Cooldown:
            {
                if(timeSinceSpawn > 2.0f) 
                {
                    currentWave++;
                    timeSinceSpawn = 0.0f;
                    spawnSystem = SpawnSystemState::Spawning;
                }
            }
            break;
            case SpawnSystemState::Spawning:
            {
                if(timeSinceSpawn > 5.0f)
                {
                    if(enemiesSpawned < currentWave) 
                    {
                        SpawnEnemy(enemies, (EnemyType)rand(0, 2), window);
                        timeSinceSpawn = 0.0f;
                        enemiesSpawned++;
                    }
                }
                if(enemies.empty() && enemiesSpawned == currentWave)
                {
                    timeSinceSpawn = 0.0f;
                    enemiesSpawned = 0;
                    spawnSystem = SpawnSystemState::Cooldown;
                }
            }
            break;
        }

        for(auto& enemy : enemies) enemy.Update(character, deltaTime);

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](EnemyWrapper& wrapper){return wrapper.remove;}), enemies.end());
    }
    inline void Draw(Window& window, float deltaTime)
    {
        window.DrawText(window.GetWidth() * 0.5f, 30, "WAVE " + std::to_string(currentWave), 3.0f,
            (spawnSystem == SpawnSystemState::Cooldown) ? Color{255, 255, 255, 255} : Color{255, 0, 0, 255}, TextRenderMode::Middle);
    
        for(auto& enemy : enemies) enemy.Draw(window, deltaTime);
    }
};

struct Chest
{
    std::unordered_map<PowerupType, Sprite> powerups;
    v2f pos = v2f(900.0f, 170.0f);
    Animator animator;
    float elapsedTime;
    enum class cUpdate
    {
        Opening,
        Closed,
        Open
    } cUpdateType = cUpdate::Closed;
    Chest()
    {
        animator.AddFrame("assets\\chest\\frames\\frame-0.png");
        animator.AddFrame("assets\\chest\\frames\\frame-1.png");
        animator.AddFrame("assets\\chest\\frames\\frame-2.png");
        powerups[PowerupType::Health] = Sprite("assets\\chest\\powerups\\health.png");
        powerups[PowerupType::Speed] = Sprite("assets\\chest\\powerups\\fast-run.png");
        powerups[PowerupType::Shield] = Sprite("assets\\chest\\powerups\\shield.png");
        powerups[PowerupType::Money] = Sprite("assets\\chest\\powerups\\money-icon.png");
        animator.data.update = AnimUpdate::Once;
        animator.data.duration = 0.2f;
        animator.data.Reverse();
        elapsedTime = 5.0f;
    }
    inline void Reset()
    {
        elapsedTime = 5.0f;
        animator.data.index = 0;
        animator.data.totalTime = 0.0f;
        cUpdateType = cUpdate::Closed;
    }
    inline void DrawPowerup(Character& character, Window& window, float deltaTime)
    {
        if(character.currPowerup == PowerupType::None) return;
        float y = pos.y - std::clamp(elapsedTime, 0.0f, 4.0f) * 10.0f;
        window.DrawSprite(pos.x, y, powerups[character.currPowerup], 3.0f);
    }
    inline void Draw(Character& character, Window& window, float deltaTime)
    {
        if(cUpdateType == cUpdate::Closed && Distance(character.pos, pos) < 100.0f && elapsedTime > 5.0f)
            window.DrawText(pos.x - 100.0f, pos.y - 60.0f, "Press E to open.", 1.5f, {255, 255, 255, 255});
        
        window.DrawSprite(pos.x, pos.y, animator.GetFrame(), 6.0f);
        
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
                    character.currPowerup = (PowerupType)rand(0, 4);
                    cUpdateType = cUpdate::Open;
                } 
            }
            break;
            case cUpdate::Closed: 
            {
                animator.Update(deltaTime);
                elapsedTime += deltaTime;
                if(elapsedTime > 5.0f && keypressed && Distance(character.pos, pos) < 100.0f) 
                {
                    elapsedTime = 0.0f;
                    animator.data.Reverse();
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
                    animator.data.Reverse();
                    character.currPowerup = PowerupType::None;
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
    Sprite sprite;
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
    v2f pos;
    float size = 1.0f;
    inline void Deserialize(DataNode& datanode)
    {
        datanode["items"].nodes_foreach([&](std::pair<std::string, DataNode> p){
            itemIndices.push_back(p.first);
            items[p.first].desc = GetString(p.second["desc"], 0).value();
            items[p.first].currLevel = GetData<int>(p.second["current index"], 0).value();
            items[p.first].sprite = Sprite(GetString(p.second["directory"], 0).value());
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
        const float y = pos.y + sprite.height * size * 1.5f;
        window.DrawText(pos.x, y, GetItemDesc(), size * 0.5f, {255, 255, 255, 255}, TextRenderMode::Middle);
        window.DrawText(10, 10, "COINS:" + std::to_string(character.coins), 2.0f, {255, 255, 255, 255});
        window.DrawSprite(pos.x, pos.y, sprite, size * 1.5f);
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