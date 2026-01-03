#pragma once

#include <SDL3/SDL.h>

#include "entities/UIButton.h"

// Constants
const glm::vec2 PAUSE_SIZE({ 84, 84 });
const glm::vec2 PAUSE_MARGIN({ 0.975, 0.05 });

// UIState struct
struct UIState
{
	// Main buttons
	//UIButton play;
	UIButton pause;

	// Buttons in the Pause panel
	//UIButton exit;
	//UIButton home;
	//UIButton continueGame;

	// Top left avatar
	//UIButton profile;
	//UIButton changeName;

	// Select button
	//UIButton select;

	// Default constructor
	UIState()
	{
		// Play button
		//play.setPosition({ 0, 0 });
		//play.setSize({ 100, 50 });

		// Pause button
		//pause.setPosition({ 120, 0 });
		//pause.setSize({ 100, 50 });
	}

	// Member function
	void setup(SDL_Renderer* renderer, Resources& res)
	{
		//play = UIButton(renderer, res, "play", { 100, 100 }, { 200, 80 });
		pause = UIButton(renderer, res, "pause", PAUSE_SIZE, PAUSE_MARGIN);
	}

	void render(const SDLState& state) const
	{
		// Debug
		//std::cout << "Reached the render function in UIState..." << std::endl;

		// Render all the buttons
		pause.render(state);
	}
};