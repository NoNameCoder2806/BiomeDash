#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "SDLState.h"
#include "Animation.h"

struct Resources
{
    // ----- I/ PLAYER ANIMATIONS -----
    const int ANIM_PLAYER_IDLE = 0;           // Idle
    const int ANIM_PLAYER_RUN = 1;            // Running
    const int ANIM_PLAYER_JUMP = 2;           // Jumping
    const int ANIM_PLAYER_SLIDE = 3;          // Sliding 
    const int ANIM_PLAYER_TRIPPED = 4;        // Tripped
    const int ANIM_PLAYER_KNOCKED = 5;        // Knocked
    const int ANIM_PLAYER_BURNT = 6;          // Burnt
    const int ANIM_PLAYER_BLEED = 7;          // Bleed
    const int ANIM_PLAYER_FALLING = 8;        // Falling 
    const int ANIM_PLAYER_SPEEDING = 9;       // Speeding
    const int ANIM_PLAYER_CAUGHT = 10;        // Caught

    std::vector<Animation> playerAnims;       // Stores all player animations

    SDL_Texture* playerIdle = nullptr;
    SDL_Texture* playerRun = nullptr;
    SDL_Texture* playerSlide = nullptr;
    SDL_Texture* playerJump = nullptr;
    SDL_Texture* playerKnocked = nullptr;
    SDL_Texture* playerTripped = nullptr;
    SDL_Texture* playerBurnt = nullptr;
    SDL_Texture* playerBleed = nullptr;
    SDL_Texture* playerFalling = nullptr;
    SDL_Texture* playerSpeeding = nullptr;
    SDL_Texture* playerCaught = nullptr;

    // ----- II/ MONSTER ANIMATIONS ----- 
    const int ANIM_MONSTER_IDLE = 0;          // Idle
    const int ANIM_MONSTER_CHASE = 1;         // Chasing
    const int ANIM_MONSTER_DESTROY = 2;       // Destroy
    const int ANIM_MONSTER_KILL = 3;          // Killing
    const int ANIM_MONSTER_NOTVISIBLE = 4;    // Not Visible

    std::vector<Animation> monsterAnims;      // Stores all enemy animations

    SDL_Texture* monsterIdle = nullptr;
    SDL_Texture* monsterChase = nullptr;
    SDL_Texture* monsterDestroy = nullptr;
    SDL_Texture* monsterKill = nullptr;
    SDL_Texture* monsterNotVisible = nullptr;

    // ----- III/ STARTING PORTAL ANIMATIONS -----
    std::vector<Animation> startPortalAnim;
    SDL_Texture* startPortal = nullptr;
    
    // ----- IV / ENDING PORTAL ANIMATIONS -----
    std::vector<Animation> endPortalAnim;
    SDL_Texture* endPortal = nullptr;
    
    // ----- V/ BACKGROUNDS -----
    SDL_Texture* background = nullptr;
    std::vector<SDL_Texture*> parallaxBackgrounds;

    std::vector<SDL_Texture*> textures;  // Keeps track of all loaded textures (for cleanup)

    // A map to keep track of all the textures
    std::unordered_map<int, SDL_Texture*> tileTextures;

    SDL_Texture* texFloor = nullptr;

