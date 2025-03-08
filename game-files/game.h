#ifndef GAME_H
#define GAME_H

#include "../custom-game-engine/headers/includes.h"

constexpr Rect<float> mapBound = {{90.0f, 160.0f}, {830.0f, 490.0f}};

inline float Distance(const vec2& v1, const vec2& v2)
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

inline bool InBounds(const vec2& pos, const Rect<float>& rc)
{
    return rc.Contains(pos);
}

struct Character
{
    PowerupType currPowerup = PowerupType::None;
    float speed = 150.0f;
    int health, maxHealth;
    int coins, coinMultiplier;
    vec2 pos;
    bool facesRight = true;
    StateMachine<Sprite, CharacterState> stateMachine;
    EntityDef<Sprite, CharacterState>* def;
    Character()
    {
        def = new EntityDef<Sprite, CharacterState>();
        
        (*def)[CharacterState::Walking].AddFrame("assets\\character\\move\\frame-1.png");
        (*def)[CharacterState::Walking].AddFrame("assets\\character\\move\\frame-2.png");
        (*def)[CharacterState::Walking].AddFrame("assets\\character\\move\\frame-3.png");
        (*def)[CharacterState::Walking].AddFrame("assets\\character\\move\\frame-4.png");
        (*def)[CharacterState::Idle].AddFrame("assets\\character\\idle\\frame-1.png");
        (*def)[CharacterState::Idle].AddFrame("assets\\character\\idle\\frame-2.png");
        (*def)[CharacterState::Idle].AddFrame("assets\\character\\idle\\frame-3.png");
        (*def)[CharacterState::Idle].AddFrame("assets\\character\\idle\\frame-4.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-1.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-2.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-3.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-4.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-5.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-6.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-7.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-8.png");
        (*def)[CharacterState::Dash].AddFrame("assets\\character\\dash\\frame-9.png");
        (*def)[CharacterState::Attack].AddFrame("assets\\character\\attack\\frame-1.png");
        (*def)[CharacterState::Attack].AddFrame("assets\\character\\attack\\frame-2.png");
        (*def)[CharacterState::Attack].AddFrame("assets\\character\\attack\\frame-3.png");
        (*def)[CharacterState::Attack].AddFrame("assets\\character\\attack\\frame-4.png");
        
        stateMachine.SetDefinition(def);
        stateMachine.DefineState(CharacterState::Walking, 0.2f, Style::Repeat);
        stateMachine.DefineState(CharacterState::Idle, 0.2f, Style::Repeat);
        stateMachine.DefineState(CharacterState::Attack, 0.1f, Style::PlayOnce);
        stateMachine.DefineState(CharacterState::Dash, 0.04f, Style::PlayOnce);

        stateMachine.SetState(CharacterState::Idle);

        coins = 0;
    }
    inline void Movement(Window& window, float speed)
    {
        if(!stateMachine.IsCurrentState(CharacterState::Walking)) return;
        const float dt = window.GetDeltaTime();
        if(currPowerup == PowerupType::Speed) speed *= 2.0f;
        if(window.GetKey(GLFW_KEY_W) == Key::Held) pos.y -= speed * dt;
        if(window.GetKey(GLFW_KEY_S) == Key::Held) pos.y += speed * dt;
        if(window.GetKey(GLFW_KEY_A) == Key::Held) 
        {
            pos.x -= speed * dt;
            if(facesRight) facesRight = !facesRight;
        }
        if(window.GetKey(GLFW_KEY_D) == Key::Held) 
        {
            pos.x += speed * dt;
            if(!facesRight) facesRight = !facesRight;
        }
        if(pos.x < mapBound.pos.x) pos.x += speed * dt;
        if(pos.x > mapBound.pos.x + mapBound.size.x) pos.x -= speed * dt;
        if(pos.y < mapBound.pos.y) pos.y += speed * dt;
        if(pos.y > mapBound.pos.y + mapBound.size.y) pos.y -= speed * dt;
    }
    inline void Dash(Window& window)
    {
        if(!stateMachine.IsCurrentState(CharacterState::Dash)) return;
        float dx = 600.0f * window.GetDeltaTime() * (!facesRight ? -1 : 1);
        if((pos.x + dx) > mapBound.pos.x && (pos.x + dx) < mapBound.pos.x + mapBound.size.x) pos.x += dx;
    }
    inline void UpdateStates(Window& window)
    {
        if((stateMachine.IsCurrentState(CharacterState::Dash) || stateMachine.IsCurrentState(CharacterState::Attack))
            && !stateMachine.HasCurrentAnimationFinishedPlaying()) return;
        if(window.GetKey(GLFW_KEY_LEFT_SHIFT) == Key::Pressed) stateMachine.SetState(CharacterState::Dash);
        else if(window.GetMouseButton(GLFW_MOUSE_BUTTON_1) == Key::Pressed) stateMachine.SetState(CharacterState::Attack);
        else stateMachine.SetState((window.GetKey(GLFW_KEY_A) == Key::Held || window.GetKey(GLFW_KEY_W) == Key::Held || 
        window.GetKey(GLFW_KEY_S) == Key::Held || window.GetKey(GLFW_KEY_D) == Key::Held) ? CharacterState::Walking : CharacterState::Idle);
    }
    inline void Update(Window& window)
    {
        if(currPowerup == PowerupType::Health) health = maxHealth;
        UpdateStates(window);
        Movement(window, speed);
        Dash(window);
        stateMachine.Update(window.GetDeltaTime());
    }
    inline void Draw(Window& window)
    {
        stateMachine.Draw(window, pos, 3.5f, 0.0f, facesRight ? 0 : Horizontal);
        window.DrawText(32, 35, "HEALTH:" + std::to_string(health), 2.0f, Colors::White);
        window.DrawText(32, 63, "COINS:" + std::to_string(coins), 2.0f, Colors::White);
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
        stateMachine.SetState(CharacterState::Idle);
        facesRight = true;
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
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-10.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-9.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-8.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-7.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-6.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-5.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-4.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-3.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-2.png");
        enemyDef[EnemyState::Spawn].AddFrame("assets\\enemy\\dead\\frame-1.png");
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
    bool facesRight = true;
    float health = 100.0f;
    vec2 pos = 0.0f;
    EnemyType type;
    virtual void Update(Character& character, float delta) = 0;
    virtual void Draw(Window& window) = 0;
    virtual void GiveDamage(Character& character, float delta) = 0;
    virtual void SetSpawnData(const vec2& pos) = 0;
    inline void DrawSelf(Window& window)
    {
        stateMachine.Draw(window, pos, defMap.at(type)->size, 0.0f, facesRight ? 0 : Flip::Horizontal);
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
            if(character.stateMachine.IsCurrentState(CharacterState::Attack)) health -= 10.0f * delta;
            else if(character.stateMachine.IsCurrentState(CharacterState::Dash)) health = 0.0f;
        }
    }
};

