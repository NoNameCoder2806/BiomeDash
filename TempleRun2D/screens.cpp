#include "TempleRun2D.h"

using namespace std;

// FUNCTION IMPLEMENTATIONS
// ----- HOME SCREEN -----
void runHomeScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime)
{
	// Events update
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			exit(0);
		}
		else if (e.type == SDL_EVENT_KEY_UP)
		{
			switch (e.key.scancode)
			{
				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_D:
				{
					// Switch to the play screen
					game.ui.switchToPlayScreen();

					// Change the Screen State
					game.screen = ScreenState::playing;

					// Set the player to the running state
					game.player().setState(PlayerState::running);

					// Set monster to chasing state
					game.monster().setState(MonsterState::chasing);

					return;
				}
				case SDL_SCANCODE_C:
				{
					//game.changeCharacter();
					break;
				}
				case SDL_SCANCODE_M:
				{
					//game.changeMonster();
					break;
				}
				case SDL_SCANCODE_ESCAPE:
				{
					// Switch the UI to the home pause screen
					game.ui.switchToHomePauseScreen();

					// Change the screen state
					game.screen = ScreenState::homePause;

					break;
				}
				case SDL_SCANCODE_F11:
				{
					sdl.fullscreen = !sdl.fullscreen;
					SDL_SetWindowFullscreen(sdl.window, sdl.fullscreen);
					// Optionally reset window dimensions after fullscreen toggle
					if (!sdl.fullscreen)
					{
						sdl.width = 1920; // or your default width
						sdl.height = 1080; // or your default height
						SDL_SetWindowSize(sdl.window, sdl.width, sdl.height);
					}
					break;
				}
			}
		}
		else if (e.type == SDL_EVENT_WINDOW_RESIZED)
		{
			sdl.width = e.window.data1;
			sdl.height = e.window.data2;
		}
	}

	// UI Buttons logic update
	float mouseX;
	float mouseY;

	// Get the mouse state
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	// Whether the mouse was left-clicked
	bool leftPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

	// Update the UI Buttons
	game.ui.updateButtons(mouseX, mouseY, leftPressed, sdl);

	// Check which buttons were clicked and act immediately
	// Pause Button
	if (game.ui.pause.isReleased())
	{
		// Debug
		//cout << "Switching to pause screen" << endl;

		// Switch the UI to the home pause screen
		game.ui.switchToHomePauseScreen();

		// Change the screen state
		game.screen = ScreenState::homePause;
	}
	
	// Play Button
	if (game.ui.play.isReleased())
	{
		// Switch the UI to the play screen
		game.ui.switchToPlayScreen();

		// Change the screen state
		game.screen = ScreenState::playing;

		// Set the player to the running state
		game.player().setState(PlayerState::running);

		// Set monster to chasing state
		game.monster().setState(MonsterState::chasing);
	}

	// Compute deltaTime like runPlayingFrame
	uint64_t nowTime = SDL_GetTicks();
	float deltaTime = (float)(nowTime - prevTime) / 1000.0f;
	prevTime = nowTime;

	// Center viewport on player
	game.mapViewport.x = (game.player().getPosition().x + TILE_SIZE / 2) - game.mapViewport.w / 2;
	game.mapViewport.y = (game.player().getPosition().y + TILE_SIZE / 2) - game.mapViewport.h / 2;

	// Clear background
	SDL_SetRenderDrawColor(sdl.renderer, 128, 0, 128, 255);
	SDL_RenderClear(sdl.renderer);

	// Draw main background
	SDL_RenderTexture(sdl.renderer, res.background, nullptr, nullptr);

	// Draw parallax backgrounds
	int layerCount = std::stoi(game.currentBiome->parallaxBackgrounds);
	if (scrollPositions.size() != res.parallaxBackgrounds.size())
	{
		scrollPositions.resize(res.parallaxBackgrounds.size(), 0.0f);
	}

	// Debug
	//cout << "Layers: " << layerCount << endl;
	//cout << "Biome: " << game.currentBiome.name << endl;
	//cout << "Backgrounds: " << game.currentBiome.parallaxBackgrounds << endl;

	for (int i = 0; i < layerCount; i++)
	{
		float scrollFactor = 0.75f * (i + 1) / layerCount;
		drawParalaxBackground(sdl.renderer, res.parallaxBackgrounds[i],
			game.player().getVelocity().x, scrollPositions[i], scrollFactor, deltaTime);
	}

	// Draw all objects and update animations using deltaTime
	for (auto& layer : game.layers)
	{
		for (auto& objPtr : layer)
		{
			GameObject& obj = *objPtr;

			if (obj.getCurrentAnimation() != -1)
				obj.getAnimations().at(obj.getCurrentAnimation()).step(deltaTime);

			drawObject(sdl, game, obj, deltaTime);
		}
	}

	// Convert player world position to screen position
	float px = game.player().getPosition().x - game.mapViewport.x;
	float py = game.player().getPosition().y - game.mapViewport.y;

	// Draw text slightly above the player's head
	/*SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 255); // black text
	SDL_RenderDebugText(
		sdl.renderer,
		(int)(px - 50),   // center horizontally
		(int)(py + 60),   // above head
		"Press > to start"
	);*/

	// Draw menu options
	//SDL_RenderDebugText(sdl.renderer, 400, 500, "Press RIGHT or RETURN to start");
	//SDL_RenderDebugText(sdl.renderer, 400, 550, "Press C to change character");
	//SDL_RenderDebugText(sdl.renderer, 400, 600, "Press M to change monster");
	//SDL_RenderDebugText(sdl.renderer, 400, 650, "Press ESC to quit");
	//SDL_RenderDebugText(sdl.renderer, 400, 700, "Press F11 to toggle fullscreen");

	// Temporarily switch to “real pixels” for UI
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.width, sdl.height, SDL_LOGICAL_PRESENTATION_DISABLED);

	// Draw the buttons
	game.ui.render(sdl);

	// Restore logical presentation so everything else still uses your world scaling
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.logW, sdl.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

