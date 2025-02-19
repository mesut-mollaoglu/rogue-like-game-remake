#ifndef GAME_H
#define GAME_H

#include "../custom-game-engine/headers/includes.h"

const Rect<float> mapBound = 
{
    {90.0f, 160.0f}, {830.0f, 490.0f}
};

inline float Distance(vec2 v1, vec2 v2)
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

enum class CharacterState
{
    Walking,
    Idle,
    Attack,
    Dash
};

inline bool InBounds(vec2 pos, Rect<float> rc)
{
    return rc.Contains(pos);
}

struct Character
{
    PowerupType currPowerup = PowerupType::None;
    float velocity = 150.0f;
    int health, maxHealth;
    int coins, coinMultiplier;
    vec2 pos;
    Horizontal direction;
    StateMachine<Sprite, CharacterState> stateMachine;
    EntityDef<Sprite, CharacterState>* def;
    Character()
    {
        direction = Horizontal::Norm;
        
        def = new EntityDef<Sprite, CharacterState>();

        def->operator[](CharacterState::Walking).AddFrame("assets\\character\\move\\frame-1.png");
        def->operator[](CharacterState::Walking).AddFrame("assets\\character\\move\\frame-2.png");
        def->operator[](CharacterState::Walking).AddFrame("assets\\character\\move\\frame-3.png");
        def->operator[](CharacterState::Walking).AddFrame("assets\\character\\move\\frame-4.png");
        def->operator[](CharacterState::Idle).AddFrame("assets\\character\\idle\\frame-1.png");
        def->operator[](CharacterState::Idle).AddFrame("assets\\character\\idle\\frame-2.png");
        def->operator[](CharacterState::Idle).AddFrame("assets\\character\\idle\\frame-3.png");
        def->operator[](CharacterState::Idle).AddFrame("assets\\character\\idle\\frame-4.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-1.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-2.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-3.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-4.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-5.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-6.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-7.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-8.png");
        def->operator[](CharacterState::Dash).AddFrame("assets\\character\\dash\\frame-9.png");
        def->operator[](CharacterState::Attack).AddFrame("assets\\character\\attack\\frame-1.png");
        def->operator[](CharacterState::Attack).AddFrame("assets\\character\\attack\\frame-2.png");
        def->operator[](CharacterState::Attack).AddFrame("assets\\character\\attack\\frame-3.png");
        def->operator[](CharacterState::Attack).AddFrame("assets\\character\\attack\\frame-4.png");
        
        stateMachine.DefineState(CharacterState::Walking, UpdateRoutine::Loop, 0.2f);
        stateMachine.DefineState(CharacterState::Idle, UpdateRoutine::Loop, 0.2f);
        stateMachine.DefineState(CharacterState::Attack, UpdateRoutine::PlayOnce, 0.1f);
        stateMachine.DefineState(CharacterState::Dash, UpdateRoutine::PlayOnce, 0.04f);
        stateMachine.def = def;

        stateMachine.currStateName = CharacterState::Idle;

        coins = 0;
    }
    inline void Movement(Window& window, float speed)
    {
        if(stateMachine.currStateName != CharacterState::Walking) return;
        const float dt = window.GetDeltaTime();
        if(currPowerup == PowerupType::Speed) speed *= 2.0f;
        if(window.GetKey(GLFW_KEY_W) == Key::Held) pos.y -= speed * dt;
        if(window.GetKey(GLFW_KEY_S) == Key::Held) pos.y += speed * dt;
        if(window.GetKey(GLFW_KEY_A) == Key::Held) 
        {
            pos.x -= speed * dt;
            if(direction == Horizontal::Norm) direction = Horizontal::Flip;
        }
        if(window.GetKey(GLFW_KEY_D) == Key::Held) 
        {
            pos.x += speed * dt;
            if(direction == Horizontal::Flip) direction = Horizontal::Norm;
        }
        if(pos.x < mapBound.pos.x) pos.x += speed * dt;
        if(pos.x > mapBound.pos.x + mapBound.size.x) pos.x -= speed * dt;
        if(pos.y < mapBound.pos.y) pos.y += speed * dt;
        if(pos.y > mapBound.pos.y + mapBound.size.y) pos.y -= speed * dt;
    }
    inline void Dash(Window& window)
    {
        if(stateMachine.currStateName != CharacterState::Dash) return;
        float dx = 600.0f * window.GetDeltaTime() * (direction == Horizontal::Flip ? -1 : 1);
        if((pos.x + dx) > mapBound.pos.x && (pos.x + dx) < mapBound.pos.x + mapBound.size.x) pos.x += dx;
    }
    inline void Update(Window& window)
    {
        if(currPowerup == PowerupType::Health) health = maxHealth;
        if(window.GetKey(GLFW_KEY_LEFT_SHIFT) == Key::Pressed) stateMachine.SetState(CharacterState::Dash);
        if(window.GetMouseButton(GLFW_MOUSE_BUTTON_1) == Key::Pressed) stateMachine.SetState(CharacterState::Attack);
        stateMachine.SetState((window.GetKey(GLFW_KEY_A) == Key::Held || window.GetKey(GLFW_KEY_W) == Key::Held || 
        window.GetKey(GLFW_KEY_S) == Key::Held || window.GetKey(GLFW_KEY_D) == Key::Held) ? CharacterState::Walking : CharacterState::Idle);
        Movement(window, velocity);
        Dash(window);
        stateMachine.Update(window.GetDeltaTime());
    }
    inline void Draw(Window& window)
    {
        stateMachine.Draw(window, pos, 3.5f, 0.0f, direction);
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
        stateMachine.currStateName = CharacterState::Idle;
        direction = Horizontal::Norm;
        currPowerup = PowerupType::None;
    }
    ~Character() {}
};

