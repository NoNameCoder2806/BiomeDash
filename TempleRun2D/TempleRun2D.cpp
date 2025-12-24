// TempleRun2D.cpp : Defines the entry point for the application.
//

// Libraries
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

// Header files
#include "TempleRun2D.h"
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

// Libraries
#include <vector>
#include <array>
#include <string>
#include <format>

using namespace std;

// Constants
// Game tiles / characters index
const int PLAYER_INDEX = 1;
const int MONSTER_INDEX = 2;
const int SECOND_PLAYER_INDEX = 3;
const int START_PORTAL_INDEX = 4;
const int END_PORTAL_INDEX = 5;
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
const SDL_FRect BURNT_COLLISION = { 0, 0, 0, 0 };
const SDL_FRect INVINCIBLE_COLLISION = { 0, 0, 0, 0 };
const SDL_FRect DIED_COLLISION = { 7, 0, 16, 100 };
const SDL_FRect START_PORTAL_COLLISION = { 0, 0, 0, 0 };
const SDL_FRect END_PORTAL_COLLISION = { 50, -96, 28, 128 };
const SDL_FRect COMMANDER_COLLISION = { -50, -96, 28, 128 };

// Static variables
static int TOTAL_TILE = 0;
static int OBSTACLES = 0;
static int FLOOR_TILES = 0;
static bool PLAYING = false;
static bool BIOME_UPDATE = true;
static int CURRENT_MAP_SIZE = 0;

// Function prototypes
void resetForNewBiome(SDLState& state, GameState& gs, Resources& res);

//void countObjectsWithTexture(const GameState& game);

void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime);

void cleanupOffscreenObjects(GameState& gs);

void updateObject(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);

void checkCollision(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB, float deltaTime);

void collisionResponse(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB,
	SDL_FRect rectA, SDL_FRect rectB, SDL_FRect rectC, float deltaTime);

void manageTiles(const SDLState& state, GameState& gs, Resources& res, bool isUpdate);

void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown);

bool checkGrounded(const GameObject& player, const std::vector<std::unique_ptr<GameObject>>& levelTiles);

void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime);