    // Helper function to load a texture from file
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filepath)
    {
        std::cout << "----- Loading from path " << filepath << " -----" << std::endl;
        SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());

        if (tex == nullptr)
        {
            std::cout << "Cannot open: " << filepath << std::endl;
        }

        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST); // Nearest neighbor scaling
        textures.push_back(tex);                              // Store for cleanup
        
        return tex;
    }

    // Create one-off textures for tiles based on their id
    SDL_Texture* getTileTexture(SDL_Renderer* renderer, const std::string& biomeName, int id)
    {
        // If the textures haven't existed in the map, we add it to the map
        if (tileTextures.count(id) == 0)
        {
            std::string path = "data/textures/biomes/" + biomeName + "/" + std::to_string(id) + ".png";
            SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());

            // Debug print
            std::cout << "Loading tile ID " << id << " from path: " << path << std::endl;
            
            if (tex == nullptr)
            {
                std::cout << "Cannot open: " << path << std::endl;
                return nullptr;
            }

            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            tileTextures[id] = tex;
        }

        // Otherwise, we return it if it is in the map
        return tileTextures[id];
    }

    // Loads all textures and animations into memory
    void load(SDLState& state, std::string playerName, std::string monsterName, std::string biomeName, std::string parallaxBackgrounds)
    {
        // ----- I/ Player's animations ----- 
        playerAnims.resize(11);
        playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 1.0f, true);
        playerAnims[ANIM_PLAYER_RUN] = Animation(8, 1.0f, true);
        playerAnims[ANIM_PLAYER_JUMP] = Animation(8, 1.0f, true);
        playerAnims[ANIM_PLAYER_SLIDE] = Animation(8, 2.0f, true);
        playerAnims[ANIM_PLAYER_TRIPPED] = Animation(8, 1.0f, true);
        playerAnims[ANIM_PLAYER_KNOCKED] = Animation(8, 1.0f, false);
        playerAnims[ANIM_PLAYER_BURNT] = Animation(8, 1.0f, false);
        playerAnims[ANIM_PLAYER_BLEED] = Animation(8, 1.0f, false);
        playerAnims[ANIM_PLAYER_FALLING] = Animation(8, 1.0f, false);
        playerAnims[ANIM_PLAYER_SPEEDING] = Animation(8, 1.0f, true);
        playerAnims[ANIM_PLAYER_CAUGHT] = Animation(8, 1.0f, true);

        playerIdle = loadTexture(state.renderer, "data/textures/characters/" + playerName +  "/idle.png");
        playerRun = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/run.png");
        playerJump = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/jump.png");
        playerSlide = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/slide.png");
        playerTripped = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/tripped.png");
        playerKnocked = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/knocked.png");
        playerBurnt = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/burnt.png");
        playerBleed = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/bleed.png");
        playerFalling = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/falling.png");
        playerSpeeding = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/speeding.png");
        playerCaught = loadTexture(state.renderer, "data/textures/characters/" + playerName + "/caught.png");

        // ----- II/ Monster's animations ----- 
        monsterAnims.resize(5);
        monsterAnims[ANIM_MONSTER_IDLE] = Animation(8, 1.0f, true);   // 8 frames over 1 second
        monsterAnims[ANIM_MONSTER_CHASE] = Animation(8, 1.0f, true);
        monsterAnims[ANIM_MONSTER_DESTROY] = Animation(18, 2.0f, true);
        
        monsterIdle = loadTexture(state.renderer, "data/textures/monsters/" + monsterName + "/default_monster_idle.png");
        monsterChase = loadTexture(state.renderer, "data/textures/monsters/" + monsterName + "/default_monster_chasing.png");
        monsterDestroy = loadTexture(state.renderer, "");
        monsterKill = loadTexture(state.renderer, "");
        monsterNotVisible = loadTexture(state.renderer, "");

        // ----- III/ Starting Portal's animations -----
        // startPortalAnim.resize(1);
        // startPortalAnim[0] = Animation(8, 1.0f, false);
        // startPortal = loadTexture(state.renderer, "data/textures/portals/startPortal");

        // ----- IV/ Ending Portal's animations -----
        endPortalAnim.resize(1);
        endPortalAnim[0] = Animation(7, 1.0f, true);
        endPortal = loadTexture(state.renderer, "data/textures/portals/end_portal.png");

        // Load background 
        background = loadTexture(state.renderer, "data/textures/background/" + biomeName + "/" + biomeName + ".png");

        // Load parallax backgrounds
        for (int i = 1; i <= std::stoi(parallaxBackgrounds); i++)
        {
            std::string path = "data/textures/background/" + biomeName + "/"
                + biomeName + "_" + std::to_string(i) + ".png";

            SDL_Texture* tex = loadTexture(state.renderer, path);
            if (tex)
            {
                this->parallaxBackgrounds.push_back(tex);
                std::cout << "Loaded successfully: " << path << std::endl;
            }
            else
            {
                std::cout << "Failed to load parallax background: " << path << std::endl;
            }
        }
    }

    // Cleans up all textures from memory when the game closes
    void unload()
    {
        for (SDL_Texture* tex : textures)
        {
            SDL_DestroyTexture(tex);
        }
        textures.clear();

        // Destroy cached tile textures
        for (auto& [id, tex] : tileTextures) {
            SDL_DestroyTexture(tex);
        }
        tileTextures.clear();
    }
};