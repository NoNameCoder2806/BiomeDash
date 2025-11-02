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

const size_t LAYER_IDX_PLAYER = 0;          // Index for the player layer in 'layers' array, including monster, player, ground tiles
const size_t LAYER_IDX_MONSTER = 1;
const size_t LAYER_IDX_OBSTACLES = 2;       // Index for obstacle background tiles
const size_t LAYER_IDX_LEVEL = 3;           // Index for the level tiles and walls of the map

struct GameState
{
	//GameObject player;                    // Player game object
	int playerIndex = 0;

	std::array<std::vector<std::unique_ptr<GameObject>>, 4> layers;    // Three layers: level + walls + obstacles
	std::vector<GameObject> mapTiles;     // Objects in the map

	SDL_FRect mapViewport;                // The camera / visible area of the level
	float backgroundScroll;               // Parallax scrolling offsets

	bool debugMode;                       // Debug mode

	std::vector<std::vector<short>> gameMap;  // Game Map

	int loadedLeftCol;    // Leftmost loaded column index
	int loadedRightCol;   // Rightmost loaded column index (initially full 15 columns)

	int currentTile;

	bool lastChunkEmpty;

	Biome currentBiome;

	const std::vector<std::string> biomeList = {"Swamp", "Industrial_Zone", "Pirate_Bay"};