int main(int argc, char* argv[])
{
	// Create an SDLState object
	SDLState sdl;

	if (!sdl.initialize())
	{
		return 1;
	}

	for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
	{
		if (sdl.keys[i])
		{
			printf("Key %d = %d\n", i, sdl.keys[i]);
		}
	}

	// Create a GameState and Resources object
	GameState game(sdl, "Transition");    // Create a GameState object
	game.updateBiome(game.currentBiome.name);
	Resources res;          // Create a Resources object
	res.load(sdl, "default_character", "default_monster", game.currentBiome.name, game.currentBiome.parallaxBackgrounds);          // Load our player and monster

	// Load the current textures of the biome
	game.currentBiome.loadTextures(res, sdl.renderer);

	// Start the map size at 0
	CURRENT_MAP_SIZE = 0;

	// Create the game tiles
	manageTiles(sdl, game, res, false);

	// Store the time into prevTime
	uint64_t prevTime = SDL_GetTicks();

	// Count FPS
	int frames = 0;
	float fps = 0.0f;
	uint64_t fpsLastTime = SDL_GetTicks();

	// Declare a vector to hold the background scroll rates
	std::vector<float> scrollPositions;
	scrollPositions.resize(res.parallaxBackgrounds.size(), 0.0f);

	// Start the game loop
	bool running = true;
	while (running)
	{
		// Check whether the game needs to transition / change biome
		if (game.needTransition)
		{
			// Call the function to reset all the textures and game state
			resetForNewBiome(sdl, game, res);

			// Reset the flag
			game.needTransition = false;
		}

		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (float)(nowTime - prevTime) / 1000;    // Convert to seconds

		// Increment the frame count
		frames++;
		uint64_t fpsNow = SDL_GetTicks();
		if (fpsNow - fpsLastTime >= 1000)
		{
			fps = frames * 1000.0f / (fpsNow - fpsLastTime);
			frames = 0;
			fpsLastTime = fpsNow;
		}

		// Event polling loop
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_EVENT_QUIT:
				{
					running = false;

					break;
				}

				case SDL_EVENT_WINDOW_RESIZED:
				{
					sdl.width = e.window.data1;
					sdl.height = e.window.data2;

					break;
				}

				case SDL_EVENT_KEY_DOWN:
				{
					handleKeyInput(sdl, game, game.player(), e.key.scancode, true);

					break;
				}

				case SDL_EVENT_KEY_UP:
				{
					handleKeyInput(sdl, game, game.player(), e.key.scancode, false);

					// Full screen
					if (e.key.scancode == SDL_SCANCODE_F11)
					{
						sdl.fullscreen = !sdl.fullscreen;
						SDL_SetWindowFullscreen(sdl.window, sdl.fullscreen);
						sdl.width = 1920;
						sdl.height = 1080;
					}

					// Debug mode
					if (e.key.scancode == SDL_SCANCODE_F10)
					{
						game.debugMode = !game.debugMode;
					}

					// Invincibility mode
					if (e.key.scancode == SDL_SCANCODE_G)
					{
						game.invincibleMode = !game.invincibleMode;
					}

					break;
				}
			}
		}

		for (auto& layer : game.layers)
		{
			for (auto& obj : layer)
			{
				updateObject(sdl, game, res, *obj, deltaTime);

				if (obj->getType() == ObjectType::player || obj->getType() == ObjectType::monster)
				{
					// Pass it into updateMovement
					obj->updateMovement(deltaTime);
				}

				if (obj->getCurrentAnimation() != -1)
				{
					obj->getAnimations().at(obj->getCurrentAnimation()).step(deltaTime);
				}
			}
		}

		// Handle collision
		bool foundGround = false;

		for (auto& objPtr : game.layers[LAYER_IDX_PLAYER])
		{
			GameObject& obj = *objPtr;  // Dereference the unique pointer

			// No need to loop through the collision boxes because player only has 1
			// Player world rect
			SDL_FRect pc = obj.getCollider().at(0);
			SDL_FRect pWorld{
				obj.getPosition().x + pc.x,
				obj.getPosition().y + pc.y,
				pc.w,
				pc.h
			};

			// Player-centered collision window (big margin to be safe)
			SDL_FRect cwin{
				pWorld.x - 256.0f,
				pWorld.y - 256.0f,
				pWorld.w + 512.0f,
				pWorld.h + 512.0f
			};

			auto processLayer = [&](auto& layer)
				{
					for (auto& objBPtr : layer)
					{
						GameObject& objB = *objBPtr;    // Dereference the unique pointer

						// Loop through all the collision boxes
						for (int i = 0; i < objB.getCollider().size(); i++)
						{
							SDL_FRect cc = objB.getCollider().at(i);
							SDL_FRect bWorld{
								objB.getPosition().x + cc.x,
								objB.getPosition().y + cc.y,
								cc.w,
								cc.h
							};

							// Fast reject against the player-centered window (all in world coords)
							if (bWorld.x + bWorld.w < cwin.x)    continue;
							if (bWorld.x > cwin.x + cwin.w)      continue;
							if (bWorld.y + bWorld.h < cwin.y)    continue;
							if (bWorld.y > cwin.y + cwin.h)      continue;

							checkCollision(sdl, game, res, obj, objB, deltaTime);
						}
					}
				};

			processLayer(game.layers[LAYER_IDX_MONSTER]);
			processLayer(game.layers[LAYER_IDX_OBSTACLES]);
			processLayer(game.layers[LAYER_IDX_LEVEL]);
		}

		// Update grounded status
		for (auto& player : game.layers[LAYER_IDX_PLAYER])
		{
			bool grounded = player->isGrounded();

			std::string debugText = std::format("Grounded: {}", grounded ? "true" : "false");

			SDL_RenderDebugText(sdl.renderer, 5, 105, debugText.c_str());

			(*player).setGrounded(checkGrounded(*player, game.layers[LAYER_IDX_LEVEL]));
		}

		// Calculate viewport position
		game.mapViewport.x = (game.player().getPosition().x + TILE_SIZE / 2) - game.mapViewport.w / 2;

		game.updateCurrentTile();
		game.updateMap();

		manageTiles(sdl, game, res, true);

		// Print total number of objects
		//countObjectsWithTexture(game);

		// Cleanup offscreen objects
		cleanupOffscreenObjects(game);

		// Draw the background black
		SDL_SetRenderDrawColor(sdl.renderer, 128, 0, 128, 255);   // Dark purple
		SDL_RenderClear(sdl.renderer);

		// Draw background
		SDL_RenderTexture(sdl.renderer, res.background, nullptr, nullptr);

		// Draw the parallax backgrounds
		int layerCount = stoi(game.currentBiome.parallaxBackgrounds);

		// Check the number of scroll positions and resize if necessary
		if (scrollPositions.size() != res.parallaxBackgrounds.size())
		{
			scrollPositions.resize(res.parallaxBackgrounds.size());
		}

		// Draw each parallax background
		for (int i = 0; i < layerCount; i++)
		{
			// Calculate the scroll factor
			float scrollFactor = 0.75f * (i + 1) / layerCount;

			// Draw the background
			drawParalaxBackground(sdl.renderer, res.parallaxBackgrounds[i], game.player().getVelocity().x, scrollPositions[i], scrollFactor, deltaTime);
		}

		// Draw all objects
		for (auto& layer : game.layers)
		{
			for (auto& objPtr : layer)
			{
				bool visible = false;

				GameObject& obj = *objPtr;

				for (const auto& localCollider : obj.getCollider()) // vector of SDL_FRect
				{
					SDL_FRect worldCollider{
						obj.getPosition().x + localCollider.x,
						obj.getPosition().y + localCollider.y - TILE_SIZE, // bottom-left anchor fix
						localCollider.w,
						localCollider.h
					};

					float preloadMargin = TILE_SIZE * 2; // preload 2 tiles ahead

					// If any collider is inside the viewport, mark object visible
					if (!(worldCollider.x + worldCollider.w < game.mapViewport.x - preloadMargin ||
						worldCollider.x > game.mapViewport.x + game.mapViewport.w + preloadMargin ||
						worldCollider.y + worldCollider.h < game.mapViewport.y - preloadMargin ||
						worldCollider.y > game.mapViewport.y + game.mapViewport.h + preloadMargin))
					{
						visible = true;
						break; // no need to check the rest
					}
				}

				if (!visible) continue; // skip drawing this object entirely

				drawObject(sdl, game, obj, deltaTime);
			}
		}

		// Display debug information
		SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, 255);

		// Debug mode
		if (game.debugMode)
		{
			// Display the player information
			game.displayPlayerInformation(sdl);

			// Display game information
			game.displayGameInformation(sdl);

			// Display the barrier
			game.displaySectionBarrier(sdl);

			// Display the PLAYING status
			std::string playingText = std::format("PLAYING: {}", PLAYING ? "true" : "false");
			//SDL_RenderDebugText(sdl.renderer, 5, 165, playingText.c_str());  // Adjust y-coordinate if needed
		}

		// Invincible mode
		if (game.invincibleMode)
		{
			game.player().clearCollider();
			game.player().addCollider(INVINCIBLE_COLLISION);
		}

		// Dark blue: #173F6C
		SDL_SetRenderDrawColor(sdl.renderer, 23, 63, 108, 255);

		// Display FPS
		std::string fpsText = std::format("FPS: {:.1f}", fps);
		SDL_RenderDebugText(sdl.renderer, 5, 5, fpsText.c_str());

		// Display the game score and the number of biomes completed
		string scoreText = format("Score: {:.1f}", game.getScore());
		SDL_RenderDebugText(sdl.renderer, 165, 5, scoreText.c_str());

		// White color
		SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, 255);

		// Render the current game frame
		SDL_RenderPresent(sdl.renderer);

		// Assign nowTime to prevTime to update the time
		prevTime = nowTime;
	}

	// Clean up and destroy all the memories and resources used 
	res.unload();
	sdl.cleanup();

	return 0;
}

