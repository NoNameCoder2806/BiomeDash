#include "GameState.h"

#include <random>
#include <algorithm>
#include <format>

#include "entities/GameObject.h"
#include "entities/Player.h"
#include "entities/Monster.h"
#include "world/Biome.h"
#include "SDLState.h"

GameState::GameState(const SDLState& state, std::string biomeName) : playerIndex(1)
{
	mapViewport = SDL_FRect
	{
		.x = 0,
		.y = 0,
		.w = static_cast<float>(state.logW),
		.h = static_cast<float>(state.logH)
	};

	backgroundScroll = 0.f;
	debugMode = false;
	invincibleMode = false;

	// Set the biome according to the name
	currentBiome = new Biome;
	currentBiome->name = biomeName;

	gameMap = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{2, 0, 0, 0, 0, 0, 0, 0, 1000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10}
	};

	loadedLeftCol = 0;
	loadedRightCol = gameMap.at(0).size() - 1;

	currentTile = 5;
	lastChunkEmpty = false;

	currentBiome->loadBiome(currentBiome->name);

	needTransition = false;
	portalGenerated = false;

	currentCharacter = 0;

	prevScreen = ScreenState::home;
	screen = ScreenState::home;
	nextScreen = ScreenState::home;

	difficulty = Difficulty::easy;

	cameraZoom = 1.0f;
	targetZoom = 3.0f;
}

Player& GameState::player()                  // The player() function quickly returns
{                                     // a reference to the player object.
	return static_cast<Player&>(*layers[LAYER_IDX_PLAYER][0]);
}

int GameState::biomeCount()
{
	return biomeList.size() - unusedBiomes.size() + 1;
}

float GameState::getScore()
{
	return player().getPosition().x - 265;
}

float GameState::getOriginalSpeed()
{
	if (difficulty == Difficulty::easy)
	{
		return 125.0f;
	}
	else if (difficulty == Difficulty::medium)
	{
		return 150.0f;
	}
	else if (difficulty == Difficulty::hard)
	{
		return 175.0f;
	}
}

Monster& GameState::monster()
{
	return static_cast<Monster&>(*layers[LAYER_IDX_MONSTER][0]);
}

void GameState::preloadBiomes(Resources& res, SDLState& state)
{
	// Load the Transition biome
	currentBiome->name = "Transition";

	// Read and load all the textures
	currentBiome->loadBiome(currentBiome->name);          // parse the text file
	currentBiome->loadTextures(res, state.renderer); // actually load images to GPU

	// Iterate through all the biomes and load the textures
	for (std::string name : biomeList)
	{
		// Change the name of the current biome
		currentBiome->name = name;

		// Debug 
		std::cout << "Preloading " << name << std::endl;

		// Read and load all the textures
		currentBiome->loadBiome(name);          // parse the text file
		currentBiome->loadTextures(res, state.renderer); // actually load images to GPU
	}
}

void GameState::resetGameState(const SDLState& state, const std::string& biomeName)
{
	// Reset the map
	resetMapForBiome(biomeName);

	// Reload biome
	//updateBiome(biomeName);

	// Reset map tracking
	loadedLeftCol = 0;
	loadedRightCol = gameMap.at(0).size() - 1;
	currentTile = 5;
	lastChunkEmpty = false;

	// Reset camera/scroll
	mapViewport.x = 0;
	backgroundScroll = 0.f;

	// Reset the portal flag
	portalGenerated = false;
}

void GameState::resetMapForBiome(const std::string& biomeName)
{
	// Change the map
	if (currentBiome->name == "Transition")
	{
		// Check to see if the player has completed all the biomes
		if (unusedBiomes.size() == 0)
		{
			// If they have, we spawn the commander instead of the portal
			gameMap = {
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10}
			};
		}
		// Otherwise, we spawn the portal
		else
		{
			gameMap = {
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10}
			};
		}
	}
	else
	{
		gameMap = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10}
		};
	}

	// Debug: isplay the new Map
	//std::cout << "New Map: " << std::endl;

	//for (size_t row = 0; row < gameMap.size(); ++row)
	//{
	//	for (size_t col = 0; col < gameMap[row].size(); ++col)
	//	{
	//		std::cout << gameMap[row][col] << " ";
	//	}
	//	std::cout << std::endl; // Move to the next row
	//}
}

void GameState::updateBiome(std::string biomeName)
{
	currentBiome->loadBiome(biomeName);
}

void GameState::updateCurrentTile()
{
	if (!layers[LAYER_IDX_PLAYER].empty())
	{
		currentTile = static_cast<int>(player().getPosition().x / 32);
	}
}

