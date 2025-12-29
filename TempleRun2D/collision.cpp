#include "TempleRun2D.h"

using namespace std;

// FUNCTION IMPLEMENTATIONS
// ----- I/ CHECK COLLISION -----
void checkCollision(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB, float deltaTime)
{
	for (const auto& colA : objA.getCollider())  // Loop through objA colliders
	{
		SDL_FRect rectA{
			.x = objA.getPosition().x + colA.x,
			.y = objA.getPosition().y + colA.y,
			.w = colA.w,
			.h = colA.h
		};

		for (const auto& colB : objB.getCollider())  // Loop through objB colliders
		{
			SDL_FRect rectB{
				.x = objB.getPosition().x + colB.x,
				.y = objB.getPosition().y + colB.y,
				.w = colB.w,
				.h = colB.h
			};

			SDL_FRect rectC{ 0 };
			if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
			{
				// Found intersection, respond
				collisionResponse(state, gs, res, objA, objB, rectA, rectB, rectC, deltaTime);
			}
		}
	}
}

// ----- II/ COLLISION RESPONSE -----
void collisionResponse(SDLState& state, GameState& gs, Resources& res, GameObject& objA, GameObject& objB,
	SDL_FRect rectA, SDL_FRect rectB, SDL_FRect rectC, float deltaTime)
{
	// Check the type of the object colliding
	if (objA.getType() == ObjectType::player)
	{
		// Cast the Player type
		Player& player = static_cast<Player&>(objA);

		switch (objB.getType())
		{
		case ObjectType::level:
		{
			// The position and velocity of the player
			glm::vec2 pos = player.getPosition();    // Position
			glm::vec2 vel = player.getVelocity();    // Velocity

			// Horizontal collision
			if (rectC.w <= rectC.h)
			{
				if (player.getVelocity().x > 0)
				{
					pos.x -= rectC.w;    // Add to the left
				}

				if (player.getVelocity().x < 0)
				{
					pos.x += rectC.w;    // Add to the right
				}

				vel.x = 0;
			}

			// Vertical collision
			if (rectC.w > rectC.h)
			{
				if (player.getVelocity().y > 0)
				{
					pos.y -= rectC.h;    // Going down
				}
				else if (player.getVelocity().y < 0)
				{
					pos.y += rectC.h;    // Going up
				}

				vel.y = 0;  // Stop object from moving further

				if (PLAYING)
				{
					if (player.getState() == PlayerState::jumping)
					{
						player.setState(PlayerState::running);
					}
				}
			}

			// Set the new position and velocity
			player.setPosition(pos);
			player.setVelocity(vel);

			break;
		}

		case ObjectType::monster:
		{
			// Set player state to caught if the player is playing 
			if (player.isAlive())
			{

				cout << "Player Caught!!!" << endl;

				player.setState(PlayerState::caught);
				player.setAlive(false);
				PLAYING = false;
			}

			Monster& monster = static_cast<Monster&>(objB);

			// Set monster state to killing
			monster.setState(MonsterState::killing);

			break;
		}

		case ObjectType::obstacle:
		{
			if (gs.invincibleMode)
			{
				break;
			}

			// Cast the GameObject to Obstacle type
			Obstacle& obs = static_cast<Obstacle&>(objB);

			switch (obs.getObstacleType())
			{
			case ObstacleType::wall:
			{
				// The position and velocity of the player
				glm::vec2 pos = player.getPosition();    // Position
				glm::vec2 vel = player.getVelocity();    // Velocity

				// Horizontal collision
				if (rectC.w <= rectC.h)
				{
					if (player.getVelocity().x != 0)
					{
						pos.x -= rectC.w;  // Add to the left
					}

					if (player.getVelocity().x < 0)
					{
						pos.x += rectC.w;  // Add to the right
					}

					vel.x = 0;

					player.setState(PlayerState::knocked);
					PLAYING = false;
				}

				// Vertical collision
				if (rectC.w > rectC.h)
				{
					if (player.getVelocity().y > 0)
					{
						pos.y -= rectC.h;  // Going down
					}
					else if (player.getVelocity().y < 0)
					{
						pos.y += rectC.h;  // Going up
					}

					vel.y = 0;  // Stop object from moving further

					if (PLAYING)
					{
						if (player.getState() == PlayerState::jumping)
						{
							player.setState(PlayerState::running);
						}
					}
				}

				// Set the new position and velocity
				player.setPosition(pos);
				player.setVelocity(vel);

				break;
			}

			case ObstacleType::spike:
			{
				player.setState(PlayerState::bleed);
				PLAYING = false;

				break;
			}

			case ObstacleType::burnt:
			{
				player.setState(PlayerState::burnt);
				PLAYING = false;

				break;
			}

			case ObstacleType::tripped:
			{
				if (player.getState() == PlayerState::caught)
				{
					break;
				}
				// The position and velocity of the player
				glm::vec2 pos = player.getPosition();    // Position
				glm::vec2 vel = player.getVelocity();    // Velocity

				// Horizontal collision
				if (rectC.w <= rectC.h)
				{
					player.setState(PlayerState::tripped);
				}

				// Vertical collision
				if (rectC.w > rectC.h)
				{
					if (player.getVelocity().y > 0)
					{
						pos.y -= rectC.h;  // Going down
					}
					else if (player.getVelocity().y < 0)
					{
						pos.y += rectC.h;  // Going up
					}

					vel.y = 0;  // Stop object from moving further
				}

				// Set the new position and velocity
				player.setPosition(pos);
				player.setVelocity(vel);

				break;
			}
			}

			break;
		}
		// End portal
		case ObjectType::endportal:
		{
			// Debug
			//cout << "Reached Portal" << endl;

			// Set the needTransition flag to true
			gs.needTransition = true;

			break;
		}
		// Commander
		case ObjectType::commander:
		{
			// Debug
			//cout << "Reached the Commander" << endl;

			// Set player state to won
			player.setState(PlayerState::won);

			// Ste the texture and animation to the idle one
			player.setTexture(res.playerIdle);
			player.setCurrentAnimation(res.ANIM_PLAYER_IDLE);

			// Reverse the animation of the commander
			Level& commander = static_cast<Level&>(objB);

			// Set the commander to be grounded, meaning that 
			// the player has collided with the commander
			commander.setGrounded(true);

			break;
		}
		}
	}
	else if (objA.getType() == ObjectType::monster)
	{

	}
}