// ----- II/ PLAYING FRAME -----
void runPlayingFrame(SDLState& sdl, GameState& game, Resources& res,
	vector<float>& scrollPositions, uint64_t& prevTime,
	float& fps, int& frames, uint64_t& fpsLastTime, bool& running)
{
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

				// Escape key
				if (e.key.scancode == SDL_SCANCODE_ESCAPE)
				{
					// Switch the UI to the playing pause screen
					game.ui.switchToPlayingPauseScreen();

					// Change the screen state
					game.screen = ScreenState::playingPause;

					break;
				}

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

	// UI Buttons logic update
	float mouseX;
	float mouseY;

	// Get the mouse state
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	// Whether the mouse was left-clicked
	bool leftPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

	// Update the UI Buttons
	game.ui.updateButtons(mouseX, mouseY, leftPressed, sdl);

	// Check which buttons were clicked and act immediately
	// Pause Button
	if (game.ui.pause.isReleased())
	{
		// Debug
		//cout << "Switching to pause screen" << endl;

		// Switch the UI to the playing pause screen
		game.ui.switchToPlayingPauseScreen();

		// Change the screen state
		game.screen = ScreenState::playingPause;
	}

	// Check whether the game needs to transition / change biome
	if (game.needTransition)
	{
		// Call the function to reset all the textures and game state
		resetForNewBiome(sdl, game, res);

		// Reset the flag
		game.needTransition = false;

		// Skip this frame
		return;
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
	int layerCount = stoi(game.currentBiome->parallaxBackgrounds);

	// Check the number of scroll positions and resize if necessary
	if (scrollPositions.size() != res.parallaxBackgrounds.size())
	{
		scrollPositions.resize(res.parallaxBackgrounds.size());
	}

	// Debug
	/*cout << "Current size: " << res.parallaxBackgrounds.size() << endl;
	cout << "Layers: " << layerCount << endl;
	cout << "Current Biome: " << game.currentBiome->name << endl;
	cout << "Number of Layers: " << game.currentBiome->parallaxBackgrounds << endl;
	*/

	// Draw each parallax background
	for (int i = 0; i < layerCount; i++)
	{
		// Debug
		//cout << "+ Loading: " << i << endl;

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
	string fpsText = std::format("FPS: {:.1f}", fps);
	SDL_RenderDebugText(sdl.renderer, 5, 5, fpsText.c_str());

	// Display the game score and the number of biomes completed
	string scoreText = format("Score: {:.1f}", game.getScore());
	SDL_RenderDebugText(sdl.renderer, 165, 5, scoreText.c_str());

	// White color
	SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, 255);

	// Temporarily switch to “real pixels” for UI
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.width, sdl.height, SDL_LOGICAL_PRESENTATION_DISABLED);

	// Draw the buttons
	game.ui.render(sdl);

	// Restore logical presentation so everything else still uses your world scaling
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.logW, sdl.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	// Assign nowTime to prevTime to update the time
	prevTime = nowTime;
}

