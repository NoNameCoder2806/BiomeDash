#include "TempleRun2D.h"

using namespace std;

// FUNCTION IMPLEMENTATIONS
// ----- I/ DRAW PARALLAX BACKGROUNDS -----
void drawParalaxBackground(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime)
{
	// If the texture is null
	if (!texture)
	{
		// Exit
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

// ----- II/ CLEAN UP OFFSCREEN OBJECTS -----
void cleanupOffscreenObjects(GameState& gs)
{
	float marginBehind = 500.0f; // how far behind the player to keep objects
	float playerX = gs.player().getPosition().x;

	for (auto& layer : gs.layers)
	{
		layer.erase(
			std::remove_if(layer.begin(), layer.end(),
				[&](std::unique_ptr<GameObject>& obj)
				{
					const auto& colliders = obj->getCollider();
					// Assume object is far behind until proven otherwise
					for (const auto& col : colliders)
					{
						float rightEdge = obj->getPosition().x + col.x + col.w;
						// If any collider is not far behind the player, keep it
						if (rightEdge >= playerX - marginBehind)
						{
							return false;
						}
					}
					return true; // all colliders are far behind
				}),
			layer.end()
		);
	}
}

// ----- III/ COUNT OBJECTS -----
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

// ----- IV/ RENDER WORLD LABEL -----
void renderWorldLabel(UILabel* label, const glm::vec2& worldPos, const glm::vec2& cameraPos, std::vector<float>& scrollPositions)
{
	if (!label) return;

	// Convert world position to screen position
	float screenX = worldPos.x + scrollPositions[0];
	float screenY = worldPos.y;

	label->setPosition(screenX, screenY);
	label->render();
}