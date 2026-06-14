#include "BiomeDash.h"

using namespace std;

// FUNCTION IMPLEMENTATIONS
// ----- I/ BIOME RESET -----
void resetForNewBiome(SDLState& state, GameState& gs, Resources& res)
{
	// Debug
	//cout << "Reseting the resources and game state" << endl;
	
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

			// Change the GameState to won
			gs.screen == ScreenState::won;

			// Change the UI to winning screen
			gs.ui.switchToWinningScreen();
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

// ----- II/ DRAW GAME OBJECTS -----
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
		float yScreen = obj.getPosition().y - gs.mapViewport.y / 4.0f;

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

// ----- III/ UPDATE GAME OBJECTS -----
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
				player.addCollider(DIED_COLLISION);
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
				player.clearCollider();
				player.addCollider(DIED_COLLISION);
				player.setAlive(false);

				PLAYING = false;

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

			case PlayerState::won:
			{
				player.setTexture(res.playerIdle);
				player.setCurrentAnimation(res.ANIM_PLAYER_IDLE);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				player.setAlive(true);

				break;
			}

			/*case PlayerState::speeding:
			{
				player.setTexture(res.playerSpeeding);
				player.setCurrentAnimation(res.ANIM_PLAYER_SPEEDING);
				player.clearCollider();
				player.addCollider(RUN_COLLISION);
				player.setAlive(true);

				break;
			}*/
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

// ----- IV/ HANDLE USER INPUT -----
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

// ----- V/ MANAGE TILES -----
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
						// Debug
						cout << "Creating a new player... " << endl;

						auto player = std::make_unique<Player>(res);
						player->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
						player->setState(PlayerState::idle);
						player->setCurrentAnimation(res.ANIM_PLAYER_IDLE);
						player->setName(gs.defaultCharacterName);
						player->setSpeed(ORIGINAL_SPEED);
						gs.layers[LAYER_IDX_PLAYER].push_back(std::move(player));
					}

					break;
				}

				// Monster
				case MONSTER_INDEX:
				{
					auto monster = std::make_unique<Monster>(res);
					monster->setPosition(glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r - 1) * TILE_SIZE));
					monster->setSpeed(ORIGINAL_SPEED);
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
				case GAME_TITLE_INDEX:
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

// ----- VI/ RESET GAME -----
void resetGame(SDLState& state, GameState& game, Resources& res, std::vector<float>& scrollPositions)
{
	// STOP gameplay immediately
	PLAYING = false;

	// Clear ALL game objects
	for (auto& layer : game.layers)
	{
		layer.clear(); // unique_ptr auto-deletes
	}

	// Reset the gameState
	// Reset viewport
	game.mapViewport = SDL_FRect{ 0, 0, (float)state.logW, (float)state.logH };

	// Reset scroll
	game.backgroundScroll = 0.f;
	scrollPositions.clear();
	scrollPositions.resize(res.parallaxBackgrounds.size(), 0.f);

	// Reset flags
	game.debugMode = false;
	game.invincibleMode = false;
	game.needTransition = false;
	game.portalGenerated = false;

	// Reset indices and counters
	game.loadedLeftCol = 0;
	game.loadedRightCol = game.gameMap.at(0).size() - 1;
	game.currentTile = 5;
	game.lastChunkEmpty = false;

	// Reset biome
	game.currentBiome = &biomeTexturesMap["Transition"];
	game.currentBiome->loadTextures(res, state.renderer); // optional

	// Reset the number of unused biomes
	game.unusedBiomes = game.biomeList;

	// Reset map
	game.gameMap = {
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

	// Start the map size at 0
	CURRENT_MAP_SIZE = 0;

	// Create the game tiles
	manageTiles(state, game, res, false);

	// Reset the backgrounds
	res.reset(state, game.currentBiome->name, game.currentBiome->parallaxBackgrounds);

	// Reset UI
	if (game.nextScreen == ScreenState::home)
	{
		game.ui.switchToHomeScreen();
	}
	else if (game.nextScreen == ScreenState::playing)
	{
		game.ui.switchToPlayScreen();
	}

	// Signal that the game no longer needs reseting
	game.needReset = false;
}

// ----- VII/ SWITCH CHARACTER TEXTURES -----
void switchCharacterTextures(GameState& gs, PlayerTextures& playerTex, Resources& res)
{
	playerTex.anims.resize(10);
	res.playerAnims.resize(10);

	res.playerAnims[res.ANIM_PLAYER_IDLE] = playerTex.anims[res.ANIM_PLAYER_IDLE];
	res.playerAnims[res.ANIM_PLAYER_RUN] = playerTex.anims[res.ANIM_PLAYER_RUN];
	res.playerAnims[res.ANIM_PLAYER_JUMP] = playerTex.anims[res.ANIM_PLAYER_JUMP];
	res.playerAnims[res.ANIM_PLAYER_SLIDE] = playerTex.anims[res.ANIM_PLAYER_SLIDE];
	res.playerAnims[res.ANIM_PLAYER_TRIPPED] = playerTex.anims[res.ANIM_PLAYER_TRIPPED];
	res.playerAnims[res.ANIM_PLAYER_KNOCKED] = playerTex.anims[res.ANIM_PLAYER_KNOCKED];
	res.playerAnims[res.ANIM_PLAYER_BURNT] = playerTex.anims[res.ANIM_PLAYER_BURNT];
	res.playerAnims[res.ANIM_PLAYER_BLEED] = playerTex.anims[res.ANIM_PLAYER_BLEED];
	res.playerAnims[res.ANIM_PLAYER_FALLING] = playerTex.anims[res.ANIM_PLAYER_FALLING];
	res.playerAnims[res.ANIM_PLAYER_CAUGHT] = playerTex.anims[res.ANIM_PLAYER_CAUGHT];
	//res.playerAnims[res.ANIM_PLAYER_SPEEDING] = playerTex.anims[res.ANIM_PLAYER_SPEEDING];

	res.playerIdle = playerTex.idle;
	res.playerRun = playerTex.run;
	res.playerJump = playerTex.jump;
	res.playerSlide = playerTex.slide;
	res.playerTripped = playerTex.tripped;
	res.playerKnocked = playerTex.knocked;
	res.playerBurnt = playerTex.burnt;
	res.playerBleed = playerTex.bleed;
	res.playerFalling = playerTex.falling;
	res.playerCaught = playerTex.caught;
	//res.playerSpeeding = playerTex.caught;

	res.avatar = playerTex.avatar;

	// Change the character avatar
	gs.ui.characterAvatar.setTexture(res.avatar);
	gs.ui.characterAvatar.setClickedTexture(res.avatar);
}