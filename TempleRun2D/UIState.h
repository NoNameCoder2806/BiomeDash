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

// EXIT BUTTON
const glm::vec2 EXIT_SIZE({ 260, 80 });
const glm::vec2 EXIT_MARGIN({ 0.4075, 0.67 });

// HOME BUTTON

// CONTINUE BUTTON
const glm::vec2 CONTINUE_SIZE({ 260, 80 });
const glm::vec2 CONTINUE_MARGIN({ 0.5925, 0.67 });

// UIState struct
struct UIState
{
	// Main buttons
	UIButton play;
	UIButton pause;

	// Pause panel
	UIButton pausePanel;

	// Buttons in the Pause panel
	UIButton exit;
	//UIButton home;
	UIButton continueGame;

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
		
		// Exit, Home, Continue buttons
		exit = UIButton(renderer, res, "exit", EXIT_SIZE, EXIT_MARGIN);
		//home = UIButton(renderer, res, "home", HOME_SIZE, HOME_MARGIN);
		continueGame = UIButton(renderer, res, "continue", CONTINUE_SIZE, CONTINUE_MARGIN);
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

		// Exit, Home and Continue buttons
		exit.render(state);
		continueGame.render(state);
	}

	void updateButtons(float mouseX, float mouseY, bool clicked, SDLState sdl)
	{
		// Check each buttons
		play.updateClicked(mouseX, mouseY, clicked, sdl);
		pause.updateClicked(mouseX, mouseY, clicked, sdl);
		exit.updateClicked(mouseX, mouseY, clicked, sdl);
		//home.updateClicked(mouseX, mouseY, clicked, sdl);
		continueGame.updateClicked(mouseX, mouseY, clicked, sdl);
	}

	void switchToHomeScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible, not toggled
		pause.setVisible(true);
		pause.setToggled(false);

		// ----- PLAY BUTTON -----
		// Visible
		play.setVisible(true);

		// ----- PAUSE PANEL -----
		// Not visible
		pausePanel.setVisible(false);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);
	}

	void switchToPlayScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible, not toggled
		pause.setVisible(true);
		pause.setToggled(false);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANNEL -----
		// Not visible
		pausePanel.setVisible(false);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);
	}

	void switchToHomePauseScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible and toggled
		pause.setVisible(true);
		pause.setToggled(true);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANEL -----
		// Visible
		pausePanel.setVisible(true);

		// ----- EXIT BUTTON -----
		// Visible
		exit.setVisible(true);

		// ----- HOME BUTTON -----

		// ----- CONTINUE BUTTON -----
		// Visible
		continueGame.setVisible(true);
	}
};