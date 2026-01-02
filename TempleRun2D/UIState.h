#pragma once

#include <SDL3/SDL.h>

#include "entities/UIButton.h"

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
		pause = UIButton(renderer, res, "pause", { 320, 100 }, { 200, 80 });
	}

	void render(SDL_Renderer* renderer, const SDL_FRect& viewport) const
	{
		// Debug
		std::cout << "Reached the render function in UIState..." << std::endl;
		
		pause.render(renderer, viewport);
	}
};