int GameState::randomInt(int min, int max)
{
	static std::random_device rd;    // seed
	static std::mt19937 gen(rd());   // Mersenne Twister engine
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

void GameState::updateMap()
{
	if (currentTile + 25 > loadedRightCol)
	{
		generateMap();
	}

	if (currentTile - 25 > loadedLeftCol && currentTile - 25 > 0)
	{
		deleteTiles();
	}
}

void GameState::generateMap()
{
	// If this is a Transition biome, we don't generate obstacles
	if (currentBiome->name == "Transition")
	{
		return;
	}

	// If the portal is generated
	if (portalGenerated)
	{
		// Then we only generated empty chunks
		generateObstacleChunk(0);

		return;
	}

	// If the score / distance is above 2500px
	if (getScore() >= MINIMAL_SCORE)
	{
		// Generate an empty chunk
		generateObstacleChunk(0);

		// Generate an obstacle chunk that has a portal
		generateObstacleChunk(-1);

		return;
	}

	// Randomize the chance of having obstacles
	int chance = randomInt(0, 99);

	// Check whether the last chunk was empty
	if (lastChunkEmpty)
	{
		// If it was, assign the chance to 99 (guaranteed obstacle generation)
		chance = 99;
	}

	if (chance < 67)  // No obstacle
	{
		generateObstacleChunk(0);
		lastChunkEmpty = true;
	}
	else              // Generate obstacles 
	{
		// Fix 10 -> 20 if you want 2 obstacles
		int obstaclesChance = randomInt(0, 10);
		int obstaclesCount = 0;

		// 5% chance to have 2 obstacles
		if (obstaclesChance <= 15)
		{
			obstaclesCount = 1;
		}
		else if (obstaclesChance > 15 && obstaclesChance <= 20)
		{
			obstaclesCount = 2;
		}

		generateObstacleChunk(obstaclesCount);

		lastChunkEmpty = false;
	}
}

void GameState::generateObstacleChunk(int obstaclesCount)
{
	size_t numRows = gameMap.size();
	int newColIndex = gameMap[0].size();

	// Obstacle chunk 
	std::vector<std::vector<short>> obstacleChunk;

	// Create a vector to store all the obstacles
	std::vector<short> obstaclesMaps;

	// Floor tiles vector
	std::vector<short> floorTiles;

	// Get all the ids from the maps
	// Tripped obstacles
	for (auto& obj : currentBiome->tripped)
	{
		obstaclesMaps.push_back(obj.first);
	}

	// Wall obstacles
	for (auto& obj : currentBiome->wall)
	{
		obstaclesMaps.push_back(obj.first);
	}

	// Burnt obstacles 
	for (auto& obj : currentBiome->burnt)
	{
		obstaclesMaps.push_back(obj.first);
	}

	// Spike obstacles
	for (auto& obj : currentBiome->spike)
	{
		obstaclesMaps.push_back(obj.first);
	}

	// Floor tiles 
	for (auto& obj : currentBiome->floor)
	{
		floorTiles.push_back(obj.first);
	}

	// Helper to build the floor by randomizing the set of 5 tiles
	auto generateFloorRow = [&](int setSize = 5)
		{
			std::vector<short> row;

			if (!floorTiles.empty() && floorTiles.size() >= setSize) {
				// how many full sets do we have?
				int numSets = floorTiles.size() / setSize;

				// pick one set at random
				int setIndex = randomInt(0, numSets - 1);

				// starting position inside the vector
				int start = setIndex * setSize;

				// take exactly 'setSize' tiles from that set
				for (int i = 0; i < setSize; i++) {
					row.push_back(floorTiles[start + i]);
				}
			}
			else
			{
				// fallback if not enough floorTiles
				for (int i = 0; i < setSize; i++)
				{
					row.push_back(5);
				}
			}

			// Debug print
			/*std::cout << "Floor tile IDs: ";
			for (short id : floorTiles) std::cout << id << " ";
			std::cout << std::endl;*/

			return row;
		};

	if (obstaclesCount == -1)
	{
		// Portal generation
		obstacleChunk = {
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 5, 0, 0},
			generateFloorRow()
		};

		// Set the flag to true
		portalGenerated = true;
	}
	else if (obstaclesCount == 0)
	{
		obstacleChunk = {
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0},
			generateFloorRow()
		};
	}
	else if (obstaclesCount == 1)
	{
		// Pick which type of obstacle
		int chance = randomInt(0, obstaclesMaps.size());

		if (chance == obstaclesMaps.size())
		{
			std::vector<short> floor = generateFloorRow();
			floor.at(1) = 0;
			floor.at(2) = 0;
			floor.at(3) = 0;

			obstacleChunk = {
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				floor
			};
		}
		else
		{
			short obsID = obstaclesMaps[chance];

			obstacleChunk = {
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, obsID, 0, 0},
				generateFloorRow()
			};
		}
	}
	else if (obstaclesCount == 2)
	{
		// Get 2 IDs for 2 obstacles
		short obs1ID = obstaclesMaps[randomInt(0, obstaclesMaps.size())];
		short obs2ID = obstaclesMaps[randomInt(0, obstaclesMaps.size())];

		// Create the chunk
		obstacleChunk = {
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0},
				{0, obs1ID, 0, 0, obs2ID},
				generateFloorRow()
		};
	}

	// Append new chunk columns
	for (size_t i = 0; i < gameMap.size(); i++)
	{
		gameMap[i].insert(gameMap[i].end(), obstacleChunk[i].begin(), obstacleChunk[i].end());
	}

	loadedRightCol += 5;
	loadedLeftCol = std::max(0, loadedRightCol - 30);
}

