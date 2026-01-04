#pragma once

#include <SDL3/SDL.h>

#include "entities/UIButton.h"

// Constants
// PLAY BUTTON
const glm::vec2 PLAY_SIZE({ 260, 80 });
const glm::vec2 PLAY_MARGIN({ 0.5, 0.65 });

// PAUSE BUTTON
const glm::vec2 PAUSE_SIZE({ 84, 84 });
const glm::vec2 PAUSE_MARGIN({ 0.975, 0.05 });

// UIState struct
struct UIState
{
	// Main buttons
	UIButton play;
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
	}

	// Member function
	void setup(SDL_Renderer* renderer, Resources& res)
	{
		play = UIButton(renderer, res, "play", PLAY_SIZE, PLAY_MARGIN);
		pause = UIButton(renderer, res, "pause", PAUSE_SIZE, PAUSE_MARGIN);
	}

	void render(const SDLState& state) const
	{
		// Debug
		//std::cout << "Reached the render function in UIState..." << std::endl;

		// Render all the buttons
		play.render(state);
		pause.render(state);
	}

	void updateButtons(float mouseX, float mouseY, bool clicked, SDLState sdl)
	{
		// Check each buttons
		play.updateClicked(mouseX, mouseY, clicked, sdl);
		pause.updateClicked(mouseX, mouseY, clicked, sdl);
	}
};