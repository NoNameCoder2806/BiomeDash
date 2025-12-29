#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include "Animation.h"
#include "Timer.h"

#include <vector>

// ----- OBJECT TYPES -----
enum class ObjectType
{
    player, monster, level, startportal, endportal, commander, obstacle, boost, text
};

// ----- GAMEOBJECT CLASS -----
class GameObject
{
private:
    // Object type
    ObjectType type;

    // Position and velocity
    glm::vec2 position;
    glm::vec2 velocity;

    // Animations
    std::vector<Animation> animations;
    int currentAnimation;

    // Texture and texture size
    SDL_Texture* texture;
    glm::vec2 imageSize;
    glm::vec2 imageOffset;

    // Collision
    bool grounded;
    std::vector<SDL_FRect> collider;

public:
    // Constructor
    GameObject() : collider{ 0 }
    {
        type = ObjectType::level;
        position = velocity = glm::vec2(0);
        animations = {};
        currentAnimation = -1;
        texture = nullptr;
        imageSize = { 32, 32 };
        imageOffset = { 0, 0 };
        grounded = false;
    }

    // Getters
    ObjectType getType() const 
    { 
        return type;
    }

    glm::vec2 getPosition() const 
    { 
        return position; 
    }

    glm::vec2 getVelocity() const 
    { 
        return velocity; 
    }

    std::vector<Animation>& getAnimations() 
    { 
        return animations; 
    }

    int getCurrentAnimation() const 
    { 
        return currentAnimation; 
    }
    
    SDL_Texture* getTexture() const 
    { 
        return texture; 
    }

    glm::vec2 getImageSize() const
    {
        return imageSize;
    }

    glm::vec2 getImageOffset() const
    {
        return imageOffset;
    }

    bool isGrounded() const 
    { 
        return grounded; 
    }

    std::vector<SDL_FRect> getCollider() const 
    { 
        return collider; 
    }

    // Setters
    void setType(ObjectType t) 
    { 
        type = t; 
    }

    void setPosition(const glm::vec2& p) 
    { 
        position = p; 
    }

    void setVelocity(const glm::vec2& v) 
    { 
        velocity = v; 
    }

    void setAnimations(const std::vector<Animation>& anims) 
    { 
        animations = anims; 
    }

    void setCurrentAnimation(int index) 
    { 
        currentAnimation = index; 
    }

    void setTexture(SDL_Texture* tex) 
    {
        texture = tex; 
    }

    void setImageSize(glm::vec2 size)
    {
        imageSize = size;
    }

    void setImageOffset(glm::vec2 offset)
    {
        imageOffset = offset;
    }

    void setGrounded(bool g) 
    { 
        grounded = g; 
    }

    void setCollider(const std::vector<SDL_FRect>& rect) 
    { 
        collider = rect; 
    }

    // Helper functions
    void update()
    {
        // Your update logic here
    }

    void addCollider(SDL_FRect r)
    {
        collider.push_back(r);
    }

    void clearCollider()
    {
        collider.clear();
    }

    virtual void updateMovement(float deltaTime) {} // default: do nothing } virtual ~GameObject() {}
};