enum class EnemyState
{
    Idle,
    Attack,
    Move,
    Dead,
    Spawn
};

struct EnemyDef
{
    EntityDef<Sprite, EnemyState> enemyDef;
    float size, healthBarOffset;
    Sprite sprEnergyBall;
};

struct GhostDef : EnemyDef
{
    GhostDef()
    {
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-1.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-2.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-3.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-4.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-5.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-6.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-7.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-8.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\enemy\\attack\\frame-9.png");
        enemyDef[EnemyState::Idle].AddFrame("assets\\enemy\\idle\\frame-1.png");
        enemyDef[EnemyState::Idle].AddFrame("assets\\enemy\\idle\\frame-2.png");
        enemyDef[EnemyState::Move].AddFrame("assets\\enemy\\move\\frame-1.png");
        enemyDef[EnemyState::Move].AddFrame("assets\\enemy\\move\\frame-2.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-1.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-2.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-3.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-4.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-5.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-6.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-7.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-8.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-9.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\enemy\\dead\\frame-10.png");
        healthBarOffset = 100.0f;
        size = 4.5f;
    }
};

struct RangedDef : EnemyDef
{
    RangedDef()
    {
        enemyDef[EnemyState::Attack].AddFrame("assets\\ranged-enemy\\attack\\frame-0.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\ranged-enemy\\attack\\frame-1.png");
        enemyDef[EnemyState::Attack].AddFrame("assets\\ranged-enemy\\attack\\frame-2.png");
        enemyDef[EnemyState::Idle].AddFrame("assets\\ranged-enemy\\idle\\frame-1.png");
        enemyDef[EnemyState::Idle].AddFrame("assets\\ranged-enemy\\idle\\frame-2.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\ranged-enemy\\dead\\frame-0.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\ranged-enemy\\dead\\frame-1.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\ranged-enemy\\dead\\frame-2.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\ranged-enemy\\dead\\frame-3.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\ranged-enemy\\dead\\frame-4.png");
        enemyDef[EnemyState::Dead].AddFrame("assets\\ranged-enemy\\dead\\frame-5.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\ranged-enemy\\spawn\\frame-0.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\ranged-enemy\\spawn\\frame-1.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\ranged-enemy\\spawn\\frame-2.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\ranged-enemy\\spawn\\frame-3.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\ranged-enemy\\spawn\\frame-4.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\ranged-enemy\\spawn\\frame-5.png");
        sprEnergyBall = Sprite("assets\\ranged-enemy\\energy-ball.png");
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
    window.DrawRect(x - width * 0.5f, y - height * 0.5f, finalWidth, height, Color(255, 255, 255, 255));
    window.DrawRect(x + finalWidth - width * 0.5f, y - height * 0.5f, width - finalWidth, height, Color(0, 0, 0, 255));
}

std::unordered_map<EnemyType, EnemyDef*> defMap
{
    {EnemyType::Ghost, new GhostDef()},
    {EnemyType::Ranged, new RangedDef()}
};

struct EnemyBase
{
    StateMachine<Sprite, EnemyState> stateMachine;
    Horizontal direction = Horizontal::Norm;
    float health = 100.0f;
    vec2 pos = 0.0f;
    EnemyType type;
    virtual void Update(Character& character, float delta) = 0;
    virtual void Draw(Window& window) = 0;
    virtual void GiveDamage(Character& character, float delta) = 0;
    virtual void SetSpawnData(const vec2& pos) = 0;
    inline void DrawSelf(Window& window)
    {
        stateMachine.Draw(window, pos, defMap.at(type)->size, 0.0f, direction);
        DrawHealth(pos.x, pos.y - defMap.at(type)->healthBarOffset, window, 50.0f, 10.0f, health);
    }
    inline void UpdateSelf(Character& character, float delta)
    {
        if(health <= 0.0f) stateMachine.SetState(EnemyState::Dead);
        else TakeDamage(character, delta);
        stateMachine.Update(delta);
    }
    inline float GetDistance(Character& character)
    {
        return Distance(character.pos, pos);
    }
    inline void TakeDamage(Character& character, float delta)
    {
        if(GetDistance(character) < 100.0f)
        {
            if(character.stateMachine.currStateName == CharacterState::Attack) health -= 10.0f * delta;
            else if(character.stateMachine.currStateName == CharacterState::Dash) health = 0.0f;
        }
    }
};

struct Ghost : EnemyBase
{
    Ghost()
    {
        type = EnemyType::Ghost;
        stateMachine.DefineState(EnemyState::Idle, UpdateRoutine::Loop, 0.2f);
        stateMachine.DefineState(EnemyState::Attack, UpdateRoutine::PlayOnce, 0.2f);
        stateMachine.DefineState(EnemyState::Move, UpdateRoutine::Loop, 0.2f);
        stateMachine.DefineState(EnemyState::Dead, UpdateRoutine::PlayOnce, 0.2f);
        stateMachine.def = &defMap.at(type)->enemyDef;
        stateMachine.currStateName = EnemyState::Idle;
    }
    inline void Update(Character& character, float delta) override
    {
        float dist = GetDistance(character);
        if(dist <= 100.0f) stateMachine.SetState(EnemyState::Attack);
        else stateMachine.SetState((!InBounds(pos, mapBound) || dist < 1000.0f) ? EnemyState::Move : EnemyState::Idle);
        if(stateMachine.currStateName == EnemyState::Move) Movement(character, delta);
        UpdateSelf(character, delta);
        GiveDamage(character, delta);
    }
    inline void Movement(Character& character, float delta, float speed = 150.0f)
    {
        direction = (character.pos.x < pos.x) ? Horizontal::Flip : Horizontal::Norm;
        float angle = std::atan2(character.pos.y - pos.y, character.pos.x - pos.x);
        pos.x += std::cos(angle) * speed * delta;
        pos.y += std::sin(angle) * speed * delta;
    }
    inline void GiveDamage(Character& character, float delta) override
    {
        if(health <= 0.0f && character.currPowerup == PowerupType::Shield) return;
        if(GetDistance(character) < 100.0f &&  stateMachine.currStateName == EnemyState::Attack) character.health -= delta;
    }
    inline void SetSpawnData(const vec2& pos) override
    {
        this->pos = pos;
    }
    inline void Draw(Window& window) override
    {
        DrawSelf(window);
    }
};

struct EnergyBall
{
    vec2 pos;
    float distanceCovered = 0.0f;
    bool remove = false;
    inline void Update(Character& character, float delta, float speed = 150.0f)
    {
        remove = distanceCovered > 300.0f;
        float angle = std::atan2(character.pos.y - pos.y, character.pos.x - pos.x);
        pos.x += std::cos(angle) * speed * delta;
        pos.y += std::sin(angle) * speed * delta;
        distanceCovered += speed * delta;
    }
};

struct Ranged : EnemyBase
{
    Ranged()
    {
        type = EnemyType::Ranged;
        timeSinceAttack = 0.0f;
        stateMachine.DefineState(EnemyState::Idle, UpdateRoutine::Loop, 0.2f);
        stateMachine.DefineState(EnemyState::Spawn, UpdateRoutine::PlayOnce, 0.2f);
        stateMachine.DefineState(EnemyState::Attack, UpdateRoutine::PlayOnce, 0.2f);
        stateMachine.DefineState(EnemyState::Dead, UpdateRoutine::PlayOnce, 0.2f);
        stateMachine.def = &defMap.at(type)->enemyDef;
        stateMachine.currStateName = EnemyState::Spawn;
        ball.remove = true;
    }
    inline void Update(Character& character, float delta) override
    {
        timeSinceAttack += delta;
        stateMachine.SetState(timeSinceAttack < 5.0f ? EnemyState::Idle : EnemyState::Attack);
        if(stateMachine.currStateName == EnemyState::Attack) Attack();
        UpdateSelf(character, delta);
        UpdateEnergyBall(character, delta);
        GiveDamage(character, delta);
    }
    inline void Attack()
    {
        ball.pos = pos;
        ball.distanceCovered = 0.0f;
        ball.remove = false;
        stateMachine.currStateName = EnemyState::Idle;
        timeSinceAttack = 0.0f;
    }
    inline void Draw(Window& window) override
    {
        DrawSelf(window);
        DrawEnergyBall(window);
    }
    inline void GiveDamage(Character& character, float delta) override
    {
        if(health <= 0.0f || character.currPowerup == PowerupType::Shield || ball.remove || Distance(ball.pos, character.pos) > 50.0f) return;
        character.health -= delta * 10.0f;
        ball.remove = true;
    }
    inline void SetSpawnData(const vec2& pos) override
    {
        this->pos = ball.pos = pos;
    }
    inline void DrawEnergyBall(Window& window) 
    {
        if(!ball.remove) 
            window.DrawSprite(ball.pos.x, ball.pos.y, defMap.at(type)->sprEnergyBall, 5.0f);
    }
    inline void UpdateEnergyBall(Character& character, float delta)
    {
        if(!ball.remove) 
            ball.Update(character, delta);
    }
private:
    EnergyBall ball;
    float timeSinceAttack;
};

struct EnemyWrapper
{
    EnemyBase* enemy;
    bool remove = false;
    void Update(Character& character, float delta)
    {
        if(enemy->stateMachine.currStateName == EnemyState::Dead) remove = enemy->stateMachine[EnemyState::Dead].playedOnce;
        int coinInc = remove ? (character.currPowerup == PowerupType::Money ? 3 : 1) : 0;
        character.coins += character.coinMultiplier * coinInc;
        enemy->Update(character, delta);
    }
    void Draw(Window& window)
    {
        enemy->Draw(window);
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
    const vec2 scrSize = window.GetScreenSize();
    vec2 pos = RndPointInRect(mapBound);
    switch(spawnMap.at(enemyType))
    {
        case eSpawnType::Inside: break;
        case eSpawnType::Outside:
        {
            pos.x += rand(0, 2) ? scrSize.w : -scrSize.w;
            pos.y += rand(0, 2) ? 0 : scrSize.h;
        }
        break;
    }
    enemies.back().enemy->SetSpawnData(pos);
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
    inline void Update(Window& window, Character& character)
    {
        timeSinceSpawn += window.GetDeltaTime();

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

        for(auto& enemy : enemies) enemy.Update(character, window.GetDeltaTime());

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](EnemyWrapper& wrapper){return wrapper.remove;}), enemies.end());
    }
    inline void Draw(Window& window)
    {
        window.DrawText(window.GetWidth() * 0.5f, 30, "WAVE " + std::to_string(currentWave), 3.0f,
            (spawnSystem == SpawnSystemState::Cooldown) ? Color{255, 255, 255, 255} : Color{255, 0, 0, 255}, {0.5f, 0.0f});
    
        for(auto& enemy : enemies) enemy.Draw(window);
    }
};

