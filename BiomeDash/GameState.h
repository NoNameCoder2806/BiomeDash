#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <array>
#include <random>
#include <cmath>

#include "entities/GameObject.h"
#include "entities/Player.h"
#include "entities/Monster.h"
#include "entities/Transition.h"

#include "world/Biome.h"

#include "SDLState.h"
#include "UIState.h"
#include "ui/UILabel.h"

const size_t LAYER_IDX_PLAYER = 3;          // Index for the player layer in 'layers' array, including monster, player, ground tiles
const size_t LAYER_IDX_MONSTER = 2;
const size_t LAYER_IDX_OBSTACLES = 1;       // Index for obstacle background tiles
const size_t LAYER_IDX_LEVEL = 0;           // Index for the level tiles and walls of the map
const std::string TRANSITION_BIOME = "";
const int MINIMAL_SCORE = 2000;

// ----- SCREEN STATES -----
enum class ScreenState
{
	home, playing, homePause, playingPause, gameOver, transition, changePlayer, won, credits, exit
};

// ----- DIFFICULTY -----
enum class Difficulty
{
	easy, medium, hard
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

	Biome* currentBiome = nullptr;

	bool needTransition;
	bool portalGenerated;
	bool worldReady = true;
	bool needReset = false;

	// UIState
	UIState ui;

	// UILabel
	std::unique_ptr<UILabel> title;
	std::unique_ptr<UILabel> titleShadow;
	glm::vec2 titleWorldPos;
	std::unique_ptr<UILabel> characterName;
	glm::vec2 characterNamePos;

	const std::vector<std::string> fullBiomeList = { "Transition", "Swamp", "Industrial_Zone", "Pirate_Bay", "Power_Station" };
	const std::vector<std::string> biomeList = { "Swamp", "Industrial_Zone", "Pirate_Bay", "Power_Station" };
	std::vector<std::string> unusedBiomes = { "Swamp", "Industrial_Zone", "Pirate_Bay", "Power_Station" };
	
	// Debug
	//const std::vector<std::string> fullBiomeList = { "Transition", "Swamp" };
	//const std::vector<std::string> biomeList = { "Swamp" };
	//std::vector<std::string> unusedBiomes = { "Swamp" };

	const std::vector<std::string> fullCharactersList = { "hooded_hero", "red_meppo", "bad_meppo", "soldier_meppo", "spring_trap"};
	const std::vector<std::string> fullCharactersNickNames = { "RED HOODIE", "RED MEPPO", "CIGAREPPO", "FIGHTYEPPO", "SPRINGTRAP"};
	const std::string defaultCharacterName = "hooded_hero";

	// Current character index
	int currentCharacter;

	// ScreenState
	ScreenState prevScreen;
	ScreenState screen;
	ScreenState nextScreen;

	// Difficulty
	Difficulty difficulty;

	// Stars
	int stars = 0;

	// Credits
	std::vector<std::unique_ptr<UILabel>> creditLines;
	SDL_FRect creditRect;
	float creditsScrollOffset = 0.0f;
	float creditScrollSpeed = 30.0f;

	// Transition
	Transition transition;

	// Camera Zoom
	float cameraZoom;
	float targetZoom;

	GameState(const SDLState& state, std::string biomeName = "Transition");

	Player& player();                  // The player() function quickly returns

	int biomeCount();

	float getScore();

	float getOriginalSpeed();

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

	void loadCredits(SDLState& state, const std::string& filepath);

	void displayPlayerInformation(SDLState& state);

	void displayGameInformation(SDLState& state);

	void displaySectionBarrier(SDLState& state);
};