// TempleRun2D.cpp : Defines the entry point for the application.
//

// Header files
#include "TempleRun2D.h"
#include <SDL3/SDL_main.h>

using namespace std;

// External variables
int TOTAL_TILE = 0;
int OBSTACLES = 0;
int FLOOR_TILES = 0;
bool PLAYING = false;
bool BIOME_UPDATE = true;
int CURRENT_MAP_SIZE = 0;

// All biomes' textures
unordered_map<string, Biome> biomeTexturesMap;

// Function prototypes
//void resetForNewBiome(SDLState& state, GameState& gs, Resources& res);

//void countObjectsWithTexture(const GameState& game);
/*
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
*/
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
	game.updateBiome(game.currentBiome->name);
	Resources res;          // Create a Resources object
	res.load(sdl, "default_character", "default_monster", game.currentBiome->name, game.currentBiome->parallaxBackgrounds);          // Load our player and monster

	// Preload the biomes
	// Load the Transition biome
	game.currentBiome->name = "Transition";

	// Read and load all the textures
	game.currentBiome->loadBiome(game.currentBiome->name);          // parse the text file
	game.currentBiome->loadTextures(res, sdl.renderer); // actually load images to GPU

	// Iterate through all the biomes and load the textures
	for (std::string name : game.fullBiomeList)
	{
		// Create a temporary Biome object
		Biome temp;
		temp.name = name;

		// Debug 
		cout << "Preloading " << name << endl;

		// Read and load all the textures
		temp.loadBiome(name);          // parse the text file
		temp.loadTextures(res, sdl.renderer); // actually load images to GPU
		
		// Debug
		cout << "Creating the objects..." << endl;

		// Call manageTiles() to create the objects
		//manageTiles(sdl, game, res, false);

		// Clear the tiles
		//game.currentBiome->clearTextures();

		// Insert the biome to the map
		biomeTexturesMap[temp.name] = move(temp);

		// Debug
		/*auto& debugBiome = biomeTexturesMap[name];
		for (auto& [id, obj] : debugBiome.floor)
		{
			std::cout << "Biome " << name << " Floor Tile ID " << id
				<< " | Texture pointer: " << obj.getTexture() << std::endl;
		}*/
	}
	
	//game.preloadBiomes(res, sdl);

	// Set a new name for the biome
	game.currentBiome = &biomeTexturesMap["Transition"];

	// Load the current textures of the biome
	//game.updateBiome(game.currentBiome->name);
	//game.currentBiome->loadTextures(res, sdl.renderer);

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
		// Check the screen state
		switch (game.screen)
		{
			case ScreenState::home:
			{
				// Debug
				//cout << "Entered Home Screen" << endl;

				runHomeScreen(sdl, game.screen, game, res, scrollPositions, prevTime); // update currentScreen if user starts game
				break;
			}

			case ScreenState::playing:
			{
				runPlayingFrame(sdl, game, res, scrollPositions, prevTime, fps, frames, fpsLastTime, running);
				break;
			}

			case ScreenState::pause:
			{
				//runPauseScreen(sdl, game.screen); // update currentScreen if user resumes or quits
				break;
			}

			case ScreenState::gameOver:
			{
				//runGameOverScreen(sdl, game.screen); // update currentScreen if user goes to home or restarts
				break;
			}
		}
	}

	// Clean up and destroy all the memories and resources used 
	res.unload();
	sdl.cleanup();

	return 0;
}

// Function implementations
void resetForNewBiome(SDLState& state, GameState& gs, Resources& res)
{
	cout << "Reseting the resources and game state" << endl;
	// Set the world ready flag to false
	gs.worldReady = false;

	// Generate a new biome name
	string nextBiome;

	// If the current biome is a transition biome
	if (gs.currentBiome->name == "Transition")
	{
		// If there are more biomes to play
		if (gs.unusedBiomes.size() > 0)
		{
			// Then we need to generate one from the normal biomes
			int idx = gs.randomInt(0, gs.unusedBiomes.size() - 1);
			nextBiome = gs.unusedBiomes[idx];
			//nextBiome = "Pirate_Bay";
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
	//res.clearTiles();
	
	// Reset the biome
	gs.currentBiome = &biomeTexturesMap[nextBiome];
	gs.resetGameState(state, nextBiome);

	// Load the new biome textures
	//gs.currentBiome->loadTextures(res, state.renderer);

	// Reset the resources
	res.reset(state, gs.currentBiome->name, gs.currentBiome->parallaxBackgrounds);

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
	if (gs.currentBiome->name == "Transition")
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

	// Reset the flag back to true
	gs.worldReady = true;

	// Debug
	//cout << "Done creating tiles!" << endl;
}

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
	else if (obj.getType() == ObjectType::text)
	{
		float scrollFactor = 0.75f; // tweak this if you want it to scroll slightly slower/faster

		float xScreen = (obj.getPosition().x - gs.mapViewport.x) * scrollFactor;
		float yScreen = obj.getPosition().y - gs.mapViewport.y / 3.0f;

		// Set draw color to black (R,G,B,A)
		SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);

		// Render the black
		SDL_RenderDebugText(state.renderer, xScreen, yScreen, "Press > to start");
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
	const float tolerance = 2.0f;

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

			// Debug
			if (id != 0)
			{
				//cout << "Creating tile " << id << " for biome " << gs.currentBiome->name << endl;
			}

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

				// Starting text
				case START_TEXT_INDEX:
				{
					auto text = std::make_unique<GameObject>();

					text->setType(ObjectType::text);      // or endportal
					text->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
					text->clearCollider();
					text->addCollider({ 0, 0, 0, 0 });

					gs.layers[LAYER_IDX_LEVEL].push_back(std::move(text));

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
						auto& floor = gs.currentBiome->floor.at(id);
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
						auto& tripped = gs.currentBiome->tripped.at(id);
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
						auto& wall = gs.currentBiome->wall.at(id);
						auto obs = std::make_unique<Obstacle>(wall);
						obs->setTexture(wall.getTexture());
						/*SDL_Texture* tex = res.getTileTexture(state.renderer, gs.currentBiome->name, id);
						obs->setTexture(tex);*/

						obs->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						gs.layers[LAYER_IDX_LEVEL].push_back(std::move(obs));
						OBSTACLES++;
						break;
					}

					// Burnt obstacles
					else if (id >= 151 && id <= 200)
					{
						auto& burnt = gs.currentBiome->burnt.at(id);
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
						auto& spike = gs.currentBiome->spike.at(id);
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