struct Ghost : EnemyBase
{
    Ghost()
    {
        type = EnemyType::Ghost;
        stateMachine.DefineState(EnemyState::Spawn, 0.2f, Style::PlayOnce);
        stateMachine.DefineState(EnemyState::Idle, 0.2f, Style::Repeat);
        stateMachine.DefineState(EnemyState::Attack, 0.2f, Style::PlayOnce);
        stateMachine.DefineState(EnemyState::Move, 0.2f, Style::Repeat);
        stateMachine.DefineState(EnemyState::Dead, 0.2f, Style::PlayOnce);
        stateMachine.SetDefinition(&defMap.at(type)->enemyDef);
        stateMachine.SetState(EnemyState::Spawn);
    }
    inline void UpdateStates(Character& character)
    {
        if((stateMachine.IsCurrentState(EnemyState::Attack) || stateMachine.IsCurrentState(EnemyState::Dead) 
            || stateMachine.IsCurrentState(EnemyState::Spawn)) && !stateMachine.HasCurrentAnimationFinishedPlaying()) return;
        float dist = GetDistance(character);
        if(dist <= 100.0f) stateMachine.SetState(EnemyState::Attack);
        else stateMachine.SetState((!InBounds(pos, mapBound) || dist < 1000.0f) ? EnemyState::Move : EnemyState::Idle);
    }
    inline void Update(Character& character, float delta) override
    {
        UpdateStates(character);
        if(stateMachine.IsCurrentState(EnemyState::Move)) Movement(character, delta);
        UpdateSelf(character, delta);
        GiveDamage(character, delta);
    }
    inline void Movement(Character& character, float delta, float speed = 150.0f)
    {
        facesRight = character.pos.x >= pos.x;
        float angle = std::atan2(character.pos.y - pos.y, character.pos.x - pos.x);
        pos.x += std::cos(angle) * speed * delta;
        pos.y += std::sin(angle) * speed * delta;
    }
    inline void GiveDamage(Character& character, float delta) override
    {
        if(health <= 0.0f && character.currPowerup == PowerupType::Shield) return;
        if(GetDistance(character) < 100.0f &&  stateMachine.IsCurrentState(EnemyState::Attack)) character.health -= delta;
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
    float travelDist = 0.0f;
    bool remove = false;
    inline void Update(Character& character, float delta, float speed = 150.0f)
    {
        remove = travelDist > 300.0f;
        pos += vec_from_angle(std::atan2(character.pos.y - pos.y, character.pos.x - pos.x)) * speed * delta;
        travelDist += speed * delta;
    }
};

struct Ranged : EnemyBase
{
    Ranged()
    {
        type = EnemyType::Ranged;
        timeSinceLastAttack = 0.0f;
        stateMachine.DefineState(EnemyState::Idle, 0.2f, Style::Repeat);
        stateMachine.DefineState(EnemyState::Spawn, 0.2f, Style::PlayOnce);
        stateMachine.DefineState(EnemyState::Attack, 0.2f, Style::PlayOnce);
        stateMachine.DefineState(EnemyState::Dead, 0.2f, Style::PlayOnce);
        stateMachine.SetDefinition(&defMap.at(type)->enemyDef);
        stateMachine.SetState(EnemyState::Spawn);
        ball.remove = true;
    }
    inline void UpdateStates()
    {
        if((stateMachine.IsCurrentState(EnemyState::Attack) || stateMachine.IsCurrentState(EnemyState::Dead) ||
            stateMachine.IsCurrentState(EnemyState::Spawn)) && !stateMachine.HasCurrentAnimationFinishedPlaying()) return;
        stateMachine.SetState(timeSinceLastAttack < 5.0f ? EnemyState::Idle : EnemyState::Attack);
    }
    inline void Update(Character& character, float delta) override
    {
        timeSinceLastAttack += delta;
        UpdateStates();
        if(stateMachine.IsCurrentState(EnemyState::Attack)) Attack();
        UpdateSelf(character, delta);
        UpdateEnergyBall(character, delta);
        GiveDamage(character, delta);
    }
    inline void Attack()
    {
        ball.pos = pos;
        ball.travelDist = 0.0f;
        ball.remove = false;
        stateMachine.SetState(EnemyState::Idle);
        timeSinceLastAttack = 0.0f;
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
    float timeSinceLastAttack;
};

struct EnemyWrapper
{
    EnemyBase* enemyBasePtr;
    bool remove = false;
    void Update(Character& character, float delta)
    {
        if(enemyBasePtr->stateMachine.IsCurrentState(EnemyState::Dead)) remove = enemyBasePtr->stateMachine.HasCurrentAnimationFinishedPlaying();
        int coinInc = remove ? (character.currPowerup == PowerupType::Money ? 3 : 1) : 0;
        character.coins += character.coinMultiplier * coinInc;
        enemyBasePtr->Update(character, delta);
    }
    void Draw(Window& window)
    {
        enemyBasePtr->Draw(window);
    }
};

inline void SpawnEnemy(std::vector<EnemyWrapper>& enemies, const EnemyType& enemyType, Window& window)
{
    switch(enemyType)
    {
        case EnemyType::Ghost: enemies.push_back({new Ghost()}); break;
        case EnemyType::Ranged: enemies.push_back({new Ranged()}); break;
    }
    enemies.back().enemyBasePtr->SetSpawnData(RandomPoint(mapBound));
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
    SpawnSystemState spawnSysState;
    int currentWave, enemiesSpawned;
    inline void Reset()
    {
        timeSinceSpawn = 0.0f;
        currentWave = 1;
        enemiesSpawned = 0;
        spawnSysState = SpawnSystemState::Cooldown;
        for(auto& e : enemies)
        {
            delete e.enemyBasePtr;
            e.enemyBasePtr = nullptr;
        }
        enemies.clear();
    }
    inline void Update(Window& window, Character& character)
    {
        timeSinceSpawn += window.GetDeltaTime();

        switch(spawnSysState)
        {
            case SpawnSystemState::Cooldown:
            {
                if(timeSinceSpawn > 2.0f) 
                {
                    currentWave++;
                    timeSinceSpawn = 0.0f;
                    spawnSysState = SpawnSystemState::Spawning;
                }
            }
            break;
            case SpawnSystemState::Spawning:
            {
                if(timeSinceSpawn > 5.0f)
                {
                    if(enemiesSpawned < currentWave) 
                    {
                        SpawnEnemy(enemies, (EnemyType)random(0, 2), window);
                        timeSinceSpawn = 0.0f;
                        enemiesSpawned++;
                    }
                }
                if(enemies.empty() && enemiesSpawned == currentWave)
                {
                    timeSinceSpawn = 0.0f;
                    enemiesSpawned = 0;
                    spawnSysState = SpawnSystemState::Cooldown;
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
            (spawnSysState == SpawnSystemState::Cooldown) ? Colors::White : Colors::DarkRed, {0.5f, 0.0f});
    
        for(auto& enemy : enemies) enemy.Draw(window);
    }
};

struct Chest
{
    std::unordered_map<PowerupType, Sprite> powerups;
    vec2 pos = {900.0f, 170.0f};
    Animator<Sprite> animator;
    float elapsedTime;
    enum class ChestState
    {
        Opening,
        Closed,
        Open
    } chestState= ChestState::Closed;
    inline Chest()
    {
        animator.AddFrame("assets\\chest\\frames\\frame-0.png");
        animator.AddFrame("assets\\chest\\frames\\frame-1.png");
        animator.AddFrame("assets\\chest\\frames\\frame-2.png");
        powerups[PowerupType::Health] = Sprite("assets\\chest\\powerups\\health.png");
        powerups[PowerupType::Speed] = Sprite("assets\\chest\\powerups\\fast-run.png");
        powerups[PowerupType::Shield] = Sprite("assets\\chest\\powerups\\shield.png");
        powerups[PowerupType::Money] = Sprite("assets\\chest\\powerups\\money-icon.png");
        animator.SetReverse(true);
        animator.SetStyle(Style::PlayOnce);
        animator.SetDuration(0.2f);
        elapsedTime = 5.0f;
    }
    inline void Reset()
    {
        elapsedTime = 5.0f;
        animator.Reset();
        animator.SetReverse(true);
        chestState= ChestState::Closed;
    }
    inline void DrawPowerup(Character& character, Window& window)
    {
        if(character.currPowerup == PowerupType::None) return;
        float y = pos.y - std::clamp(elapsedTime, 0.0f, 4.0f) * 10.0f;
        window.DrawSprite(pos.x, y, powerups[character.currPowerup], 3.0f);
    }
    inline void Draw(Character& character, Window& window)
    {
        if(chestState== ChestState::Closed && Distance(character.pos, pos) < 100.0f && elapsedTime > 5.0f)
            window.DrawText(pos.x - 100.0f, pos.y - 60.0f, "Press E to open.", 1.5f, {255, 255, 255, 255});
        
        window.DrawSprite(pos.x, pos.y, animator.GetImage(), 6.0f);
        
        DrawPowerup(character, window);
    }
    inline void Update(Character& character, Window& window)
    {
        const float dt = window.GetDeltaTime();
        switch(chestState)
        {
            case ChestState::Opening: 
            {
                animator.Update(dt);
                if(animator.HasFinishedPlaying())
                {
                    character.currPowerup = (PowerupType)random(0, 4);
                    chestState= ChestState::Open;
                } 
            }
            break;
            case ChestState::Closed: 
            {
                animator.Update(dt);
                elapsedTime += dt;
                if(elapsedTime > 5.0f && window.GetKey(GLFW_KEY_E) == Key::Pressed && Distance(character.pos, pos) < 100.0f) 
                {
                    elapsedTime = 0.0f;
                    animator.Reverse();
                    chestState= ChestState::Opening;
                }
            }
            break;
            case ChestState::Open:
            {
                elapsedTime += dt;
                if(elapsedTime > 10.0f) 
                {
                    elapsedTime = 0.0f;
                    animator.Reverse();
                    character.currPowerup = PowerupType::None;
                    chestState= ChestState::Closed;
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
        character.speed = items["speed"].GetPower();
        character.maxHealth = items["health"].GetPower();
        character.coinMultiplier = items["money"].GetPower();
    }
};

#endif