#pragma once

#include "GameObject.h"
#include "Resources.h"

//static int countJump = 0;

// Constants
const float GRAVITY = 700.0f;
const float JUMP_FORCE = -350.0f;
const float SLIDE_PULL = 350.0f;

// ----- PLAYER STATES -----
enum class PlayerState
{
    idle, running, jumping, sliding, tripped, knocked, burnt, falling, bleed, speeding, caught, won
};

// ----- PLAYER CLASS ----- 
class Player : public GameObject
{
private:
    PlayerState state;

    // Character name
    std::string characterName;

    // Jumping
    bool jumpRequested = false;
    bool slideRequested = false;

    float slideDuration = 2.0f;
    float trippedDuration = 1.0f;

    float speed = 150.0f;
    float boostMeter = 0.0f; // Progress from 0.0 to 1.0
    bool active = false;      // True if boost is ready
    bool alive = true;

    float speedMultiplier = 1.0f;

    float jumpBufferTime = 0.0f;         // counts down if jump was pressed
    const float jumpBufferDuration = 1.0f; // 150 ms buffer

public:
    // Constructor
    Player(const Resources& res) : characterName("hooded_hero"), state(PlayerState::idle), speed(150.0f), slideDuration(2.0f), trippedDuration(1.0f),
                                boostMeter(0.0f), active(false), jumpRequested(false), slideRequested(false)
    {
        setType(ObjectType::player);
        setPosition(glm::vec2(0));
        setVelocity(glm::vec2(0));
        setAnimations(res.playerAnims);
        setCurrentAnimation(res.ANIM_PLAYER_IDLE);
        setTexture(res.playerIdle);
        setImageSize({ 32, 32 });
        setImageOffset({ 0, 0 });
        setGrounded(true);
    }

    // Getters
    PlayerState getState() const
    {
        return state;
    }

    std::string getName() const
    {
        return characterName;
    }

    float getSpeed() const
    {
        return speed;
    }

    float getBoostMeter() const
    {
        return boostMeter;
    }

    bool isActive() const
    {
        return active;
    }

    bool isJumpRequested() const
    {
        return jumpRequested;
    }

    bool isSlideRequested() const
    {
        return slideRequested;
    }

    bool isAlive() const
    {
        return alive;
    }

    float getJumpBufferTime() const
    {
        return jumpBufferTime;
    }

    float getJumpBufferDuration() const
    {
        return jumpBufferDuration;
    }

    float getSpeedMultiplier() const
    {
        return speedMultiplier;
    }


    // Setters
    void setState(PlayerState s)
    {
        state = s;
    }

    void setName(std::string n)
    {
        characterName = n;
    }

    void setSpeed(float s)
    {
        speed = s;
    }

    void setBoostMeter(float bm)
    {
        boostMeter = bm;
    }

    void setActive(bool a)
    {
        active = a;
    }

    void setJumpRequested(bool jr)
    {
        jumpRequested = jr;
    }

    void setSlideRequested(bool sr)
    {
        slideRequested = sr;
    }

    void setAlive(bool a)
    {
        alive = a;
    }

    void setJumpBufferTime(float t)
    {
        jumpBufferTime = t;
    }

    void setSpeedMultiplier(float sm)
    {
        speedMultiplier = sm;
    }

    // Helper function
    int score()
    {
        return getPosition().x - 320;
    }

    void updateMovement(float deltaTime)
    {
        bool jumped = false;

        // If the player is dead we don't update the movement
        if (!isAlive())
        {
            // Set the velocity to 0
            glm::vec2 velocity = getVelocity();
            velocity.x = 0;
            setVelocity(velocity);

            return;
        }

        glm::vec2 velocity = getVelocity();

        // If the object isn't grounded we apply gravity
        if (!isGrounded())
        {
            velocity.y += GRAVITY * deltaTime; // Gravity
        }

        // Jump buffer countdown
        if (jumpBufferTime > 0.0f)
        {
            jumpBufferTime -= deltaTime;
        }

        // Sliding logic
        if (slideRequested && isAlive() && getState() != PlayerState::tripped)
        {
            if (!isGrounded())
            {
                velocity.y = SLIDE_PULL;

                setState(PlayerState::running);
                slideRequested = false;
            }
            else
            {
                setState(PlayerState::sliding);
                slideDuration = 2.0f;
                slideRequested = false;
            }
        }

        // Calculate the sliding duration
        if (getState() == PlayerState::sliding)
        {
            slideDuration -= deltaTime;

            if (slideDuration <= 0)
            {
                slideDuration = 2.0f;              // Reset the sliding duration for the next slide 
                setState(PlayerState::running);
                slideRequested = false;
            }

            if (jumpRequested)
            {
                slideDuration = 2.0f;
                setState(PlayerState::jumping);
                slideRequested = false;

                // Reset the animation frame to the first frame
                getAnimations().at(getCurrentAnimation()).getTimer().reset();
            }
        }

        // Jumping logic with buffer
        if (getJumpBufferTime() > 0.0f && isGrounded() &&
            (getState() == PlayerState::running || getState() == PlayerState::sliding))
        {
            //std::cout << "Signal Received!!" << std::endl;

            setState(PlayerState::jumping);

            //std::cout << "State set!!!" << std::endl;

            velocity.y = JUMP_FORCE;  // Jump force

            //std::cout << "Velocity added!" << std::endl;

            jumpBufferTime = 0.0f; // clear buffer after successful jump

            //std::cout << "Buffered Jumped!" << std::endl;

            jumped = true;
        }

        // Tripped duration
        if (getState() == PlayerState::tripped)
        {
            trippedDuration -= deltaTime;

            if (trippedDuration <= 0)
            {
                trippedDuration = 1.0f;
                setState(PlayerState::running);
            }
        }

        // Falling state
        if (getPosition().y > 500)
        {
            setState(PlayerState::falling);
        }

        // --- Horizontal movement ---
        float moveAmount = 0.0f;
        switch (getState())
        {
            case PlayerState::idle:
            case PlayerState::knocked:
            case PlayerState::falling:
            case PlayerState::burnt:
            case PlayerState::bleed:
            case PlayerState::caught:
            case PlayerState::won:
            {
                moveAmount = 0.0f;
                break;
            }

            case PlayerState::running:
            case PlayerState::jumping:
            case PlayerState::sliding:
            {
                moveAmount = getSpeed();
                break;
            }

            case PlayerState::tripped:
            {
                moveAmount = getSpeed() / 2.0f;
                break;
            }

            case PlayerState::speeding:
            {
                moveAmount = getSpeed() + 50.0f;
                break;
            }

            default:
            {
                moveAmount = 0.0f;
            }
        }
        velocity.x = moveAmount * speedMultiplier;

        // --- Apply velocity ---
        setVelocity(velocity);
        setPosition(getPosition() + velocity * deltaTime);

        if (jumped)
        {
            setGrounded(false);
        }

        // --- Debug ---
        /*if (velocity.y != 0 && getPosition().y >= 300 && getPosition().y <= 400)
        {
            std::cout << "Final velocity.y = " << velocity.y
                << " | Final pos.y = " << getPosition().y
                << " | Grounded? " << isGrounded()
                << " | State = " << (int)getState()
                << std::endl;
        }*/
    }
};