// ----- III/ HOME PAUSE SCREEN -----
void runHomePauseScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime)
{
	// Events update
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			exit(0);
		}
		else if (e.type == SDL_EVENT_KEY_UP)
		{
			switch (e.key.scancode)
			{
				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_D:
				{
					// Switch to the play screen
					game.ui.switchToPlayScreen();

					// Change the Screen State
					game.screen = ScreenState::playing;

					// Set the player to the running state
					game.player().setState(PlayerState::running);

					// Set monster to chasing state
					game.monster().setState(MonsterState::chasing);

					return;
				}
				case SDL_SCANCODE_C:
				{
					//game.changeCharacter();
					break;
				}
				case SDL_SCANCODE_M:
				{
					//game.changeMonster();
					break;
				}
				case SDL_SCANCODE_ESCAPE:
				{
					// Switch to the home screen
					game.ui.switchToHomeScreen();

					// Change the Screen State
					game.screen = ScreenState::home;

					break;
				}
				case SDL_SCANCODE_F11:
				{
					sdl.fullscreen = !sdl.fullscreen;
					SDL_SetWindowFullscreen(sdl.window, sdl.fullscreen);
					// Optionally reset window dimensions after fullscreen toggle
					if (!sdl.fullscreen)
					{
						sdl.width = 1920; // or your default width
						sdl.height = 1080; // or your default height
						SDL_SetWindowSize(sdl.window, sdl.width, sdl.height);
					}
					break;
				}
			}
		}
		else if (e.type == SDL_EVENT_WINDOW_RESIZED)
		{
			sdl.width = e.window.data1;
			sdl.height = e.window.data2;
		}
	}

	// UI Buttons logic update
	float mouseX;
	float mouseY;

	// Get the mouse state
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	// Whether the mouse was left-clicked
	bool leftPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

	// Update the UI Buttons
	game.ui.updateButtons(mouseX, mouseY, leftPressed, sdl);

	// Check which buttons were clicked and act immediately
	// Volume button
	if (game.ui.volume.isReleased())
	{
		// Debug
		cout << "Muted sound!" << endl;

		// Change the toggled state of the button
		bool t = !game.ui.volume.isToggled();
		game.ui.volume.setToggled(t);

		// Do something else here
	}

	// Credits button
	if (game.ui.credits.isReleased())
	{
		// Debug
		cout << "Credits pannel pops up" << endl;

		// Do something else here
	}

	// Continue button
	if (game.ui.continueGame.isReleased())
	{
		// Switch to the home screen
		game.ui.switchToHomeScreen();

		// Change the Screen State
		game.screen = ScreenState::home;
	}

	// Exit button
	if (game.ui.exit.isReleased())
	{
		// Debug
		//cout << "Exiting the game..." << endl;

		exit(0);
	}

	// Render frame
	uint64_t nowTime = SDL_GetTicks();
	float deltaTime = (float)(nowTime - prevTime) / 1000.0f;
	prevTime = nowTime;

	// Clear background
	SDL_SetRenderDrawColor(sdl.renderer, 128, 0, 128, 255);
	SDL_RenderClear(sdl.renderer);

	// Draw main background
	SDL_RenderTexture(sdl.renderer, res.background, nullptr, nullptr);

	// Draw parallax (optional: frozen scroll)
	int layerCount = std::stoi(game.currentBiome->parallaxBackgrounds);
	if (scrollPositions.size() != res.parallaxBackgrounds.size())
		scrollPositions.resize(res.parallaxBackgrounds.size(), 0.0f);

	for (int i = 0; i < layerCount; i++)
	{
		float scrollFactor = 0.75f * (i + 1) / layerCount;
		drawParalaxBackground(sdl.renderer, res.parallaxBackgrounds[i],
			0.0f, scrollPositions[i], scrollFactor, deltaTime); // keep deltaTime for animation
	}

	// Draw all objects and update animations
	for (auto& layer : game.layers)
	{
		for (auto& objPtr : layer)
		{
			GameObject& obj = *objPtr;

			// Ignore the animation step since this is a pause frame
			/*if (obj.getCurrentAnimation() != -1)
			{
				obj.getAnimations().at(obj.getCurrentAnimation()).step(deltaTime); // keep animations
			}*/

			drawObject(sdl, game, obj, deltaTime);
		}
	}

	// Draw UI
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.width, sdl.height, SDL_LOGICAL_PRESENTATION_DISABLED);
	game.ui.render(sdl);
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.logW, sdl.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

