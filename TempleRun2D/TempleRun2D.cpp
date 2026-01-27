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
float ORIGINAL_SPEED = 125.0f;
string CREDITS_FILE = "data/credits.txt";
glm::vec2 GAME_TITLE_POSITION = { 0, 0 };
glm::vec2 CHARACTER_NAME_POSITION = { 70, 16 };
glm::vec2 CHARACTER_NAME_POSITION_CHANGE = { 222, 68 };
SDL_FRect CREDITS_BOX = { 320.0f, 80.0f, 480.0f, 200.0f };

// All textures (Biomes, Characters, Monsters)
unordered_map<string, Biome> biomeTexturesMap;
unordered_map<string, PlayerTextures> playerTexturesMap;

// Main function
int main(int argc, char* argv[])
{
	// Create an SDLState object
	SDLState sdl;

	if (!sdl.initialize())
	{
		return 1;
	}

	if (!TTF_Init())
	{
		SDL_Log("TTF_Init failed: %s", SDL_GetError());
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
	res.load(sdl, "default_monster", game.currentBiome->name, game.currentBiome->parallaxBackgrounds);          // Load our player and monster

	// Create the game title
	game.title = std::make_unique<UILabel>(sdl.renderer, "assets/fonts/ArcadeIn.ttf", 64);
	game.title->setText("Biome Dash");
	game.title->setColor(160, 64, 255);    // Purple text
	game.titleShadow = std::make_unique<UILabel>(sdl.renderer, "assets/fonts/ArcadeOut.ttf", 64);
	game.titleShadow->setText("Biome Dash");
	game.titleShadow->setColor(0, 0, 0);    // Black text

	// Create the character's name
	game.characterName = std::make_unique<UILabel>(sdl.renderer, "assets/fonts/VirtupetPixies.ttf", 32);
	string name = game.fullCharactersNickNames.at(game.currentCharacter);
	replace(name.begin(), name.end(), '_', ' ');
	game.characterName->setText(name);
	game.characterName->setColor(255, 255, 255);    // White text
	game.characterNamePos = CHARACTER_NAME_POSITION;

	// Set up the UI and the Transition
	game.ui.setup(sdl.renderer, res);
	game.ui.render(sdl);
	game.transition = Transition(res, sdl);

	// Load the home screen
	game.ui.switchToHomeScreen();

	// ----- PRELOAD THE CHARACTERES TEXTURES -----
	// Iterate through all the characters and load the textures
	for (string name : game.fullCharactersList)
	{
		// Debug
		cout << " --- Loading character: " << name << endl;

		// Create a temporary PlayerTextures object
		PlayerTextures playerTex;

		// Load the animations and textures
		playerTex.loadTextures(sdl.renderer, name, res);

		// Add the Player textures into the map
		playerTexturesMap[name] = playerTex;
	}

	// Switch to the default character's textures
	switchCharacterTextures(game, playerTexturesMap[game.defaultCharacterName], res);
	
	// ----- PRELOAD THE BIOMES -----
	// Iterate through all the biomes and load the textures
	for (std::string name : game.fullBiomeList)
	{
		// Create a temporary Biome object
		Biome temp;
		temp.name = name;

		// Debug
		//cout << "Preloading " << name << endl;

		// Read and load all the textures
		temp.loadBiome(name);          // parse the text file
		temp.loadTextures(res, sdl.renderer); // actually load images to GPU
		
		// Debug
		//cout << "Creating the objects..." << endl;

		// Insert the biome to the map
		biomeTexturesMap[temp.name] = move(temp);
	}
	
	// Set a new name for the biome
	game.currentBiome = &biomeTexturesMap["Transition"];

	// ----- PRELOAD ALL THE CREDITS -----
	game.loadCredits(sdl, CREDITS_FILE);

	// Set the credits box
	game.creditRect = CREDITS_BOX;

	// Start the map size at 0
	CURRENT_MAP_SIZE = 0;

	// Create the game tiles
	manageTiles(sdl, game, res, false);

	// Set the title world position
	GAME_TITLE_POSITION = { game.player().getPosition().x - 260, game.player().getPosition().y / 3.5 };
	game.titleWorldPos = GAME_TITLE_POSITION;

	// Set the player speed based on the difficulty
	ORIGINAL_SPEED = game.getOriginalSpeed();

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
		// Debug
		//cout << "Game Screen State: " << static_cast<int>(game.screen) << endl;

		// Increment the frame count
		frames++;
		uint64_t fpsNow = SDL_GetTicks();
		if (fpsNow - fpsLastTime >= 1000)
		{
			fps = frames * 1000.0f / (fpsNow - fpsLastTime);
			frames = 0;
			fpsLastTime = fpsNow;
		}

		// Check the screen state
		switch (game.screen)
		{
			case ScreenState::home:
			{
				runHomeScreen(sdl, game, res, scrollPositions, prevTime); // update currentScreen if user starts game
				break;
			}

			case ScreenState::playing:
			{
				runPlayingFrame(sdl, game, res, scrollPositions, prevTime);
				break;
			}

			case ScreenState::homePause:
			{
				runHomePauseScreen(sdl, game, res, scrollPositions, prevTime); // update currentScreen if user resumes or quits
				break;
			}

			case ScreenState::playingPause:
			{
				runPlayingPauseScreen(sdl, game, res, scrollPositions, prevTime); // update currentScreen if user resumes or quits
				break;
			}

			case ScreenState::gameOver:
			{
				runGameOverScreen(sdl, game, res, scrollPositions, prevTime); // update currentScreen if user goes to home or restarts
				break;
			}

			case ScreenState::transition:
			{
				runTransitionScreen(sdl, game, res, scrollPositions, prevTime);
				break;
			}

			case ScreenState::changePlayer:
			{
				runChangePlayerScreen(sdl, game, res, scrollPositions, prevTime);
				break;
			}

			case ScreenState::won:
			{
				runWinningScreen(sdl, game, res, scrollPositions, prevTime);
				break;
			}

			case ScreenState::credits:
			{
				runCreditsScreen(sdl, game, res, scrollPositions, prevTime);
				break;
			}

			case ScreenState::exit:
			{
				running = false;
				break;
			}
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

		// Render the frames
		SDL_RenderPresent(sdl.renderer);
	}

	// Clean up and destroy all the memories and resources used 
	res.unload();
	sdl.cleanup();

	return 0;
}