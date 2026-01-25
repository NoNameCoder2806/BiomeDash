#pragma once

#include <SDL3/SDL.h>

#include "ui/UIButton.h"
#include "ui/AnimatedUIButton.h"
#include "GameState.h"

// Constants
// PLAY BUTTON
const glm::vec2 PLAY_SIZE({ 260, 80 });
const glm::vec2 PLAY_MARGIN({ 0.5, 0.625 });

// PAUSE BUTTON
const glm::vec2 PAUSE_SIZE({ 84, 84 });
const glm::vec2 PAUSE_MARGIN({ 0.975, 0.08 });

// PAUSE PANEL
const glm::vec2 PAUSE_PANEL_SIZE({ 672, 504 });
const glm::vec2 PAUSE_PANEL_MARGIN({ 0.5, 0.55 });

// EXIT BUTTON
const glm::vec2 EXIT_SIZE({ 260, 80 });
const glm::vec2 EXIT_MARGIN({ 0.41, 0.67 });

// HOME BUTTON
const glm::vec2 HOME_SIZE({ 260, 80 });
const glm::vec2 HOME_MARGIN({ 0.41, 0.67 });

// CONTINUE BUTTON
const glm::vec2 CONTINUE_SIZE({ 260, 80 });
const glm::vec2 CONTINUE_MARGIN({ 0.59, 0.67 });

// VOLUME BUTTON
const glm::vec2 VOLUME_SIZE({ 110, 100 });
const glm::vec2 VOLUME_MARGIN({ 0.415, 0.495 });

// RESTART BUTTON
const glm::vec2 RESTART_SIZE({ 110, 110 });
const glm::vec2 RESTART_MARGIN({ 0.585, 0.495 });

// CREDITS BUTTON
const glm::vec2 CREDITS_SIZE({ 110, 110 });
const glm::vec2 CREDITS_MARGIN({ 0.585, 0.495 });

// MAIN MENU BUTTON
const glm::vec2 MAIN_MENU_SIZE({ 520, 80 });
const glm::vec2 MAIN_MENU_MARGIN({ 0.5, 0.67 });

// AVATAR BUTTON
const glm::vec2 AVATAR_SIZE({ 534, 150 });
const glm::vec2 AVATAR_MARGIN({ 0.06, 0.08 });

// CHARACTER AVATAR
const glm::vec2 CHARACTER_AVATAR_SIZE({ 120, 120 });
const glm::vec2 CHARACTER_AVATAR_MARGIN({ 0.055, 0.09 });

// HEROES BUTTON
const glm::vec2 HEROES_SIZE({ 260, 80 });
const glm::vec2 HEROES_MARGIN({ 0.5, 0.75 });

// BACK BUTTON
const glm::vec2 BACK_SIZE({ 364, 112 });
const glm::vec2 BACK_MARGIN({ 0.5, 0.85 });

// NAME TAG
const glm::vec2 NAME_TAG_SIZE({ 442, 112 });
const glm::vec2 NAME_TAG_MARGIN({ 0.5, 0.25 });

// RIGHT ARROW BUTTON
const glm::vec2 RIGHT_ARROW_SIZE({ 60, 100 });
const glm::vec2 RIGHT_ARROW_MARGIN({ 0.65, 0.55 });

// LEFT ARROW BUTTON
const glm::vec2 LEFT_ARROW_SIZE({ 60, 100 });
const glm::vec2 LEFT_ARROW_MARGIN({ 0.35, 0.55 });

// WINNING SCREEN PANNEL
const glm::vec2 WINNING_SCREEN_SIZE({ 1001, 504 });        // Frame size
const glm::vec2 WINNING_SCREEN_FRAME_SIZE({ 143, 72 });    // Pixels size
const glm::vec2 WINNING_SCREEN_MARGIN({ 0.5, 0.5 });