// ----- IV/ PLAYING PAUSE SCREEN -----
void runPlayingPauseScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime)
{
	// Events update
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			exit(0);
		}
		else if (e.type == SDL_EVENT_KEY_UP)
		{
			switch (e.key.scancode)
			{
				case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_D:
				{
					// Switch to the play screen
					game.ui.switchToPlayScreen();

					// Change the Screen State
					game.screen = ScreenState::playing;

					// Set the player to the running state
					game.player().setState(PlayerState::running);

					// Set monster to chasing state
					game.monster().setState(MonsterState::chasing);

					return;
				}
				case SDL_SCANCODE_C:
				{
					//game.changeCharacter();
					break;
				}
				case SDL_SCANCODE_M:
				{
					//game.changeMonster();
					break;
				}
				case SDL_SCANCODE_ESCAPE:
				{
					// Switch to the home screen
					game.ui.switchToPlayScreen();

					// Change the Screen State
					game.screen = ScreenState::playing;

					break;
				}
				case SDL_SCANCODE_F11:
				{
					sdl.fullscreen = !sdl.fullscreen;
					SDL_SetWindowFullscreen(sdl.window, sdl.fullscreen);
					// Optionally reset window dimensions after fullscreen toggle
					if (!sdl.fullscreen)
					{
						sdl.width = 1920; // or your default width
						sdl.height = 1080; // or your default height
						SDL_SetWindowSize(sdl.window, sdl.width, sdl.height);
					}
					break;
				}
			}
		}
		else if (e.type == SDL_EVENT_WINDOW_RESIZED)
		{
			sdl.width = e.window.data1;
			sdl.height = e.window.data2;
		}
	}

	// UI Buttons logic update
	float mouseX;
	float mouseY;

	// Get the mouse state
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	// Whether the mouse was left-clicked
	bool leftPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

	// Update the UI Buttons
	game.ui.updateButtons(mouseX, mouseY, leftPressed, sdl);

	// Check which buttons were clicked and act immediately
	// Volume button
	if (game.ui.volume.isReleased())
	{
		// Debug
		cout << "Muted sound!" << endl;

		// Change the toggled state of the button
		bool t = !game.ui.volume.isToggled();
		game.ui.volume.setToggled(t);

		// Do something else here
	}

	// Restart button
	if (game.ui.restart.isReleased())
	{
		// Reset the game
		resetGame(sdl, game, res, scrollPositions);

		// Start the game immidiately
		// Switch the UI to the play screen
		game.ui.switchToPlayScreen();

		// Change the screen state
		game.screen = ScreenState::playing;

		// Set the player to the running state
		game.player().setState(PlayerState::running);

		// Set monster to chasing state
		game.monster().setState(MonsterState::chasing);
	}

	// Continue button
	if (game.ui.continueGame.isReleased())
	{
		// Switch to the home screen
		game.ui.switchToPlayScreen();

		// Change the Screen State
		game.screen = ScreenState::playing;
	}

	// Home button
	if (game.ui.home.isReleased())
	{
		// Debug
		cout << "Exiting to home..." << endl;

		// Reset the game
		//resetGame(sdl, game, res, scrollPositions);

		// Change the states for the screen
		game.prevScreen = ScreenState::playingPause;
		game.screen = ScreenState::transition;
		game.nextScreen = ScreenState::home;
	}

	// Render frame
	uint64_t nowTime = SDL_GetTicks();
	float deltaTime = (float)(nowTime - prevTime) / 1000.0f;
	prevTime = nowTime;

	// Clear background
	SDL_SetRenderDrawColor(sdl.renderer, 128, 0, 128, 255);
	SDL_RenderClear(sdl.renderer);

	// Draw main background
	SDL_RenderTexture(sdl.renderer, res.background, nullptr, nullptr);

	// Draw parallax (optional: frozen scroll)
	int layerCount = std::stoi(game.currentBiome->parallaxBackgrounds);
	if (scrollPositions.size() != res.parallaxBackgrounds.size())
		scrollPositions.resize(res.parallaxBackgrounds.size(), 0.0f);

	for (int i = 0; i < layerCount; i++)
	{
		float scrollFactor = 0.75f * (i + 1) / layerCount;
		drawParalaxBackground(sdl.renderer, res.parallaxBackgrounds[i],
			0.0f, scrollPositions[i], scrollFactor, deltaTime); // keep deltaTime for animation
	}

	// Draw all objects and update animations
	for (auto& layer : game.layers)
	{
		for (auto& objPtr : layer)
		{
			GameObject& obj = *objPtr;

			// Ignore the animation step since this is a pause frame
			/*if (obj.getCurrentAnimation() != -1)
			{
				obj.getAnimations().at(obj.getCurrentAnimation()).step(deltaTime); // keep animations
			}*/

			drawObject(sdl, game, obj, deltaTime);
		}
	}

	// Draw UI
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.width, sdl.height, SDL_LOGICAL_PRESENTATION_DISABLED);
	game.ui.render(sdl);
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.logW, sdl.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