void GameState::deleteTiles()
{
	if (loadedLeftCol >= 0)
	{
		for (auto& v : gameMap)
		{
			v[loadedLeftCol] = 0;
		}

		loadedLeftCol += 1;  // update left boundary
	}
}

void GameState::displayPlayerInformation(SDLState& state)
{
	std::string strState = "";

	Player& p = static_cast<Player&>(player());

	switch (p.getState())
	{
		case PlayerState::idle: // PlayerState::idle
		{
			strState = "idle";
			break;
		}
		case PlayerState::running: // PlayerState::running
		{
			strState = "running";
			break;
		}
		case  PlayerState::jumping: // PlayerState::jumping
		{
			strState = "jumping";
			break;
		}
		case PlayerState::sliding: // PlayerState::sliding
		{
			strState = "sliding";
			break;
		}
		case PlayerState::tripped: // PlayerState::tripped
		{
			strState = "tripped";
			break;
		}
		case PlayerState::knocked: // PlayerState::knocked
		{
			strState = "knocked";
			break;
		}
		case PlayerState::burnt: // PlayerState::burnt
		{
			strState = "burnt";
			break;
		}
		/*case PlayerState::speeding: // PlayerState::speeding
		{
			strState = "speeding";
			break;
		}*/
		case PlayerState::falling: // PlayerState::falling
		{
			strState = "falling";
			break;
		}
		case PlayerState::caught: // PlayerState::caught
		{
			strState = "caught";
			break;
		}
		case PlayerState::bleed: // PlayerState::bleed
		{
			strState = "bleed";
			break;
		}
		case PlayerState::won:  // PlayerState::won
		{
			strState = "won";
			break;
		}
	}
	SDL_RenderDebugText(state.renderer, 5, 5, std::format("----- Player -----").c_str());
	SDL_RenderDebugText(state.renderer, 5, 25, std::format("State: {}\n", strState).c_str());
	SDL_RenderDebugText(state.renderer, 5, 125, std::format("Velocity: {:.0f}", player().getVelocity().x).c_str());
	SDL_RenderDebugText(state.renderer, 5, 65, std::format("X-position: {:.0f}", player().getPosition().x).c_str());
	SDL_RenderDebugText(state.renderer, 5, 85, std::format("Y-position: {:.0f}", player().getPosition().y).c_str());
	SDL_RenderDebugText(state.renderer, 5, 105, std::format("Invincible: {}", invincibleMode).c_str());
}

void GameState::displayGameInformation(SDLState& state)
{
	SDL_RenderDebugText(state.renderer, 165, 5, std::format("----- Game -----").c_str());
	SDL_RenderDebugText(state.renderer, 165, 25, std::format("Current Tile: {}", currentTile).c_str());
	SDL_RenderDebugText(state.renderer, 165, 45, std::format("Distance: {:.0f} px", player().getPosition().x - 320).c_str());

	if (layers[LAYER_IDX_MONSTER].size() != 0)
	{
		SDL_RenderDebugText(state.renderer, 165, 65, std::format("Monster X-position: {:.0f}", monster().getPosition().x).c_str());
		SDL_RenderDebugText(state.renderer, 165, 85, std::format("Monster Y-position: {:.0f}", monster().getPosition().y).c_str());

	}
}

void GameState::displaySectionBarrier(SDLState& state)
{
	for (int y = 5; y <= 130; y += 5)
	{
		SDL_RenderDebugText(state.renderer, 150, y, "|");
	}
}