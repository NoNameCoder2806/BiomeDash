#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include "Animation.h"
#include "Timer.h"

#include <vector>

// ----- TRANSITION CLASS -----
class Transition
{
private:
	std::vector<Animation> animations;
	int currentAnimation;

	SDL_Texture* texture;
	glm::vec2 position;
	glm::vec2 imageSize;

	float timer;

public:
	// Constructors
	Transition()
	{

	}

	Transition(Resources& res, SDLState& state)
	{
		texture = res.transition;
		animations = res.transitionAnims;
		currentAnimation = res.ANIM_TRANSITION;

		position = { state.width / 2, state.height / 2 };
		imageSize = { 128, 128 };

		timer = 0.0f;
	}

	// Getters
	std::vector<Animation>& getAnimations()
	{
		return animations;
	}

	int getCurrentAnimation() const
	{
		return currentAnimation;
	}

	SDL_Texture* getTexture() const
	{
		return texture;
	}

	glm::vec2 getPosition() const
	{
		return position;
	}

	glm::vec2 getImageSize() const
	{
		return imageSize;
	}

	float getTime() const
	{
		return timer;
	}

	// Setters
	void setAnimations(const std::vector<Animation>& anims)
	{
		animations = anims;
	}

	void setCurrentAnimation(int index)
	{
		currentAnimation = index;
	}

	void setTexture(SDL_Texture* tex)
	{
		texture = tex;
	}

	void setPosition(const glm::vec2& p)
	{
		position = p;
	}

	void setImageSize(glm::vec2 size)
	{
		imageSize = size;
	}

	// Helper functions
	void addTime(float time)
	{
		timer += time;
	}

	void resetTimer()
	{
		timer = 0.0f;
	}
};