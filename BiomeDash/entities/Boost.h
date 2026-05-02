#pragma once

#include "GameObject.h"
#include "Resources.h"

// ----- BOOST TYPES -----
enum class BoostType
{
    shield, boost, score
};

// ----- BOOST CLASS -----
class Boost : public GameObject
{
private:
    BoostType boostType;    // Type of the obstacle
    float amount;
    int time;

public:
    Boost(const Resources& res) : boostType(BoostType::shield), amount(1), time(-1)
    {
        setType(ObjectType::boost);
        setPosition(glm::vec2(0));
        setVelocity(glm::vec2(0));
        // setAnimations(res.playerAnims);
        // setCurrentAnimation(res.ANIM_PLAYER_IDLE);
        // setTexture(res.playerIdle);
        // setGrounded(true);
        /*setCollider({
            .x = 10,
            .y = 10,
            .w = 12,
            .h = 12
            });*/
    }

    // Getters
    BoostType getBoostType() const
    {
        return boostType;
    }

    float getAmount() const
    {
        return amount;
    }

    int getTime() const
    {
        return time;
    }

    // Setters
    void setBoostType(BoostType t)
    {
        boostType = t;
    }

    void setAmount(float a)
    {
        amount = a;
    }

    void setTime(int t)
    {
        time = t;
    }
};