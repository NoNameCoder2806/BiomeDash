#include "TempleRun2D.h"

using namespace std;

// FUNCTION IMPLEMENTATIONS
// ----- I/ DRAW PARALLAX BACKGROUNDS -----
void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime)
{
	// If the texture is null
	if (!texture)
	{
		// We exit
		return;
	}

	// Get texture size safely
	float texW, texH;
	SDL_GetTextureSize(texture, &texW, &texH);

	// Scroll background relative to player velocity and scroll factor (parallax speed)
	scrollPos -= xVelocity * scrollFactor * deltaTime;

	// Loop scrolling by resetting position once fully scrolled off screen
	if (scrollPos <= -texW)
	{
		scrollPos = 0;
	}

	// Destination rect for rendering tiled background
	SDL_FRect dst
	{
		.x = scrollPos,
		.y = 0, // Fixed Y offset for the background layer
		.w = texW * 2.0f, // Draw double width for seamless tiling
		.h = static_cast<float>(texH)
	};

	// Render tiled texture to fill the horizontal area smoothly
	SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}

// ----- II/ COUNTS OBJECTS -----
//void countObjectsWithTexture(const GameState& game)
//{
//	int count = 0;
//	for (const auto& layer : game.layers)
//	{
//		for (const auto& objPtr : layer)
//		{
//			if (objPtr->getTexture() != nullptr)
//				++count;
//		}
//	}
//	
//	cout << "Total number of Objects: " << count << endl;
//}