void resetForNewBiome(SDLState& state, GameState& gs, Resources& res)
{
	cout << "Reseting the resources and game state" << endl;

	// Generate a new biome name
	string nextBiome;

	// If the current biome is a transition biome
	if (gs.currentBiome.name == "Transition")
	{
		// If there are more biomes to play
		if (gs.unusedBiomes.size() > 0)
		{
			// Then we need to generate one from the normal biomes
			int idx = gs.randomInt(0, gs.unusedBiomes.size() - 1);
			nextBiome = gs.unusedBiomes[idx];
			//nextBiome = "Swamp";
			gs.unusedBiomes.erase(gs.unusedBiomes.begin() + idx);  // remove it so it's not picked again

			// Debug
			cout << "Next biome: " << nextBiome << endl;
			cout << "Remaining: " << gs.unusedBiomes.size() << endl;
		}
		// Otherwise
		else
		{
			// Then we stop the game because the player has won!
			cout << "Game won!" << endl;
		}
	}
	// Otherwise
	else
	{
		// We set the next biome to be transition
		nextBiome = "Transition";
	}

	// Clear the old tiles textures
	res.clearTiles();
	
	// Reset the game state
	gs.resetGameState(state, nextBiome);

	// Load the new biome textures
	gs.currentBiome.loadTextures(res, state.renderer);

	// Reset the resources
	res.reset(state, gs.currentBiome.name, gs.currentBiome.parallaxBackgrounds);

	// Clear the Level layer and Destroy the portal
	gs.layers[LAYER_IDX_LEVEL].clear();

	// Rebuild the game map
	CURRENT_MAP_SIZE = 0;
	manageTiles(state, gs, res, false);

	// Starting positions
	glm::vec2 playerStartPos(state.logW / 2.0f, state.logH / 2.0f + 32.0f);
	glm::vec2 monsterStartPos(state.logW / 2.0f - 10.25 * TILE_SIZE, 32.0f * 9 - 14.0f);

	// Reset player position
	if (!gs.layers[LAYER_IDX_PLAYER].empty())
	{
		gs.player().setPosition(playerStartPos);
	}

	// Check the biome name
	if (gs.currentBiome.name == "Transition")
	{
		// If this is a transition biome, we need to hide the monster
		if (!gs.layers[LAYER_IDX_MONSTER].empty())
		{
			monsterStartPos = glm::vec2(state.logW / 2.0f - 12 * TILE_SIZE, 32.0f * 9 - 14.0f);
			gs.monster().setPosition(monsterStartPos);
		}
	}
	else
	{ 
		// Reset monster position
		if (!gs.layers[LAYER_IDX_MONSTER].empty())
		{
			gs.monster().setPosition(monsterStartPos);
		}
	}

	// Reset the player and monster's speed
	float newSpeed = ORIGINAL_SPEED + (gs.biomeList.size() - gs.unusedBiomes.size() - 1) * 0.1f * ORIGINAL_SPEED;
	gs.player().setSpeed(newSpeed);
	gs.monster().setSpeed(newSpeed);

	// Debug
	//cout << "Done creating tiles!" << endl;
}

//void countObjectsWithTexture(const GameState& game)
//{
//	int count = 0;
//	for (const auto& layer : game.layers)
//	{
//		for (const auto& objPtr : layer)
//		{
//			if (objPtr->getTexture() != nullptr)
//				++count;
//		}
//	}
//	
//	cout << "Total number of Objects: " << count << endl;
//}

