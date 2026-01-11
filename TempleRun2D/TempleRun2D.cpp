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

// Main function
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

	// Set up the UI and the Transition
	game.ui.setup(sdl.renderer, res);
	game.ui.render(sdl);
	game.transition = Transition(res, sdl);

	// Load the home screen
	game.ui.switchToHomeScreen();

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
				// Debug
				//cout << "Entered Home Screen" << endl;

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