	GameState(const SDLState& state) : playerIndex(1)
	{
		mapViewport = SDL_FRect{
			.x = 0,
			.y = 0,
			.w = static_cast<float>(state.logW),
			.h = static_cast<float>(state.logH)
		};

		backgroundScroll = 0.f;
		debugMode = false;

		gameMap = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10, 6, 7, 8, 9, 10}
		};

		/*gameMap = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 152, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}
		};*/
		/*
		gameMap = {
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
		};*/

		loadedLeftCol = 0;
		loadedRightCol = gameMap.at(0).size() - 1;

		currentTile = 5;
		lastChunkEmpty = false;

		//std::cout << "Current Biome: " << currentBiome.name << std::endl;
		//currentBiome.loadBiome(currentBiome.name);
		currentBiome.loadBiome(currentBiome.name);
	}

	Player& player()                  // The player() function quickly returns
	{                                     // a reference to the player object.
		return static_cast<Player&>(*layers[LAYER_IDX_PLAYER][0]);
	}

	Monster& monster()
	{
		return static_cast<Monster&>(*layers[LAYER_IDX_MONSTER][0]);
	}

	void updateBiome(std::string biomeName)
	{
		currentBiome.loadBiome(biomeName);
	}

	void updateCurrentTile()
	{
		if (!layers[LAYER_IDX_PLAYER].empty())
		{
			currentTile = static_cast<int>(player().getPosition().x / 32);
		}
	}

	int randomInt(int min, int max) {
		static std::random_device rd;    // seed
		static std::mt19937 gen(rd());   // Mersenne Twister engine
		std::uniform_int_distribution<> dist(min, max);
		return dist(gen);
	}

	void updateMap()
	{
		if (currentTile + 25 > loadedRightCol)
		{
			generateMap();
		}

		if (currentTile - 5 > loadedLeftCol && currentTile - 5 > 0)
		{
			deleteTiles();
		}
	}

	void generateMap()
	{
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
			lastChunkEmpty = false;
			int obstaclesChance = randomInt(0, 10);
			int obstaclesCount = 0;

			// 5% chance to have 2 obstacles
			if (obstaclesChance <= 19)
			{
				obstaclesCount = 1;
			}
			else if (obstaclesChance == 20)
			{
				obstaclesCount = 2;
			}

			generateObstacleChunk(obstaclesCount);

			lastChunkEmpty = false;
		}
	}

	void generateObstacleChunk(int obstaclesCount)
	{
		size_t numRows = gameMap.size();
		int newColIndex = gameMap[0].size();

		// Obstacle chunk 
		std::vector<std::vector<short>> obstacleChunk;

		// All the obstacles from each types
		std::vector<short> trippedObstacles;
		std::vector<short> wallObstacles;
		std::vector<short> burntObstacles;
		std::vector<short> spikeObstacles;
		std::vector<short> floorTiles;
		
		// Get all the ids from the maps 
		// Tripped obstacles
		for (auto& obj : currentBiome.tripped)
		{
			trippedObstacles.push_back(obj.first);
		}

		// Wall obstacles
		for (auto& obj : currentBiome.wall)
		{
			wallObstacles.push_back(obj.first);
		}

		// Burnt obstacles 
		for (auto& obj : currentBiome.burnt)
		{
			burntObstacles.push_back(obj.first);
		}

		// Spike obstacles
		for (auto& obj : currentBiome.spike)
		{
			spikeObstacles.push_back(obj.first);
		}

		// Floor tiles 
		for (auto& obj : currentBiome.floor)
		{
			floorTiles.push_back(obj.first);
		}

		// Helper to build the floor by randomizing the set of 5 tiles
		auto generateFloorRow = [&](int setSize = 5) {
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
				for (int i = 0; i < setSize; i++) {
					row.push_back(5);
				}
			}

			return row;
			};

		if (obstaclesCount == 0)
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
			int chance = randomInt(0, 99);

			if (chance < 20)  // Tripped
			{
				short chosenID = trippedObstacles[randomInt(0, trippedObstacles.size() - 1)];
				obstacleChunk = {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, chosenID, 0, 0},
					generateFloorRow()
				};
			}
			else if (chance < 40)  // Walls
			{
				short chosenID = wallObstacles[randomInt(0, wallObstacles.size() - 1)];
				obstacleChunk = {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, chosenID, 0, 0},
					generateFloorRow()
				};
			}
			else if (chance < 60)  // Burnt
			{
				short chosenID = burntObstacles[randomInt(0, burntObstacles.size() - 1)];
				obstacleChunk = {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, chosenID, 0, 0},
					generateFloorRow()
				};
			}
			else if (chance < 80)  // Spike
			{
				short chosenID = spikeObstacles[randomInt(0, spikeObstacles.size() - 1)];
				obstacleChunk = {
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, 0, 0, 0},
					{0, 0, chosenID, 0, 0},
					generateFloorRow()
				};
			}
			else // Falling
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
		}
		else if (obstaclesCount == 2)
		{
			// Create a pool of wall and tripped obstacles
			std::vector<std::vector<short>*> pools1 = {
				&trippedObstacles,
				&wallObstacles,
			};

			// Create a pool of spike and burnt obstacles
			std::vector<std::vector<short>*> pools2 = {
				&burntObstacles,
				&spikeObstacles
			};

			// Pick first obstacle
			const std::vector<short>* pool1 = pools1[randomInt(0, pools1.size() - 1)];
			short chosenID1 = (*pool1)[randomInt(0, pool1->size() - 1)];

			// Pick second obstacle
			const std::vector<short>* pool2 = pools2[randomInt(0, pools2.size() - 1)];
			short chosenID2 = (*pool2)[randomInt(0, pool2->size() - 1)];

			// chosenID1 and chosenID2 now hold your 2 random picks
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
					{0, chosenID1, 0, 0, chosenID2},
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

	void deleteTiles()
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

	void displayPlayerInformation(SDLState& state)
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
			case PlayerState::speeding: // PlayerState::speeding
			{
				strState = "speeding";
				break;
			}
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
		}
		SDL_RenderDebugText(state.renderer, 5, 5, std::format("----- Player -----").c_str());
		SDL_RenderDebugText(state.renderer, 5, 25, std::format("State: {}\n", strState).c_str());
		SDL_RenderDebugText(state.renderer, 5, 45, std::format("Velocity: {:.0f}", player().getVelocity().x).c_str());
		SDL_RenderDebugText(state.renderer, 5, 65, std::format("X-position: {:.0f}", player().getPosition().x).c_str());
		SDL_RenderDebugText(state.renderer, 5, 85, std::format("Y-position: {:.0f}", player().getPosition().y).c_str());
		SDL_RenderDebugText(state.renderer, 5, 105, std::format("Grounded: {}", player().isGrounded()).c_str());
		SDL_RenderDebugText(state.renderer, 5, 125, std::format("Jump Requested: {}", player().isJumpRequested()).c_str());
		SDL_RenderDebugText(state.renderer, 5, 145, std::format("Slide Requested: {}", player().isSlideRequested()).c_str());
		SDL_RenderDebugText(state.renderer, 5, 185, std::format("Jump Buffer Time: {}", player().getJumpBufferTime() * 100).c_str());
	}

	void displayGameInformation(SDLState& state)
	{
		SDL_RenderDebugText(state.renderer, 165, 5, std::format("----- Game -----").c_str());
		SDL_RenderDebugText(state.renderer, 165, 25, std::format("Current Tile: {}", currentTile).c_str());
		SDL_RenderDebugText(state.renderer, 165, 45, std::format("Distance: {:.0f} px", player().getPosition().x - 320).c_str());
		SDL_RenderDebugText(state.renderer, 165, 65, std::format("Left: {}", loadedLeftCol).c_str());
		SDL_RenderDebugText(state.renderer, 165, 85, std::format("Right: {}", loadedRightCol).c_str());
		SDL_RenderDebugText(state.renderer, 165, 105, std::format("Current Biome: {}", currentBiome.name).c_str());
		SDL_RenderDebugText(state.renderer, 165, 125, std::format("Background: {}", currentBiome.background).c_str());
	}

	void displaySectionBarrier(SDLState& state)
	{
		for (int y = 5; y <= 130; y += 5)
		{
			SDL_RenderDebugText(state.renderer, 150, y, "|");
		}
	}	
};