// STARS
const glm::vec2 STARS_SIZE({ 44, 44 });
const glm::vec2 STARS_MARGIN({ 0.175, 0.1575 });

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
	UIButton home;
	UIButton continueGame;
	UIButton volume;
	UIButton restart;
	UIButton credits;
	UIButton mainMenu;

	// Top left avatar
	UIButton avatar;
	UIButton characterAvatar;
	//UIButton changeName;

	// Change heroes
	UIButton heroes;
	UIButton back;
	UIButton leftArrow;
	UIButton rightArrow;
	UIButton nameTag;

	// Winning pannel
	AnimatedUIButton winningScreen;

	// Stars
	UIButton stars;

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
		home = UIButton(renderer, res, "home", HOME_SIZE, HOME_MARGIN);
		continueGame = UIButton(renderer, res, "continue", CONTINUE_SIZE, CONTINUE_MARGIN);
	
		// Volume, Restart and Credits buttons
		volume = UIButton(renderer, res, "volume", VOLUME_SIZE, VOLUME_MARGIN);
		restart = UIButton(renderer, res, "restart", RESTART_SIZE, RESTART_MARGIN);
		credits = UIButton(renderer, res, "credits", CREDITS_SIZE, CREDITS_MARGIN);
	
		// Main Menu button
		mainMenu = UIButton(renderer, res, "mainMenu", MAIN_MENU_SIZE, MAIN_MENU_MARGIN);
	
		// Character Avatar and Avatar button
		avatar = UIButton(renderer, res, "avatar", AVATAR_SIZE, AVATAR_MARGIN);
		characterAvatar = UIButton(renderer, res, "characterAvatar", CHARACTER_AVATAR_SIZE, CHARACTER_AVATAR_MARGIN);

		// Heroes change buttons
		heroes = UIButton(renderer, res, "heroes", HEROES_SIZE, HEROES_MARGIN);
		back = UIButton(renderer, res, "back", BACK_SIZE, BACK_MARGIN);
		leftArrow = UIButton(renderer, res, "leftArrow", LEFT_ARROW_SIZE, LEFT_ARROW_MARGIN);
		rightArrow = UIButton(renderer, res, "rightArrow", RIGHT_ARROW_SIZE, RIGHT_ARROW_MARGIN);
		nameTag = UIButton(renderer, res, "nameTag", NAME_TAG_SIZE, NAME_TAG_MARGIN);
	
		// Winning screen
		winningScreen = AnimatedUIButton(renderer, res, "winningScreen", WINNING_SCREEN_SIZE, WINNING_SCREEN_MARGIN, WINNING_SCREEN_FRAME_SIZE);
		Animation anim(3, 1.0f, true);
		winningScreen.setAnimation(anim);

		// Stars
		stars = UIButton(renderer, res, "star", STARS_SIZE, STARS_MARGIN);
	}

	void render(const SDLState& state)
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
		home.render(state);
		continueGame.render(state);

		// Volume, Restart and Credits buttons
		volume.render(state);
		restart.render(state);
		credits.render(state);

		// Character Avatar and Avatar button
		avatar.render(state);
		characterAvatar.render(state);

		// Change Heroes, Back, Select, Left and Right Arrow buttons
		heroes.render(state);
		back.render(state);
		leftArrow.render(state);
		rightArrow.render(state);
		nameTag.render(state);

		// Winning screen
		winningScreen.render(state);

		// Main Menu button
		mainMenu.render(state);
	}

	void renderStars(const SDLState& state, int numStars)
	{
		// Render the stars based on the number of stars
		for (int i = 0; i < numStars; i++)
		{
			glm::vec2 newMargin = STARS_MARGIN;
			newMargin.x = newMargin.x + i * 0.03;
			stars.setMargin(newMargin);
			stars.render(state);
		}

		// Reset the margin
		stars.setMargin(STARS_MARGIN);
	}

	void updateButtons(float mouseX, float mouseY, bool clicked, SDLState sdl, float deltaTime = 0.0f)
	{
		// Check each buttons
		play.updateClicked(mouseX, mouseY, clicked, sdl);
		pause.updateClicked(mouseX, mouseY, clicked, sdl);
		exit.updateClicked(mouseX, mouseY, clicked, sdl);
		home.updateClicked(mouseX, mouseY, clicked, sdl);
		continueGame.updateClicked(mouseX, mouseY, clicked, sdl);
		volume.updateClicked(mouseX, mouseY, clicked, sdl);
		restart.updateClicked(mouseX, mouseY, clicked, sdl);
		credits.updateClicked(mouseX, mouseY, clicked, sdl);
		mainMenu.updateClicked(mouseX, mouseY, clicked, sdl);
		avatar.updateClicked(mouseX, mouseY, clicked, sdl);
		characterAvatar.updateClicked(mouseX, mouseY, clicked, sdl);
		heroes.updateClicked(mouseX, mouseY, clicked, sdl);
		back.updateClicked(mouseX, mouseY, clicked, sdl);
		leftArrow.updateClicked(mouseX, mouseY, clicked, sdl);
		rightArrow.updateClicked(mouseX, mouseY, clicked, sdl);
		nameTag.updateClicked(mouseX, mouseY, clicked, sdl);
		winningScreen.stepAnimation(deltaTime);
		winningScreen.updateClicked(mouseX, mouseY, clicked, sdl);
		stars.updateClicked(mouseX, mouseY, clicked, sdl);
	}

	void switchToHomeScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible, not toggled
		pause.setVisible(true);
		pause.setToggled(false);
		pause.setClickable(true);

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
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);

		// ----- VOLUME BUTTON -----
		// Not visible
		volume.setVisible(false);

		// ----- RESTART BUTTON -----
		// Not visible
		restart.setVisible(false);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Not visible
		mainMenu.setVisible(false);

		// ----- AVATAR BUTTON -----
		// Visible
		avatar.setVisible(true);

		// ----- CHARACTER AVATAR -----
		// Visible
		characterAvatar.setVisible(true);

		// ----- STARS -----
		// Visible
		stars.setVisible(true);

		// ----- HEROES BUTTON -----
		// Visible
		heroes.setVisible(true);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToPlayScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible, not toggled
		pause.setVisible(true);
		pause.setToggled(false);
		pause.setClickable(true);

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
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);

		// ----- VOLUME BUTTON -----
		// Not visible
		volume.setVisible(false);

		// ----- RESTART BUTTON -----
		// Not visible
		restart.setVisible(false);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Not visible
		mainMenu.setVisible(false);

		// ----- AVATAR BUTTON -----
		// Visible
		avatar.setVisible(true);

		// ----- STARS -----
		// Visible
		stars.setVisible(true);

		// ----- CHARACTER AVATAR -----
		// Visible
		characterAvatar.setVisible(true);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToHomePauseScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible and toggled
		pause.setVisible(true);
		pause.setToggled(true);
		pause.setClickable(true);

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
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Visible
		continueGame.setVisible(true);

		// ----- VOLUME BUTTON -----
		// Visible
		volume.setVisible(true);

		// ----- RESTART BUTTON -----
		// Not visible
		restart.setVisible(false);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(true);

		// ----- MAIN MENU BUTTON -----
		// Not visible
		mainMenu.setVisible(false);

		// ----- AVATAR BUTTON -----
		// Visible
		avatar.setVisible(true);

		// ----- STARS -----
		// Visible
		stars.setVisible(true);

		// ----- CHARACTER AVATAR -----
		// Visible
		characterAvatar.setVisible(true);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToPlayingPauseScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible and toggled
		pause.setVisible(true);
		pause.setToggled(true);
		pause.setClickable(true);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANEL -----
		// Visible
		pausePanel.setVisible(true);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----
		// Visible
		home.setVisible(true);

		// ----- CONTINUE BUTTON -----
		// Visible
		continueGame.setVisible(true);

		// ----- VOLUME BUTTON -----
		// Visible
		volume.setVisible(true);

		// ----- RESTART BUTTON -----
		// Visible
		restart.setVisible(true);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Not visible
		mainMenu.setVisible(false);

		// ----- AVATAR BUTTON -----
		// Visible
		avatar.setVisible(true);

		// ----- STARS -----
		// Visible
		stars.setVisible(true);

		// ----- CHARACTER AVATAR -----
		// Visible
		characterAvatar.setVisible(true);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToGameOverScreen()
	{
		// ----- PAUSE BUTTON -----
		// Visible and not toggled
		pause.setVisible(true);
		pause.setToggled(false);
		pause.setClickable(true);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANEL -----
		// Visible
		pausePanel.setVisible(true);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);

		// ----- VOLUME BUTTON -----
		// Visible
		volume.setVisible(true);

		// ----- RESTART BUTTON -----
		// Visible
		restart.setVisible(true);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Visible
		mainMenu.setVisible(true);

		// ----- AVATAR BUTTON -----
		// Visible
		avatar.setVisible(true);

		// ----- STARS -----
		// Visible
		stars.setVisible(true);

		// ----- CHARACTER AVATAR -----
		// Visible
		characterAvatar.setVisible(true);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToChangePlayerTransitionScreen()
	{
		// ----- PAUSE BUTTON -----
		// Not visible
		pause.setVisible(false);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANEL -----
		// Not visible
		pausePanel.setVisible(false);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);

		// ----- VOLUME BUTTON -----
		// Not visible
		volume.setVisible(false);

		// ----- RESTART BUTTON -----
		// Not visible
		restart.setVisible(false);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Not visible
		mainMenu.setVisible(false);

		// ----- AVATAR BUTTON -----
		// Not visible
		avatar.setVisible(false);

		// ----- STARS -----
		// Not visible
		stars.setVisible(false);

		// ----- CHARACTER AVATAR -----
		// Not visible
		characterAvatar.setVisible(false);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToChangePlayerScreen()
	{
		// ----- PAUSE BUTTON -----
		// Not visible
		pause.setVisible(false);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANEL -----
		// Not visible
		pausePanel.setVisible(false);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);

		// ----- VOLUME BUTTON -----
		// Not visible
		volume.setVisible(false);

		// ----- RESTART BUTTON -----
		// Not visible
		restart.setVisible(false);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Not visible
		mainMenu.setVisible(false);

		// ----- AVATAR BUTTON -----
		// Not visible
		avatar.setVisible(false);

		// ----- STARS -----
		// Not visible
		stars.setVisible(false);

		// ----- CHARACTER AVATAR -----
		// Not visible
		characterAvatar.setVisible(false);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Visible
		back.setVisible(true);

		// ----- LEFT AND RIGHT ARROWS -----
		// Visible
		leftArrow.setVisible(true);
		rightArrow.setVisible(true);

		// ----- NAME TAG -----
		// Visible
		nameTag.setVisible(true);

		// ----- WINNING PANNEL -----
		// Not visible
		winningScreen.setVisible(false);
	}

	void switchToWinningScreen()
	{
		// ----- PAUSE BUTTON -----
		// Not visible and not toggled
		pause.setVisible(true);
		pause.setToggled(false);
		pause.setClickable(false);

		// ----- PLAY BUTTON -----
		// Not visible
		play.setVisible(false);

		// ----- PAUSE PANEL -----
		// Not visible
		pausePanel.setVisible(false);

		// ----- EXIT BUTTON -----
		// Not visible
		exit.setVisible(false);

		// ----- HOME BUTTON -----
		// Not visible
		home.setVisible(false);

		// ----- CONTINUE BUTTON -----
		// Not visible
		continueGame.setVisible(false);

		// ----- VOLUME BUTTON -----
		// Not visible
		volume.setVisible(false);

		// ----- RESTART BUTTON -----
		// Not visible
		restart.setVisible(false);

		// ----- CREDITS BUTTON -----
		// Not visible
		credits.setVisible(false);

		// ----- MAIN MENU BUTTON -----
		// Visible
		mainMenu.setVisible(true);

		// ----- AVATAR BUTTON -----
		// Visible
		avatar.setVisible(true);

		// ----- STARS -----
		// Visible
		stars.setVisible(true);

		// ----- CHARACTER AVATAR -----
		// Visible
		characterAvatar.setVisible(true);

		// ----- HEROES BUTTON -----
		// Not visible
		heroes.setVisible(false);

		// ----- BACK BUTTON -----
		// Not visible
		back.setVisible(false);

		// ----- LEFT AND RIGHT ARROWS -----
		// Not visible
		leftArrow.setVisible(false);
		rightArrow.setVisible(false);

		// ----- NAME TAG -----
		// Not visible
		nameTag.setVisible(false);

		// ----- WINNING PANNEL -----
		// Visible
		winningScreen.setVisible(true);
	}
};