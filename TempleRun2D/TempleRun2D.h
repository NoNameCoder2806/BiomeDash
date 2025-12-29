// TempleRun2D.h : Include file for standard system include files,
// or project specific include files.

// TODO: Reference additional headers your program requires here.

#pragma once

// Standard libraries
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <format>
#include <memory>

// SDL libraries
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

// Project headers
#include "GameState.h"
#include "SDLState.h"
#include "Resources.h"
#include "Animation.h"
#include "Timer.h"

// Entities
#include "entities/GameObject.h"
#include "entities/Player.h"
#include "entities/Monster.h"
#include "entities/Level.h"
#include "entities/Obstacle.h"
#include "entities/Boost.h"

// World
#include "world/Biome.h"

// Forward declarations / externals
extern std::unordered_map<std::string, Biome> biomeTexturesMap;

// Function prototypes

// Screens
void runHomeScreen(SDLState& sdl, ScreenState& currentScreen, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);
void runPlayingFrame(SDLState& sdl, GameState& game, Resources& res,
    std::vector<float>& scrollPositions, uint64_t& prevTime,
    float& fps, int& frames, uint64_t& fpsLastTime, bool& running);

// Game logic
void resetForNewBiome(SDLState& state, GameState& gs, Resources& res);
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime);
void cleanupOffscreenObjects(GameState& gs);
void updateObject(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
void checkCollision(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB, float deltaTime);
void collisionResponse(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB,
    SDL_FRect rectA, SDL_FRect rectB, SDL_FRect rectC, float deltaTime);
void manageTiles(const SDLState& state, GameState& gs, Resources& res, bool isUpdate);
void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown);
bool checkGrounded(const GameObject& player, const std::vector<std::unique_ptr<GameObject>>& levelTiles);

// Utilities
void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime);
