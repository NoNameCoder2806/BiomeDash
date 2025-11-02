#pragma once

#include "GameObject.h"
#include "Resources.h"

// ----- OBSTACLE TYPES -----
enum class ObstacleType
{
    tripped, burnt, spike, wall
};

// ----- OBSTACLE CLASS -----
class Obstacle : public GameObject
{
private:
    ObstacleType type;    // Type of the obstacle
    bool lethal ;         // Does the obstacle cause death?
    
public:
    Obstacle() : type(ObstacleType::wall), lethal(true)
    {
        setType(ObjectType::obstacle);
        setPosition(glm::vec2(0));
        setVelocity(glm::vec2(0));
        /*setCollider({
            .x = 0,
            .y = 0,
            .w = 32,
            .h = 32
        });*/
    }

    Obstacle(const Obstacle& obs)
    {
        setObstacleType(obs.getObstacleType());
        setType(obs.getType());
        setTexture(obs.getTexture());
        setCollider(obs.getCollider());
        setImageSize(obs.getImageSize());
        setImageOffset(obs.getImageOffset());
    }

    Obstacle(const Resources& res) : type(ObstacleType::wall), lethal(true)
    {
        setType(ObjectType::obstacle);
        setPosition(glm::vec2(0));
        setVelocity(glm::vec2(0));
        // setAnimations(res.playerAnims);
        // setCurrentAnimation(res.ANIM_PLAYER_IDLE);
        /*addCollider({
            .x = 0,
            .y = 0,
            .w = 32,
            .h = 32
        });*/
    }

    // Getters
    ObstacleType getObstacleType() const
    {
        return type;
    }

    bool isLethal() const
    {
        return lethal;
    }

    // Setters
    void setObstacleType(ObstacleType t)
    {
        type = t;
    }

    void setLethal(bool l)
    {
        lethal = l;
    }
};