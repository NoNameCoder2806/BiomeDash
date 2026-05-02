#pragma once
#include "Timer.h"

class Animation
{
	Timer timer;       // Controls timing of the animation
	int frameCount;    // Total number of frames in this animation
	bool loop;

public:
	Animation() : timer(0), frameCount(0)
	{

	}

	Animation(int frameCount, float length, bool shouldLoop) : frameCount(frameCount), timer(length), loop(shouldLoop)
	{

	}

	Timer& getTimer()
	{
		return timer;
	}

	float getLength() const
	{
		return timer.getLength();
	}

	int currentFrame() const
	{
		if (timer.getTime() <= 0)
		{
			return 0;
		}

		if (!loop && timer.isTimeout())
		{
			// Freeze on the last frame
			return frameCount - 1;
		}

		return static_cast<int>(timer.getTime() / timer.getLength() * frameCount);
	}

	void step(float deltaTime)
	{
		timer.step(deltaTime);
	}

	// Might not be necessary
	bool isDone() const
	{
		return timer.isTimeout();
	}

	void reset()                 // Reset function
	{
		timer.reset();
	}

	void setLoop(bool shouldLoop)
	{
		loop = shouldLoop;
	}
};