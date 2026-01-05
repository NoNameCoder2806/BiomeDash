#pragma once

#include <SDL3/SDL.h>

#include "entities/UIButton.h"
#include "GameState.h"

// Constants
// PLAY BUTTON
const glm::vec2 PLAY_SIZE({ 260, 80 });
const glm::vec2 PLAY_MARGIN({ 0.5, 0.65 });

// PAUSE BUTTON
const glm::vec2 PAUSE_SIZE({ 84, 84 });
const glm::vec2 PAUSE_MARGIN({ 0.975, 0.05 });

// PAUSE PANEL
const glm::vec2 PAUSE_PANEL_SIZE({ 672, 504 });
const glm::vec2 PAUSE_PANEL_MARGIN({ 0.5, 0.55 });

// UIState struct
struct UIState
{
	// Main buttons
	UIButton play;
	UIButton pause;

	// Pause panel
	UIButton pausePanel;

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
		// Play and Pause buttons
		play = UIButton(renderer, res, "play", PLAY_SIZE, PLAY_MARGIN);
		pause = UIButton(renderer, res, "pause", PAUSE_SIZE, PAUSE_MARGIN);
		
		// Pause pannel
		pausePanel = UIButton(renderer, res, "pausePanel", PAUSE_PANEL_SIZE, PAUSE_PANEL_MARGIN);
	}

	void render(const SDLState& state) const
	{
		// Debug
		//std::cout << "Reached the render function in UIState..." << std::endl;

		// Render all the buttons
		// Play and Pause buttons
		play.render(state);
		pause.render(state);

		// Pause panel
		pausePanel.render(state);
	}

	void updateButtons(float mouseX, float mouseY, bool clicked, SDLState sdl)
	{
		// Check each buttons
		play.updateClicked(mouseX, mouseY, clicked, sdl);
		pause.updateClicked(mouseX, mouseY, clicked, sdl);
	}

	void switchToHomeScreen()
	{
		// Visible Buttons
		// ----- PLAY BUTTON -----
		play.setVisible(true);

		// ----- PAUSE BUTTON -----
		pause.setVisible(true);

		// Buttons that are NOT Visible
		// ----- PAUSE PANEL -----
		pausePanel.setVisible(false);
	}

	void switchToPlayScreen()
	{
		// Visible Buttons
		// ----- PAUSE BUTTON -----
		pause.setVisible(true);

		// Buttons that are NOT Visible
		// ----- PLAY BUTTON -----
		play.setVisible(false);

		// ----- PAUSE PANNEL -----
		pausePanel.setVisible(false);
	}

	void switchToHomePauseScreen()
	{
		// Visible Buttons
		// ----- PAUSE PANEL -----
		pausePanel.setVisible(true);

		// Buttons that are NOT Visible
		// ----- PLAY BUTTON -----
		play.setVisible(false);

		// Mark toggled buttons
		// ----- PAUSE BUTTON -----
		pause.setToggled(true);
	}
};