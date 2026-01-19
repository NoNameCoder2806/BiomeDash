#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "Resources.h"

struct PlayerTextures
{
    std::string name;

    std::vector<Animation> anims;

    SDL_Texture* idle = nullptr;
    SDL_Texture* run = nullptr;
    SDL_Texture* jump = nullptr;
    SDL_Texture* slide = nullptr;
    SDL_Texture* tripped = nullptr;
    SDL_Texture* knocked = nullptr;
    SDL_Texture* burnt = nullptr;
    SDL_Texture* bleed = nullptr;
    SDL_Texture* falling = nullptr;
    SDL_Texture* speeding = nullptr;
    SDL_Texture* caught = nullptr;

    // A helper function to load all the textures
    void loadTextures(SDL_Renderer* renderer, const std::string& characterName, Resources& res)
    {
        name = characterName;

        anims.resize(11);
        anims[res.ANIM_PLAYER_IDLE] = Animation(8, 1.0f, true);
        anims[res.ANIM_PLAYER_RUN] = Animation(8, 1.0f, true);
        anims[res.ANIM_PLAYER_JUMP] = Animation(8, 1.0f, true);
        anims[res.ANIM_PLAYER_SLIDE] = Animation(8, 2.0f, true);
        anims[res.ANIM_PLAYER_TRIPPED] = Animation(8, 1.0f, true);
        anims[res.ANIM_PLAYER_KNOCKED] = Animation(8, 1.0f, false);
        anims[res.ANIM_PLAYER_BURNT] = Animation(8, 1.0f, false);
        anims[res.ANIM_PLAYER_BLEED] = Animation(8, 1.0f, false);
        anims[res.ANIM_PLAYER_FALLING] = Animation(8, 1.0f, false);
        anims[res.ANIM_PLAYER_CAUGHT] = Animation(8, 1.0f, true);
        anims[res.ANIM_PLAYER_SPEEDING] = Animation(8, 1.0f, true);

        idle = res.loadTexture(renderer, "data/textures/characters/" + name + "/idle.png");
        run = res.loadTexture(renderer, "data/textures/characters/" + name + "/run.png");
        jump = res.loadTexture(renderer, "data/textures/characters/" + name + "/jump.png");
        slide = res.loadTexture(renderer, "data/textures/characters/" + name + "/slide.png");
        tripped = res.loadTexture(renderer, "data/textures/characters/" + name + "/tripped.png");
        knocked = res.loadTexture(renderer, "data/textures/characters/" + name + "/knocked.png");
        burnt = res.loadTexture(renderer, "data/textures/characters/" + name + "/burnt.png");
        bleed = res.loadTexture(renderer, "data/textures/characters/" + name + "/bleed.png");
        falling = res.loadTexture(renderer, "data/textures/characters/" + name + "/falling.png");
        caught = res.loadTexture(renderer, "data/textures/characters/" + name + "/caught.png");
        speeding = res.loadTexture(renderer, "data/textures/characters/" + name + "/speeding.png");
    }
};