// ----- V/ GAME OVER SCREEN -----
void runGameOverScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime)
{
	// Events update
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			exit(0);
		}
		else if (e.type == SDL_EVENT_KEY_UP)
		{
			switch (e.key.scancode)
			{
				/*case SDL_SCANCODE_RETURN:
				case SDL_SCANCODE_RIGHT:
				case SDL_SCANCODE_D:
				{
					// Switch to the play screen
					game.ui.switchToPlayScreen();

					// Change the Screen State
					game.screen = ScreenState::playing;

					// Set the player to the running state
					game.player().setState(PlayerState::running);

					// Set monster to chasing state
					game.monster().setState(MonsterState::chasing);

					return;
				}*/
				case SDL_SCANCODE_C:
				{
					//game.changeCharacter();
					break;
				}
				case SDL_SCANCODE_M:
				{
					//game.changeMonster();
					break;
				}
				/*case SDL_SCANCODE_ESCAPE:
				{
					// Switch to the home screen
					game.ui.switchToPlayScreen();

					// Change the Screen State
					game.screen = ScreenState::playing;

					break;
				}*/
				case SDL_SCANCODE_F11:
				{
					sdl.fullscreen = !sdl.fullscreen;
					SDL_SetWindowFullscreen(sdl.window, sdl.fullscreen);
					// Optionally reset window dimensions after fullscreen toggle
					if (!sdl.fullscreen)
					{
						sdl.width = 1920; // or your default width
						sdl.height = 1080; // or your default height
						SDL_SetWindowSize(sdl.window, sdl.width, sdl.height);
					}
					break;
				}
			}
		}
		else if (e.type == SDL_EVENT_WINDOW_RESIZED)
		{
			sdl.width = e.window.data1;
			sdl.height = e.window.data2;
		}
	}

	// UI Buttons logic update
	float mouseX;
	float mouseY;

	// Get the mouse state
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	// Whether the mouse was left-clicked
	bool leftPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);

	// Update the UI Buttons
	game.ui.updateButtons(mouseX, mouseY, leftPressed, sdl);

	// Check which buttons were clicked and act immediately
	// Volume button
	if (game.ui.volume.isReleased())
	{
		// Debug
		cout << "Muted sound!" << endl;

		// Change the toggled state of the button
		bool t = !game.ui.volume.isToggled();
		game.ui.volume.setToggled(t);

		// Do something else here
	}

	// Restart button
	if (game.ui.restart.isReleased())
	{
		// Reset the game
		resetGame(sdl, game, res, scrollPositions);

		// Start the game immidiately
		// Switch the UI to the play screen
		game.ui.switchToPlayScreen();

		// Change the screen state
		game.screen = ScreenState::playing;

		// Set the player to the running state
		game.player().setState(PlayerState::running);

		// Set monster to chasing state
		game.monster().setState(MonsterState::chasing);
	}

	// Main menu button
	if (game.ui.mainMenu.isReleased())
	{
		// Debug
		cout << "Back to main menu..." << endl;

		// Reset the game
		//resetGame(sdl, game, res, scrollPositions);

		// Change the states for the screen
		game.prevScreen = ScreenState::gameOver;
		game.screen = ScreenState::transition;
		game.nextScreen = ScreenState::home;
	}

	// Render frame
	uint64_t nowTime = SDL_GetTicks();
	float deltaTime = (float)(nowTime - prevTime) / 1000.0f;
	prevTime = nowTime;

	// Clear background
	SDL_SetRenderDrawColor(sdl.renderer, 128, 0, 128, 255);
	SDL_RenderClear(sdl.renderer);

	// Draw main background
	SDL_RenderTexture(sdl.renderer, res.background, nullptr, nullptr);

	// Draw parallax (optional: frozen scroll)
	int layerCount = std::stoi(game.currentBiome->parallaxBackgrounds);
	if (scrollPositions.size() != res.parallaxBackgrounds.size())
		scrollPositions.resize(res.parallaxBackgrounds.size(), 0.0f);

	for (int i = 0; i < layerCount; i++)
	{
		float scrollFactor = 0.75f * (i + 1) / layerCount;
		drawParalaxBackground(sdl.renderer, res.parallaxBackgrounds[i],
			0.0f, scrollPositions[i], scrollFactor, deltaTime); // keep deltaTime for animation
	}

	// Draw all objects and update animations
	for (auto& layer : game.layers)
	{
		for (auto& objPtr : layer)
		{
			GameObject& obj = *objPtr;

			// Ignore the animation step since this is a pause frame
			/*if (obj.getCurrentAnimation() != -1)
			{
				obj.getAnimations().at(obj.getCurrentAnimation()).step(deltaTime); // keep animations
			}*/

			drawObject(sdl, game, obj, deltaTime);
		}
	}

	// Draw UI
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.width, sdl.height, SDL_LOGICAL_PRESENTATION_DISABLED);
	game.ui.render(sdl);
	SDL_SetRenderLogicalPresentation(sdl.renderer, sdl.logW, sdl.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

// ----- TRANSITION SCREEN -----
void runTransitionScreen(SDLState& sdl, GameState& game, Resources& res, std::vector<float>& scrollPositions, uint64_t& prevTime)
{
	// Debug
	//std::cout << "Drawing the Transition screen. Timer: " << game.transition.getTime() << std::endl;

	// --- Compute deltaTime ---
	uint64_t nowTime = SDL_GetTicks();
	float deltaTime = (float)(nowTime - prevTime) / 1000.0f;
	prevTime = nowTime;

	// --- Step the transition timer ---
	game.transition.addTime(deltaTime);
	float t = game.transition.getTime();

	// Get current animation and texture
	Animation& anim = game.transition.getAnimations()[game.transition.getCurrentAnimation()];
	SDL_Texture* tex = game.transition.getTexture();

	// --- Phase 1: Closing animation (0s - 1s) ---
	if (t <= 1.0f)
	{
		if (game.transition.getCurrentAnimation() != res.ANIM_TRANSITION)
		{
			game.transition.setCurrentAnimation(res.ANIM_TRANSITION);
			game.transition.setTexture(res.transition);
			game.transition.getAnimations()[res.ANIM_TRANSITION].reset();
		}

		// Draw previous screen
		switch (game.prevScreen)
		{
			case ScreenState::gameOver:
			{
				runGameOverScreen(sdl, game, res, scrollPositions, prevTime);
				break;
			}
			case ScreenState::playingPause:
			{
				runPlayingPauseScreen(sdl, game, res, scrollPositions, prevTime);
				break;
			}
			default:
			{
				break;
			}
		}

		// Step animation
		game.transition.getAnimations()[game.transition.getCurrentAnimation()].step(deltaTime);

		// current frame of animation
		int frameIndex = game.transition.getAnimations()[game.transition.getCurrentAnimation()].currentFrame();

		SDL_FRect srcRect = {
			.x = frameIndex * (float)game.transition.getImageSize().x,
			.y = 0,
			.w = (float)game.transition.getImageSize().x,
			.h = (float)game.transition.getImageSize().y
		};

		// Full screen destination
		SDL_FRect dstRect = { 0, 0, (float)sdl.logW, (float)sdl.logH };

		SDL_RenderTexture(sdl.renderer, game.transition.getTexture(), &srcRect, &dstRect);
	}
	// --- Phase 2: Black pause (1s - 1.5s) ---
	else if (t <= 1.5f)
	{
		if (t >= 1.00001f && t <= 1.01f)
		{
			// Reset the game
			resetGame(sdl, game, res, scrollPositions);
		}

		SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 255);
		SDL_RenderClear(sdl.renderer);
	}
	// --- Phase 3: Opening animation (1.5s - 2.5s) ---
	else if (t <= 2.5f)
	{
		// Switch animation if needed
		if (game.transition.getCurrentAnimation() != res.ANIM_TRANSITION_REVERSED)
		{
			game.transition.setCurrentAnimation(res.ANIM_TRANSITION_REVERSED);
			game.transition.setTexture(res.transitionReversed);
			game.transition.getAnimations()[res.ANIM_TRANSITION_REVERSED].reset();
		}

		// Draw next screen
		if (game.nextScreen == ScreenState::home)
		{
			runHomeScreen(sdl, game, res, scrollPositions, prevTime);
		}

		// Step animation
		game.transition.getAnimations()[game.transition.getCurrentAnimation()].step(deltaTime);

		// current frame of animation
		int frameIndex = game.transition.getAnimations()[game.transition.getCurrentAnimation()].currentFrame();

		SDL_FRect srcRect = {
			.x = frameIndex * (float)game.transition.getImageSize().x,
			.y = 0,
			.w = (float)game.transition.getImageSize().x,
			.h = (float)game.transition.getImageSize().y
		};

		// Full screen destination
		SDL_FRect dstRect = { 0, 0, (float)sdl.logW, (float)sdl.logH };

		SDL_RenderTexture(sdl.renderer, game.transition.getTexture(), &srcRect, &dstRect);
	}
	// --- Phase 4: End of transition ---
	else
	{
		// Reset the timer
		game.transition.resetTimer();

		// Switch to next screen
		game.screen = game.nextScreen;
	}
}