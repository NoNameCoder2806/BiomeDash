#pragma once

#include "GameObject.h"
#include "Resources.h"

// ----- MONSTER STATES -----
enum class MonsterState
{
    idle, chasing, destroying, killing, notVisible, accelerating
};

// ----- MONSTER CLASS -----
class Monster : public GameObject
{
private:
    MonsterState state;
    Timer chaseTimer;
    float speed;
    bool active;
    float speedMultiplier = 1.0f;

public:
    // Constructor
    Monster(const Resources& res) : state(MonsterState::idle), speed(150.0f), chaseTimer(0), active(false)
    {
        setType(ObjectType::monster);
        setPosition(glm::vec2(0));
        setVelocity(glm::vec2(0));
        setAnimations(res.monsterAnims);
        setCurrentAnimation(res.ANIM_MONSTER_IDLE);
        setTexture(res.monsterIdle);
        setImageSize({ 64, 64 });
        setImageOffset({ 0, 0 });
        setGrounded(false);
        addCollider({
            .x = 8,
            .y = -32,
            .w = 100,
            .h = 64
        });
    }

    // Getters
    MonsterState getState() const
    {
        return state;
    }

    float getSpeed() const
    {
        return speed;
    }

    bool isActive() const
    {
        return active;
    }

    const Timer& getChaseTimer() const
    {
        return chaseTimer;
    }
    float getSpeedMultiplier() const
    {
        return speedMultiplier;
    }

    // Setters
    void setState(MonsterState st)
    {
        state = st;
    }

    void setSpeed(float s)
    {
        speed = s;
    }

    void setActive(bool a)
    {
        active = a;
    }

    void setChaseTimer(const Timer& t)
    {
        chaseTimer = t;
    }

    void setSpeedMultiplier(float sm)
    {
        speedMultiplier = sm;
    }

    // Helper function
    void update(float deltaTime)
    {
        // Example update logic (you can customize this)
        if (state == MonsterState::chasing)
        {
            // Update chase timer
            chaseTimer.step(deltaTime);

            // TODO: movement logic here
        }

        // TODO: other states update
    }

    void updateMovement(float deltaTime)
    {
        glm::vec2 velocity = getVelocity();

        // Endless horizontal movement
        float moveAmount = 0.0f;

        switch (getState())
        {
            case MonsterState::idle:
            case MonsterState::killing:
            {
                moveAmount = 0.0f;
                break;
            }

            case MonsterState::chasing:
            case MonsterState::destroying:
            case MonsterState::notVisible:
            {
                moveAmount = getSpeed();
                break;
            }

            default:
            {
                moveAmount = 0.0f;
            }
        }

        velocity.x = moveAmount * speedMultiplier;

        setVelocity(velocity);
        setPosition(getPosition() + velocity * deltaTime);
    }
};