// ----- III/ CHECK GROUNDED STATUS -----
bool checkGrounded(const GameObject& player, const std::vector<std::unique_ptr<GameObject>>& levelTiles)
{
	SDL_FRect playerCollider = player.getCollider().at(0);

	float playerLeft = player.getPosition().x + playerCollider.x;
	float playerRight = playerLeft + playerCollider.w;
	float playerBottom = player.getPosition().y + playerCollider.y + playerCollider.h;

	// Small tolerance for float comparisons
	const float tolerance = 2.0f;

	for (const auto& tile : levelTiles)
	{
		for (int i = 0; i < tile->getCollider().size(); i++)
		{
			SDL_FRect tileRect{
				tile->getPosition().x + tile->getCollider().at(i).x,
				tile->getPosition().y + tile->getCollider().at(i).y,
				tile->getCollider().at(i).w,
				tile->getCollider().at(i).h
			};

			float tileLeft = tileRect.x;
			float tileRight = tileRect.x + tileRect.w;
			float tileTop = tileRect.y;

			// Check horizontal overlap
			bool overlapX = (playerRight > tileLeft) && (playerLeft < tileRight);

			// Check if player bottom is within tolerance of tile top
			bool onTop = (playerBottom >= tileTop - tolerance && playerBottom <= tileTop + tolerance);

			if (overlapX && onTop)
			{
				return true;
			}
		}
	}

	return false;
}