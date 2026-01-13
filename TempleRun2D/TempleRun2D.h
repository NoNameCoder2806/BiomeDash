// TempleRun2D.h : Include file for standard system include files,
// or project specific include files.

// TODO: Reference additional headers your program requires here.

// Libraries
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

// Header files
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
#include "entities/Transition.h"
#include "entities/UIButton.h"

// World
#include "world/Biome.h"

// Libraries
#include <vector>
#include <array>
#include <string>
#include <format>
#include <unordered_map>

// Constants
// Screen size
const glm::vec2 NORMAL_SCREEN = { 544.0f, 306.0f };
const glm::vec2 ZOOMED_SCREEN = { 160.0f, 90.0f };

// Game tiles / characters index
const int PLAYER_INDEX = 1;
const int MONSTER_INDEX = 2;
const int SECOND_PLAYER_INDEX = 3;
const int START_PORTAL_INDEX = 4;
const int END_PORTAL_INDEX = 5;
const int START_TEXT_INDEX = 998;
const int ORB_INDEX = 999;
const int COMMANDER_INDEX = 1000;

// Sizes
const int PLAYER_SIZE = 32;
const int MONSTER_SIZE = 128;
const int END_PORTAL_SIZE = 128;
const int START_PORTAL_SIZE = 64;
const int COMMANDER_HEIGHT = 37;
const int COMMANDER_WIDTH = 19;
const int TILE_SIZE = 32;
const int MAP_ROWS = 10;
const int MAP_COLS = 15;
const int CHUNK_SIZE = 5;
const int TILE_PRELOAD_AHEAD = 5;
const float ORIGINAL_SPEED = 150.0f;

// Collision boxes
const SDL_FRect RUN_COLLISION = { 7, 5, 16, 27 };
const SDL_FRect SLIDE_COLLISION = { 1, 21, 30, 11 };
const SDL_FRect INVINCIBLE_COLLISION = { 0, 0, 0, 0 };
const SDL_FRect DIED_COLLISION = { 7, -500, 16, 600 };
const SDL_FRect START_PORTAL_COLLISION = { 0, 0, 0, 0 };
const SDL_FRect END_PORTAL_COLLISION = { 50, -96, 28, 128 };
const SDL_FRect COMMANDER_COLLISION = { -50, -96, 28, 128 };

// External variables
extern int TOTAL_TILE;
extern int OBSTACLES;
extern int FLOOR_TILES;
extern bool PLAYING;
extern bool BIOME_UPDATE;
extern int CURRENT_MAP_SIZE;
extern std::vector<UIButton> buttons;

// All biomes' textures
extern std::unordered_map<std::string, Biome> biomeTexturesMap;

// FUNCTION PROTOTYPES
// ----- I/ SCREENS -----
void runHomeScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);
void runPlayingFrame(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTimee);
void runHomePauseScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);
void runPlayingPauseScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);
void runGameOverScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);
void runTransitionScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);
void runChangePlayerScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime);

// ----- II/ GAME LOGIC -----
void resetForNewBiome(SDLState& state, GameState& gs, Resources& res);
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime);
void updateObject(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
void manageTiles(const SDLState& state, GameState& gs, Resources& res, bool isUpdate);
void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown);
void resetGame(SDLState& state, GameState& gs, Resources& res, std::vector<float>& scrollPositions);

// ----- III/ COLLISION -----
void checkCollision(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB, float deltaTime);
void collisionResponse(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB,
    SDL_FRect rectA, SDL_FRect rectB, SDL_FRect rectC, float deltaTime);
bool checkGrounded(const GameObject& player, const std::vector<std::unique_ptr<GameObject>>& levelTiles);

// ----- IV/ UTILITIES -----
void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime);
void cleanupOffscreenObjects(GameState& gs);
//void countObjectsWithTexture(const GameState& game);