void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime)
{
	// Determine the current frame of the current animation
	float srcX = 0.0f;

	if (obj.getCurrentAnimation() != -1)
	{
		srcX = obj.getAnimations().at(obj.getCurrentAnimation()).currentFrame() * obj.getImageSize().x;
	}

	if (obj.getType() == ObjectType::player)
	{
		SDL_FRect src    // Source (from the animation png). Example: 1st frame: 0x32 , 2nd: 1x32 (skips the first 32 pixels), and so on 
		{
			.x = srcX,
			.y = 0,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_FRect dst    // Destination 
		{
			.x = obj.getPosition().x - gs.mapViewport.x,
			.y = obj.getPosition().y - PLAYER_SIZE,
			.w = PLAYER_SIZE,
			.h = PLAYER_SIZE
		};

		// Render the object's texture
		SDL_RenderTexture(state.renderer, obj.getTexture(), &src, &dst);
	}
	else if (obj.getType() == ObjectType::monster)    // Monster
	{
		SDL_FRect src    // Source (from the animation png).  
		{
			.x = srcX,
			.y = 0,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_FRect dst    // Destination 
		{
			.x = obj.getPosition().x - gs.mapViewport.x,
			.y = obj.getPosition().y - TILE_SIZE * 4,
			.w = MONSTER_SIZE,
			.h = MONSTER_SIZE
		};

		// Render the object's texture
		SDL_RenderTexture(state.renderer, obj.getTexture(), &src, &dst);
	}
	else if (obj.getType() == ObjectType::endportal)
	{
		SDL_FRect src
		{
			.x = srcX,
			.y = 0,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_FRect dst    // Destination 
		{
			.x = obj.getPosition().x - gs.mapViewport.x,
			.y = obj.getPosition().y - TILE_SIZE * 4,
			.w = END_PORTAL_SIZE,
			.h = END_PORTAL_SIZE
		};

		// Render the object's texture
		SDL_RenderTexture(state.renderer, obj.getTexture(), &src, &dst);
	}
	else if (obj.getType() == ObjectType::startportal)
	{
		SDL_FRect src
		{
			.x = srcX,
			.y = 0,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_FRect dst    // Destination 
		{
			.x = obj.getPosition().x + obj.getImageOffset().x - gs.mapViewport.x,
			.y = obj.getPosition().y + obj.getImageOffset().y - TILE_SIZE,
			.w = START_PORTAL_SIZE,
			.h = START_PORTAL_SIZE
		};

		// Render the object's texture
		SDL_RenderTexture(state.renderer, obj.getTexture(), &src, &dst);

	}
	else if (obj.getType() == ObjectType::commander)
	{
		SDL_FRect src
		{
			.x = srcX,
			.y = 0,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_FRect dst    // Destination 
		{
			.x = obj.getPosition().x + obj.getImageOffset().x - gs.mapViewport.x,
			.y = obj.getPosition().y + obj.getImageOffset().y - COMMANDER_HEIGHT,
			.w = COMMANDER_WIDTH,
			.h = COMMANDER_HEIGHT
		};

		// Render the object's texture
		SDL_RenderTexture(state.renderer, obj.getTexture(), &src, &dst);

	}
	else    // Obstacles, Floor Tiles 
	{
		SDL_FRect src
		{
			.x = 0,
			.y = 0,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_FRect dst
		{
			.x = obj.getPosition().x + obj.getImageOffset().x - gs.mapViewport.x,
			.y = obj.getPosition().y + obj.getImageOffset().y - TILE_SIZE,
			.w = obj.getImageSize().x,
			.h = obj.getImageSize().y
		};

		SDL_RenderTexture(state.renderer, obj.getTexture(), &src, &dst);
	}

	if (gs.debugMode)
	{
		for (int i = 0; i < obj.getCollider().size(); i++)
		{
			SDL_FRect rectA
			{
				.x = obj.getPosition().x + obj.getCollider().at(i).x - gs.mapViewport.x,
				.y = obj.getPosition().y + obj.getCollider().at(i).y - TILE_SIZE,
				.w = obj.getCollider().at(i).w,
				.h = obj.getCollider().at(i).h
			};

			SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);

			if (i == 0)
				SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 150);   // red
			else if (i == 1)
				SDL_SetRenderDrawColor(state.renderer, 0, 0, 255, 150);   // blue
			else if (i == 2)
				SDL_SetRenderDrawColor(state.renderer, 0, 255, 0, 150);   // green
			else if (i == 3)
				SDL_SetRenderDrawColor(state.renderer, 255, 255, 0, 150); // yellow
			else
				SDL_SetRenderDrawColor(state.renderer, 255, 0, 255, 150); // magenta for 5th+ colliders

			SDL_RenderFillRect(state.renderer, &rectA);
		}
	}
}

void cleanupOffscreenObjects(GameState& gs)
{
	float marginBehind = 300.0f; // how far behind the player to keep objects
	float playerX = gs.player().getPosition().x;

	for (auto& layer : gs.layers)
	{
		layer.erase(
			std::remove_if(layer.begin(), layer.end(),
				[&](std::unique_ptr<GameObject>& obj)
				{
					const auto& colliders = obj->getCollider();
					// Assume object is far behind until proven otherwise
					for (const auto& col : colliders)
					{
						float rightEdge = obj->getPosition().x + col.x + col.w;
						// If any collider is not far behind the player, keep it
						if (rightEdge >= playerX - marginBehind)
						{
							return false;
						}
					}
					return true; // all colliders are far behind
				}),
			layer.end()
		);
	}
}

void updateObject(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime)
{
	// Check which type the object is
	switch (obj.getType())
	{
		// ----- I/ PLAYER -----
		case ObjectType::player:
		{
			Player& player = static_cast<Player&>(obj);

			if (!player.isAlive())
			{
				// still allow processing of certain death states (caught, bleed, etc)
				switch (player.getState())
				{
				case PlayerState::caught:
				case PlayerState::burnt:
				case PlayerState::bleed:
				case PlayerState::knocked:
				case PlayerState::falling:
					break;
				default:
					return;
				}
			}

			switch (player.getState())
			{
			case PlayerState::idle:
			{
				player.setTexture(res.playerIdle);
				player.setCurrentAnimation(res.ANIM_PLAYER_IDLE);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				glm::vec2 v = obj.getVelocity();
				v.y = 0;
				player.setVelocity(v);
				player.setAlive(true);

				break;
			}

			case PlayerState::running:
			{
				player.setTexture(res.playerRun);
				player.setCurrentAnimation(res.ANIM_PLAYER_RUN);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				player.setAlive(true);

				// Reset the sliding animation
				player.getAnimations().at(res.ANIM_PLAYER_SLIDE).getTimer().reset();

				PLAYING = true;

				break;
			}

			case PlayerState::jumping:
			{
				player.setTexture(res.playerJump);
				player.setCurrentAnimation(res.ANIM_PLAYER_JUMP);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				player.setAlive(true);

				// Reset the sliding animation
				player.getAnimations().at(res.ANIM_PLAYER_SLIDE).getTimer().reset();

				break;
			}

			case PlayerState::sliding:
			{
				player.setTexture(res.playerSlide);
				player.setCurrentAnimation(res.ANIM_PLAYER_SLIDE);
				player.clearCollider();
				player.addCollider(SLIDE_COLLISION);
				player.setAlive(true);

				break;
			}

			case PlayerState::tripped:
			{
				player.setTexture(res.playerTripped);
				player.setCurrentAnimation(res.ANIM_PLAYER_TRIPPED);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				player.setAlive(true);

				break;
			}

			case PlayerState::knocked:
			{
				player.setTexture(res.playerKnocked);
				player.setCurrentAnimation(res.ANIM_PLAYER_KNOCKED);
				player.clearCollider();
				player.addCollider(DIED_COLLISION);
				player.setAlive(false);

				// Mark this animation as non-looping
				player.getAnimations().at(res.ANIM_PLAYER_KNOCKED).setLoop(false);

				PLAYING = false;

				break;
			}

			case PlayerState::burnt:
			{
				player.setTexture(res.playerBurnt);
				player.setCurrentAnimation(res.ANIM_PLAYER_BURNT);
				player.clearCollider();
				player.addCollider(BURNT_COLLISION);
				player.setAlive(false);

				// Mark this animation as non-looping
				player.getAnimations().at(res.ANIM_PLAYER_BURNT).setLoop(false);

				PLAYING = false;

				break;
			}

			case PlayerState::bleed:
			{
				player.setTexture(res.playerBleed);
				player.setCurrentAnimation(res.ANIM_PLAYER_BLEED);
				player.clearCollider();
				player.addCollider(DIED_COLLISION);
				player.setAlive(false);

				// Mark the animation as non-looping
				player.getAnimations().at(res.ANIM_PLAYER_BLEED).setLoop(false);

				PLAYING = false;

				break;
			}

			case PlayerState::falling:
			{
				player.setTexture(res.playerFalling);
				player.setCurrentAnimation(res.ANIM_PLAYER_FALLING);
				player.setAlive(false);

				PLAYING = false;

				break;
			}

			case PlayerState::speeding:
			{
				player.setTexture(res.playerSpeeding);
				player.setCurrentAnimation(res.ANIM_PLAYER_SPEEDING);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				player.setAlive(true);

				break;
			}

			case PlayerState::caught:
			{
				player.setTexture(res.playerCaught);
				player.setCurrentAnimation(res.ANIM_PLAYER_CAUGHT);
				player.clearCollider();
				player.addCollider(DIED_COLLISION);
				player.setAlive(false);
				PLAYING = false;

				// Mark the animation as non-looping
				player.getAnimations().at(res.ANIM_PLAYER_CAUGHT).setLoop(false);

				break;
			}
			}

			break;
		}
		// ----- II/ MONSTER -----
		case ObjectType::monster:
		{
			Monster& monster = static_cast<Monster&>(obj);

			switch (monster.getState())
			{
			case MonsterState::idle:
			{
				obj.setTexture(res.monsterIdle);
				obj.setCurrentAnimation(res.ANIM_MONSTER_IDLE);

				if (PLAYING)
				{
					monster.setState(MonsterState::chasing);
				}

				break;
			}

			case MonsterState::chasing:
			{
				monster.setTexture(res.monsterChase);
				monster.setCurrentAnimation(res.ANIM_MONSTER_CHASE);

				monster.setSpeedMultiplier(gs.player().getSpeedMultiplier());

				break;
			}

			case MonsterState::killing:
			{
				monster.setTexture(res.monsterKill);
				monster.setCurrentAnimation(res.ANIM_MONSTER_KILL);

				break;
			}
			}

			break;
		}
		// ----- III/ END PORTAL -----
		case ObjectType::endportal:
		{
			obj.setTexture(res.endPortal);
			obj.setCurrentAnimation(0);

			break;
		}
		// ----- IV/ COMMANDER -----
		case ObjectType::commander:
		{
			// If the object collides with the player
			if (obj.isGrounded())
			{
				// Reverse the animation and texture
				obj.setTexture(res.commander_reversed);
				obj.setCurrentAnimation(res.ANIM_COMMANDER_REVERSED);

				// Remove the monster after the game is won
				gs.layers[LAYER_IDX_MONSTER].clear();
			}

			// Note: we use the grounded boolean member to check 
			// whether the player has collided with the commander

			break;
		}
	}
}

void checkCollision(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB, float deltaTime)
{
	for (const auto& colA : objA.getCollider())  // Loop through objA colliders
	{
		SDL_FRect rectA{
			.x = objA.getPosition().x + colA.x,
			.y = objA.getPosition().y + colA.y,
			.w = colA.w,
			.h = colA.h
		};

		for (const auto& colB : objB.getCollider())  // Loop through objB colliders
		{
			SDL_FRect rectB{
				.x = objB.getPosition().x + colB.x,
				.y = objB.getPosition().y + colB.y,
				.w = colB.w,
				.h = colB.h
			};

			SDL_FRect rectC{ 0 };
			if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
			{
				// Found intersection, respond
				collisionResponse(state, gs, res, objA, objB, rectA, rectB, rectC, deltaTime);
			}
		}
	}
}

void collisionResponse(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB,
	SDL_FRect rectA, SDL_FRect rectB, SDL_FRect rectC, float deltaTime)
{
	// Check the type of the object colliding
	if (objA.getType() == ObjectType::player)
	{
		// Cast the Player type
		Player& player = static_cast<Player&>(objA);

		switch (objB.getType())
		{
			case ObjectType::level:
			{
				// The position and velocity of the player
				glm::vec2 pos = player.getPosition();    // Position
				glm::vec2 vel = player.getVelocity();    // Velocity

				// Horizontal collision
				if (rectC.w <= rectC.h)
				{
					if (player.getVelocity().x > 0)
					{
						pos.x -= rectC.w;    // Add to the left
					}

					if (player.getVelocity().x < 0)
					{
						pos.x += rectC.w;    // Add to the right
					}

					vel.x = 0;
				}

				// Vertical collision
				if (rectC.w > rectC.h)
				{
					if (player.getVelocity().y > 0)
					{
						pos.y -= rectC.h;    // Going down
					}
					else if (player.getVelocity().y < 0)
					{
						pos.y += rectC.h;    // Going up
					}

					vel.y = 0;  // Stop object from moving further

					if (PLAYING)
					{
						if (player.getState() == PlayerState::jumping)
						{
							player.setState(PlayerState::running);
						}
					}
				}

				// Set the new position and velocity
				player.setPosition(pos);
				player.setVelocity(vel);

				break;
			}

			case ObjectType::monster:
			{
				// Set player state to caught if the player is playing 
				if (player.isAlive())
				{

					cout << "Player Caught!!!" << endl;

					player.setState(PlayerState::caught);
					player.setAlive(false);
					PLAYING = false;
				}

				Monster& monster = static_cast<Monster&>(objB);

				// Set monster state to killing
				monster.setState(MonsterState::killing);

				break;
			}

			case ObjectType::obstacle:
			{
				if (gs.invincibleMode)
				{
					break;
				}

				// Cast the GameObject to Obstacle type
				Obstacle& obs = static_cast<Obstacle&>(objB);

				switch (obs.getObstacleType())
				{
					case ObstacleType::wall:
					{
						// The position and velocity of the player
						glm::vec2 pos = player.getPosition();    // Position
						glm::vec2 vel = player.getVelocity();    // Velocity

						// Horizontal collision
						if (rectC.w <= rectC.h)
						{
							if (player.getVelocity().x != 0)
							{
								pos.x -= rectC.w;  // Add to the left
							}

							if (player.getVelocity().x < 0)
							{
								pos.x += rectC.w;  // Add to the right
							}

							vel.x = 0;

							player.setState(PlayerState::knocked);
							PLAYING = false;
						}

						// Vertical collision
						if (rectC.w > rectC.h)
						{
							if (player.getVelocity().y > 0)
							{
								pos.y -= rectC.h;  // Going down
							}
							else if (player.getVelocity().y < 0)
							{
								pos.y += rectC.h;  // Going up
							}

							vel.y = 0;  // Stop object from moving further

							if (PLAYING)
							{
								if (player.getState() == PlayerState::jumping)
								{
									player.setState(PlayerState::running);
								}
							}
						}

						// Set the new position and velocity
						player.setPosition(pos);
						player.setVelocity(vel);

						break;
					}

					case ObstacleType::spike:
					{
						player.setState(PlayerState::bleed);
						PLAYING = false;

						break;
					}

					case ObstacleType::burnt:
					{
						player.setState(PlayerState::burnt);
						PLAYING = false;

						break;
					}

					case ObstacleType::tripped:
					{
						if (player.getState() == PlayerState::caught)
						{
							break;
						}
						// The position and velocity of the player
						glm::vec2 pos = player.getPosition();    // Position
						glm::vec2 vel = player.getVelocity();    // Velocity

						// Horizontal collision
						if (rectC.w <= rectC.h)
						{
							player.setState(PlayerState::tripped);
						}

						// Vertical collision
						if (rectC.w > rectC.h)
						{
							if (player.getVelocity().y > 0)
							{
								pos.y -= rectC.h;  // Going down
							}
							else if (player.getVelocity().y < 0)
							{
								pos.y += rectC.h;  // Going up
							}

							vel.y = 0;  // Stop object from moving further
						}

						// Set the new position and velocity
						player.setPosition(pos);
						player.setVelocity(vel);

						break;
					}
				}

				break;
			}
			// End portal
			case ObjectType::endportal:
			{
				// Debug
				//cout << "Reached Portal" << endl;

				// Set the needTransition flag to true
				gs.needTransition = true;

				break;
			}
			// Commander
			case ObjectType::commander:
			{
				// Debug
				//cout << "Reached the Commander" << endl;

				// Set player state to won
				player.setState(PlayerState::won);

				// Ste the texture and animation to the idle one
				player.setTexture(res.playerIdle);
				player.setCurrentAnimation(res.ANIM_PLAYER_IDLE);
				
				// Reverse the animation of the commander
				Level& commander = static_cast<Level&>(objB);

				// Set the commander to be grounded, meaning that 
				// the player has collided with the commander
				commander.setGrounded(true);

				break;
			}
		}
	}
	else if (objA.getType() == ObjectType::monster)
	{

	}
}

void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown)
{
	// Exit if the object is not of player type 
	if (obj.getType() != ObjectType::player)
	{
		return;
	}

	Player& player = static_cast<Player&>(obj);

	// Check which keys are being pressed only if the game is running 
	// or if the player is in idle state (game not started)
	if (!keyDown)
	{
		return;
	}

	const float JUMP_FORCE = -320.0f;

	switch (player.getState())
	{
		case PlayerState::idle:
		{
			if (key == SDL_SCANCODE_D || key == SDL_SCANCODE_RIGHT)
			{
				player.setState(PlayerState::running);
			}

			break;
		}

		case PlayerState::running:
		{
			if (key == SDL_SCANCODE_W || key == SDL_SCANCODE_UP || key == SDL_SCANCODE_LSHIFT)
			{
				player.setJumpBufferTime(player.getJumpBufferDuration());
			}

			if (key == SDL_SCANCODE_S || key == SDL_SCANCODE_DOWN || key == SDL_SCANCODE_LCTRL)
			{
				player.setSlideRequested(true);
			}

			break;
		}

		case PlayerState::jumping:
		{
			if (key == SDL_SCANCODE_S || key == SDL_SCANCODE_DOWN || key == SDL_SCANCODE_LCTRL)
			{
				player.setSlideRequested(true);
			}

			break;
		}

		case PlayerState::sliding:
		{
			if (key == SDL_SCANCODE_W || key == SDL_SCANCODE_UP || key == SDL_SCANCODE_LSHIFT)
			{
				player.setJumpBufferTime(player.getJumpBufferDuration());
			}

			break;
		}
	}
}

bool checkGrounded(const GameObject& player, const std::vector<std::unique_ptr<GameObject>>& levelTiles)
{
	SDL_FRect playerCollider = player.getCollider().at(0);

	float playerLeft = player.getPosition().x + playerCollider.x;
	float playerRight = playerLeft + playerCollider.w;
	float playerBottom = player.getPosition().y + playerCollider.y + playerCollider.h;

	// Small tolerance for float comparisons
	const float tolerance = 5.0f;

	for (const auto& tile : levelTiles)
	{
		for (int i = 0; i < tile->getCollider().size(); i++)
		{
			SDL_FRect tileRect{
				tile->getPosition().x + tile->getCollider().at(i).x,
				tile->getPosition().y + tile->getCollider().at(i).y,
				tile->getCollider().at(i).w,
				tile->getCollider().at(i).h
			};

			float tileLeft = tileRect.x;
			float tileRight = tileRect.x + tileRect.w;
			float tileTop = tileRect.y;

			// Check horizontal overlap
			bool overlapX = (playerRight > tileLeft) && (playerLeft < tileRight);

			// Check if player bottom is within tolerance of tile top
			bool onTop = (playerBottom >= tileTop - tolerance && playerBottom <= tileTop + tolerance);

			if (overlapX && onTop)
			{
				return true;
			}
		}
	}

	return false;
}

void manageTiles(const SDLState& state, GameState& gs, Resources& res, bool isUpdate)
{
	int startCol = CURRENT_MAP_SIZE;

	// If map width hasn't changed, nothing to do
	if (CURRENT_MAP_SIZE == gs.gameMap.at(0).size())
	{
		return;
	}

	// Check whether a specific tile (row, col) already has something spawned there
	auto tileAlreadySpawned = [&](int row, int col) -> bool
		{
			float expectedX = static_cast<float>(col * TILE_SIZE);
			float expectedY = state.logH - (MAP_ROWS - row - 1) * TILE_SIZE;

			for (auto& layer : gs.layers)
			{
				for (auto& obj : layer)
				{
					auto pos = obj->getPosition();
					if (std::round(pos.x) == std::round(expectedX) &&
						std::round(pos.y) == std::round(expectedY))
					{
						return true; // exact tile already has an object
					}
				}
			}

			return false;
		};

	// For each row
	for (int r = 0; r < static_cast<int>(gs.gameMap.size()); ++r)
	{
		// For each element of the new column range
		for (int c = startCol; c < static_cast<int>(gs.gameMap.at(r).size()); ++c)
		{
			// Check to see if this column is already spawned
			if (c < CURRENT_MAP_SIZE && c != 0)
			{
				continue;
			}

			// If this is the last row
			if (r == static_cast<int>(gs.gameMap.size()) - 1)
			{
				// Increment the map size
				CURRENT_MAP_SIZE++;
				
				// Debug: print the columns that are loaded
				//cout << "Loaded Column: " << CURRENT_MAP_SIZE - 1 << endl;
			}

			int id = gs.gameMap.at(r).at(c);
			switch (id)
			{
				// Player (spawn only if no player exists)
				case PLAYER_INDEX:
				{
					if (gs.layers[LAYER_IDX_PLAYER].empty())
					{
						auto player = std::make_unique<Player>(res);
						player->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_PLAYER].push_back(std::move(player));
					}

					break;
				}

				// Monster
				case MONSTER_INDEX:
				{
					auto monster = std::make_unique<Monster>(res);
					monster->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
					gs.layers[LAYER_IDX_MONSTER].push_back(std::move(monster));
					std::cout << "Spawned monster at row " << r << " col " << c
						<< " pos (" << c * TILE_SIZE << ", " << state.logH - (MAP_ROWS - r - 1) * TILE_SIZE << ")\n";

					break;
				}

				// Second player
				case SECOND_PLAYER_INDEX:
				{
					break;
				}

				// Starting portal
				case START_PORTAL_INDEX:
				{
					// Debug
					/*cout << "Spawned starting portal!" << endl;
					cout << "Ptr: " << res.startPortal << endl;*/

					auto portal = std::make_unique<GameObject>();

					portal->setType(ObjectType::startportal);      // or endportal
					portal->setImageSize({ START_PORTAL_SIZE, START_PORTAL_SIZE });
					portal->setTexture(res.startPortal);           // texture from Resources
					portal->setAnimations(res.startPortalAnim);     // add animation
					portal->setCurrentAnimation(0);                // start at frame 0
					portal->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
					portal->clearCollider();
					portal->addCollider(START_PORTAL_COLLISION);

					gs.layers[LAYER_IDX_LEVEL].push_back(std::move(portal));

					break;
				}

				// Ending portal
				case END_PORTAL_INDEX:
				{
					auto portal = std::make_unique<GameObject>();

					portal->setType(ObjectType::endportal);      // or endportal
					portal->setImageSize({ END_PORTAL_SIZE, END_PORTAL_SIZE });
					portal->setTexture(res.endPortal);           // texture from Resources
					portal->setAnimations(res.endPortalAnim);     // add animation
					portal->setCurrentAnimation(0);                // start at frame 0
					portal->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
					portal->clearCollider();
					portal->addCollider(END_PORTAL_COLLISION);

					gs.layers[LAYER_IDX_LEVEL].push_back(std::move(portal));

					break;
				}

				// Commander
				case COMMANDER_INDEX:
				{
					auto commander = std::make_unique<GameObject>();

					commander->setType(ObjectType::commander);
					commander->setImageSize({ COMMANDER_WIDTH, COMMANDER_HEIGHT });
					commander->setTexture(res.commander);
					commander->setAnimations(res.commanderAnim);
					commander->setCurrentAnimation(0);
					commander->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
					commander->clearCollider();
					commander->addCollider(COMMANDER_COLLISION);

					gs.layers[LAYER_IDX_LEVEL].push_back(std::move(commander));

					break;
				}

				default:
				{
					// Floor Tiles
					if (id >= 6 && id <= 50)
					{
						auto& floor = gs.currentBiome.floor.at(id);
						auto obs = std::make_unique<Level>(floor);
						obs->setTexture(floor.getTexture());
						/*SDL_Texture* tex = res.getTileTexture(state.renderer, gs.currentBiome.name, id);
						obs->setTexture(tex);*/

						obs->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_LEVEL].push_back(std::move(obs));
						FLOOR_TILES++;
						break;
					}

					// Tripped obstacles
					else if (id >= 51 && id <= 100)
					{
						auto& tripped = gs.currentBiome.tripped.at(id);
						auto obs = std::make_unique<Obstacle>(tripped);
						obs->setTexture(tripped.getTexture());
						/*SDL_Texture* tex = res.getTileTexture(state.renderer, gs.currentBiome.name, id);
						obs->setTexture(tex);*/

						obs->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_LEVEL].push_back(std::move(obs));
						OBSTACLES++;
						break;
					}

					// Wall obstacles
					else if (id >= 101 && id <= 150)
					{
						auto& wall = gs.currentBiome.wall.at(id);
						auto obs = std::make_unique<Obstacle>(wall);
						//obs->setTexture(wall.getTexture());
						SDL_Texture* tex = res.getTileTexture(state.renderer, gs.currentBiome.name, id);
						obs->setTexture(tex);

						obs->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_LEVEL].push_back(std::move(obs));
						OBSTACLES++;
						break;
					}

					// Burnt obstacles
					else if (id >= 151 && id <= 200)
					{
						auto& burnt = gs.currentBiome.burnt.at(id);
						auto obs = std::make_unique<Obstacle>(burnt);
						obs->setTexture(burnt.getTexture());
						/*SDL_Texture* tex = res.getTileTexture(state.renderer, gs.currentBiome.name, id);
						obs->setTexture(tex);*/

						obs->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_LEVEL].push_back(std::move(obs));
						OBSTACLES++;
						break;
					}

					// Spike obstacles
					else if (id >= 201 && id <= 250)
					{
						auto& spike = gs.currentBiome.spike.at(id);
						auto obs = std::make_unique<Obstacle>(spike);
						obs->setTexture(spike.getTexture());
						/*SDL_Texture* tex = res.getTileTexture(state.renderer, gs.currentBiome.name, id);
						obs->setTexture(tex);*/

						obs->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_LEVEL].push_back(std::move(obs));
						OBSTACLES++;
						break;
					}

					// Others
					else
					{
						break;
					}
				}
			}
		}
	}
}

void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime)
{
	// If the texture is null
	if (!texture)
	{
		// We exit
		return;
	}

	// Get texture size safely
	float texW, texH;
	SDL_GetTextureSize(texture, &texW, &texH);

	// Scroll background relative to player velocity and scroll factor (parallax speed)
	scrollPos -= xVelocity * scrollFactor * deltaTime;

	// Loop scrolling by resetting position once fully scrolled off screen
	if (scrollPos <= -texW)
	{
		scrollPos = 0;
	}

	// Destination rect for rendering tiled background
	SDL_FRect dst
	{
		.x = scrollPos,
		.y = 0, // Fixed Y offset for the background layer
		.w = texW * 2.0f, // Draw double width for seamless tiling
		.h = static_cast<float>(texH)
	};

	// Render tiled texture to fill the horizontal area smoothly
	SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}