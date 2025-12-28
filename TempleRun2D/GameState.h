#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <array>
#include <random>
#include <cmath>

#include "entities/GameObject.h"
#include "entities/Player.h"
#include "entities/Monster.h"

#include "world/Biome.h"

#include "SDLState.h"

const size_t LAYER_IDX_PLAYER = 3;          // Index for the player layer in 'layers' array, including monster, player, ground tiles
const size_t LAYER_IDX_MONSTER = 2;
const size_t LAYER_IDX_OBSTACLES = 1;       // Index for obstacle background tiles
const size_t LAYER_IDX_LEVEL = 0;           // Index for the level tiles and walls of the map
const std::string TRANSITION_BIOME = "";
const int MINIMAL_SCORE = 1000;

// ----- SCREEN STATES -----
enum class ScreenState
{
	home, playing, pause, gameOver
};

// ----- GAMESTATE STRUCT -----
struct GameState
{
	//GameObject player;                    // Player game object
	int playerIndex = 0;

	std::array<std::vector<std::unique_ptr<GameObject>>, 4> layers;    // Three layers: level + walls + obstacles
	std::vector<GameObject> mapTiles;     // Objects in the map

	SDL_FRect mapViewport;                // The camera / visible area of the level
	float backgroundScroll;               // Parallax scrolling offsets

	bool debugMode;                       // Debug mode
	bool invincibleMode;                   // Invincible mode

	std::vector<std::vector<short>> gameMap;  // Game Map

	int loadedLeftCol;    // Leftmost loaded column index
	int loadedRightCol;   // Rightmost loaded column index (initially full 15 columns)

	int currentTile;

	bool lastChunkEmpty;

	Biome currentBiome;

	bool needTransition;
	bool portalGenerated;

	bool worldReady = true;

	const std::vector<std::string> biomeList = { "Swamp", "Industrial_Zone", "Pirate_Bay", "Power_Station"};
	std::vector<std::string> unusedBiomes = { "Swamp", "Industrial_Zone", "Pirate_Bay", "Power_Station"};

	ScreenState screen;

	GameState(const SDLState& state, std::string biomeName = "Transition");

	Player& player();                  // The player() function quickly returns

	int biomeCount();

	float getScore();

	Monster& monster();

	void preloadBiomes(Resources& res, SDLState& state);

	void resetGameState(const SDLState& state, const std::string& biomeName = "Transition");

	void resetMapForBiome(const std::string& biomeName);

	void updateBiome(std::string biomeName);

	void updateCurrentTile();

	int randomInt(int min, int max);

	void updateMap();

	void generateMap();

	void generateObstacleChunk(int obstaclesCount);

	void deleteTiles();

	void displayPlayerInformation(SDLState& state);

	void displayGameInformation(SDLState& state);

	void displaySectionBarrier(SDLState& state);
};