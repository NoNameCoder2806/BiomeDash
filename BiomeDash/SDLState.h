#pragma once

#include <SDL3/SDL.h>

#include <format>
#include <string>

struct SDLState
{
	SDL_Window* window = nullptr;      // Window 
	int width = 640;                   // Width of screen
	int height = 360;                  // Height of screen
	int logW = 544;                    // Logical width
	int logH = 306;                    // Logical height

	SDL_Renderer* renderer = nullptr;  // Renderer
	
	const bool* keys = nullptr;       // Keyboard state array

	bool fullscreen;

	bool running = true;               // Game loop flag

	Uint64 lastTick = 0;               // Time of last frame
	float deltaTime = 0.0f;            // Time between frames

	SDLState()
	{
		keys = SDL_GetKeyboardState(nullptr);
		fullscreen = false;
	}

	bool initialize()
	{
		// Initializes the SDL library for video
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
			return false;
		}

		// Setup window
		window = SDL_CreateWindow("TempleRun2D", width, height, SDL_WINDOW_RESIZABLE);

		// Check if the window was created
		if (!window)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
			cleanup();
			return false;
		}

		// Create a renderer
		renderer = SDL_CreateRenderer(window, nullptr);
		SDL_SetRenderVSync(renderer, true);
		if (!renderer)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", window);
			cleanup();
			return false;
		}

		// Configure presentation
		SDL_SetRenderLogicalPresentation(renderer, logW, logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

		return true;
	}

	void cleanup()
	{
		// Clean up and destroy all the memories and resources used 
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};