struct Chest
{
    std::unordered_map<PowerupType, Sprite> powerups;
    vec2 pos = vec2(900.0f, 170.0f);
    Animator<Sprite> animator;
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
        animator.data.update = UpdateRoutine::PlayOnce;
        animator.data.duration = 0.2f;
        animator.data.Reverse();
        elapsedTime = 5.0f;
    }
    inline void Reset()
    {
        elapsedTime = 5.0f;
        animator.data.currFrameIndex = 0ull;
        animator.data.elapsedTime = 0.0f;
        animator.data.reverse = true;
        cUpdateType = cUpdate::Closed;
    }
    inline void DrawPowerup(Character& character, Window& window)
    {
        if(character.currPowerup == PowerupType::None) return;
        float y = pos.y - std::clamp(elapsedTime, 0.0f, 4.0f) * 10.0f;
        window.DrawSprite(pos.x, y, powerups[character.currPowerup], 3.0f);
    }
    inline void Draw(Character& character, Window& window)
    {
        if(cUpdateType == cUpdate::Closed && Distance(character.pos, pos) < 100.0f && elapsedTime > 5.0f)
            window.DrawText(pos.x - 100.0f, pos.y - 60.0f, "Press E to open.", 1.5f, {255, 255, 255, 255});
        
        window.DrawSprite(pos.x, pos.y, animator.GetFrame(), 6.0f);
        
        DrawPowerup(character, window);
    }
    inline void Update(Character& character, Window& window)
    {
        const float dt = window.GetDeltaTime();
        switch(cUpdateType)
        {
            case cUpdate::Opening: 
            {
                animator.Update(dt);
                if(animator.data.playedOnce)
                {
                    character.currPowerup = (PowerupType)rand(0, 4);
                    cUpdateType = cUpdate::Open;
                } 
            }
            break;
            case cUpdate::Closed: 
            {
                animator.Update(dt);
                elapsedTime += dt;
                if(elapsedTime > 5.0f && window.GetKey(GLFW_KEY_E) == Key::Pressed && Distance(character.pos, pos) < 100.0f) 
                {
                    elapsedTime = 0.0f;
                    animator.data.Reverse();
                    cUpdateType = cUpdate::Opening;
                }
            }
            break;
            case cUpdate::Open:
            {
                elapsedTime += dt;
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
    vec2 pos;
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
        window.DrawText(pos.x, y, GetItemDesc(), size * 0.5f, {255, 255, 255, 255}, 0.5f);
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