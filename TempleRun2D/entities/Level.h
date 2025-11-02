#pragma once

#include "GameObject.h"
#include "Resources.h"

// ----- LEVEL TYPES -----
enum class LevelType
{
    floor, wall
};

// ----- LEVEL CLASS -----
class Level : public GameObject
{
private:
    LevelType levelType;    // Type of the level tile
    
public:
    Level(const GameObject& obj) : levelType(LevelType::floor)
    {
        setType(obj.getType());
        setTexture(obj.getTexture());
        setCollider(obj.getCollider());
        setImageSize(obj.getImageSize());
        setImageOffset(obj.getImageOffset());
    }

    Level(const Resources& res) : levelType(LevelType::floor)
    {
        setType(ObjectType::level);
        setPosition(glm::vec2(0));
        setVelocity(glm::vec2(0));
        // setAnimations(res.playerAnims);
        // setCurrentAnimation(res.ANIM_PLAYER_IDLE);
        setTexture(res.texFloor);
        setImageSize({ 32, 32 });
        setImageOffset({ 0, 0 });
        // setGrounded(true);
        addCollider({
            .x = 0,
            .y = 0,
            .w = 32,
            .h = 32
            });
    }

    // Getters
    LevelType getLevelType() const
    {
        return levelType;
    }

    // Setters
    void setLevelType(LevelType t)
    {
        levelType = t;
    }
};