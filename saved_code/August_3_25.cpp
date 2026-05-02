// BiomeDash.cpp : Defines the entry point for the application.
//


// Libraries
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

// Header files
#include "BiomeDash.h"
#include "GameState.h"
#include "GameObject.h"
#include "SDLState.h"
#include "Resources.h"
#include "Animation.h"
#include "Timer.h"

#include <vector>
#include <array>
#include <string>

using namespace std;

// Constants

const int MAP_ROWS = 5;                 // Number of rows in the map
const int MAP_COLS = 50;                // Number of columns in the map
const int TILE_SIZE = 64;

// Functions declarations
bool initialize(SDLState&);
void cleanup(SDLState&);
void createTiles(const SDLState&, GameState&, const Resources&);
void update(const SDLState&, GameState&, const Resources&, GameObject&, float);
void checkCollision(const SDLState&, GameState&, const Resources&, GameObject&, GameObject&, float);
void collisionResponse(const SDLState&, GameState&, const Resources&, const SDL_FRect&, const SDL_FRect&, const SDL_FRect&,
	GameObject&, GameObject&, float);
void handleKeyInput(const SDLState&, GameState&, GameObject&, SDL_Scancode, bool);
void drawObject(const SDLState&, GameState&, const Resources&, GameObject&, int, int, float); 
void drawBackground(SDL_Renderer*, SDL_Texture*, int, int);

int main(int argc, char* argv[])
{
	// Create a SDLState object
	SDLState state;
	state.width = 1600;    // Actual window width in pixels
	state.height = 900;    // Actual window height in pixels
	state.logW = 640;      // Logical width used for scaling/rendering
	state.logH = 320;      // Logical height used for scaling/rendering

	// Initialize SDL and create window and renderer
	if (!initialize(state))
	{
		return 1;  // Exit if initialization failed
	}

	// Create a Resources object
	Resources res;
	res.load(state);

	// Create the main game state and initialize it
	GameState gs(state);

	// Create game tiles
	createTiles(state, gs, res);

	// Store the previous time in milliseconds (used for frame timing)
	uint64_t prevTime = SDL_GetTicks();

	// Start the game loop
	// Main game loop - runs until running is set to false (e.g., window close)
	bool running = true;
	while (running)
	{
		// Get current time in milliseconds and calculate delta time (seconds since last frame)
		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (nowTime - prevTime) / 1000.0f;
		prevTime = nowTime;

		// Event handling loop - process all pending SDL events
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					running = false;  // Window close requested -> exit main loop
					break;
				}

				case SDL_EVENT_WINDOW_RESIZED:
				{
					// Update stored window size when user resizes the window
					state.width = event.window.data1;
					state.height = event.window.data2;
					break;
				}

				case SDL_EVENT_KEY_DOWN:
				{
					// Handle key press events, update player/game state accordingly
					handleKeyInput(state, gs, gs.player(), event.key.scancode, true);
					break;
				}

				case SDL_EVENT_KEY_UP:
				{
					// Handle key release events
					handleKeyInput(state, gs, gs.player(), event.key.scancode, false);

					// Toggle debug mode when F12 or F10 is pressed
					if (event.key.scancode == SDL_SCANCODE_F12 || event.key.scancode == SDL_SCANCODE_F10)
					{
						gs.debugMode = !gs.debugMode;
					}
					break;
				}
			}
		}

		// Center the viewport horizontally on the player (with some offset)
		gs.mapViewport.x = (gs.player().position.x + TILE_SIZE / 2) - gs.mapViewport.w / 2;

		// Clear the screen
		SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
		SDL_RenderClear(state.renderer);

		// Then draw the player
		SDL_RenderTexture(state.renderer, res.texBgDesert, nullptr, nullptr);

		// Update all game objects in all layers with the current delta time
		for (auto& layer : gs.layers)
		{
			for (GameObject& obj : layer)
			{
				update(state, gs, res, obj, deltaTime);
				drawObject(state, gs, res, obj, TILE_SIZE, TILE_SIZE, deltaTime);
			}
		}

		SDL_RenderPresent(state.renderer);
			
	}

	// Create a renderer
	// SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_ACCELERATED);

	std::cout << "Hello CMake." << endl;


	// Clean up after the program has finished running
	cleanup(state);
	return 0;
}

bool initialize(SDLState& state)
{
	// Initialize SDL3
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
		return false;
	}

	// Create a game window
	state.window = SDL_CreateWindow("TempleRun2D", state.width, state.height, 0);
	if (!state.window)  // Check whether the window creation was successful
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
		cleanup(state);
		return false;
	}

	// Create the renderer (responsible for all drawing)
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer)  // Check whether the renderer creation was successful
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
		cleanup(state);
		return false;
	}

	// Enable vertical sync to avoid screen tearing and cap FPS to monitor's refresh rate
	SDL_SetRenderVSync(state.renderer, 1);

	// Set a fixed "logical" resolution for the game (will be scaled automatically)
	// Letterbox mode keeps aspect ratio and adds black bars if needed
	SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return true;
}

void cleanup(SDLState& state)
{
	// Quit after the program has finished running
	// SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}

void createTiles(const SDLState& state, GameState& gs, const Resources& res)
{
	/*
	1. player
	2. monster
	3. floor
	*/

	short map[MAP_ROWS][MAP_COLS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
	};

	/*short background[MAP_ROWS][MAP_COLS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};
	short foreground[MAP_ROWS][MAP_COLS] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};*/


	const auto loadMap = [&state, &gs, &res](short layer[5][50])
	{
		const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type)
		{
			// Create and initialize a game object at grid position (r, c)
			GameObject o;
			o.type = type;
			o.position = glm::vec2(c * TILE_SIZE, state.logH - (5 - r) * TILE_SIZE); // Y is inverted because of coordinate system
			o.texture = tex;
			o.collider = {
				.x = 0,
				.y = 0,
				.w = TILE_SIZE,
				.h = TILE_SIZE
			};
			return o;
		};

		for (int r = 0; r < MAP_ROWS; r++)
		{
			for (int c = 0; c < MAP_COLS; c++)
			{
				switch (layer[r][c])
				{
					case 1: // Player
					{
						GameObject player = createObject(r, c, res.playerIdle, ObjectType::player);
						player.data.player = PlayerData();
						player.collider = SDL_FRect{ 
							.x = player.position.x, 
							.y = 0, 
							.w = 16, 
							.h = 28 
						};
							
						player.animations = res.playerAnims;
						player.currentAnimation = res.ANIM_PLAYER_IDLE;

						player.acceleration = glm::vec2(300, 0);
						player.maxSpeedX = 100;
						player.dynamic = true;

						gs.layers[LAYER_IDX_CHARACTER].push_back(player);
						gs.playerIndex = static_cast<int>(gs.layers[LAYER_IDX_CHARACTER].size() - 1);

						break;
					}

					case 2: // Monster
					{
						GameObject monster = createObject(r, c, res.monsterIdle, ObjectType::monster);
						gs.layers[LAYER_IDX_CHARACTER].push_back(monster);
						break;
					}

					case 3: // Floor
					{
						GameObject floor = createObject(r, c, res.texFloor, ObjectType::level);
						gs.layers[LAYER_IDX_LEVEL].push_back(floor);
					}
				}
			}
		}
	};

	// Load all layers
	loadMap(map);

	// Ensure player was placed
	//assert(gs.playerIndex != -1);
}

void update(const SDLState& state, GameState& gs, const Resources& res, GameObject& obj, float deltaTime)
{
	// Update the animations for this object if it has any
	if (obj.currentAnimation != -1)
	{
		obj.animations[obj.currentAnimation].step(deltaTime);
	}

	// Apply velocity to update the position
	obj.position += obj.velocity * deltaTime;

	// Apply gravity if the object is dynamic (affected by physics) and not on the ground
	if (obj.dynamic && !obj.grounded)
	{
		obj.velocity += glm::vec2(0, 50) * deltaTime; // 500 is gravity strength
	}

	// ----- PLAYER BEHAVIOR -----
	if (obj.type == ObjectType::player)
	{
		// Handle player state machine (Idle / Running / Jumping)
		switch (obj.data.player.state)
		{
			case PlayerState::idle:
			{
				obj.texture = res.playerIdle;
				obj.currentAnimation = res.ANIM_PLAYER_IDLE;

				if (state.keys[SDL_SCANCODE_D] || state.keys[SDL_SCANCODE_RIGHT])
				{
					obj.data.player.state = PlayerState::running;
				}

				if (state.keys[SDL_SCANCODE_S] || state.keys[SDL_SCANCODE_DOWN])
				{
					obj.data.player.state = PlayerState::sliding;
				}

				break;
			}
			case PlayerState::running:
			{
				obj.texture = res.playerRun;
				obj.currentAnimation = res.ANIM_PLAYER_RUN;
				obj.velocity.x = 50;

				if (state.keys[SDL_SCANCODE_J] || state.keys[SDL_SCANCODE_UP] || state.keys[SDL_SCANCODE_SPACE])
				{
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y = 50;
				}

				if (state.keys[SDL_SCANCODE_S] || state.keys[SDL_SCANCODE_DOWN])
				{
					obj.data.player.state = PlayerState::sliding;
				}

				break;
			}

			case PlayerState::jumping:
			{
				obj.texture = res.playerJump;
				obj.currentAnimation = res.ANIM_PLAYER_JUMP;

				if (!state.keys[SDL_SCANCODE_J] && !state.keys[SDL_SCANCODE_UP] && !state.keys[SDL_SCANCODE_SPACE])
				{
					obj.data.player.state = PlayerState::running;
				}
			
				break;
			}

			case PlayerState::sliding:
			{
				obj.texture = res.playerSlide;
				obj.currentAnimation = res.ANIM_PLAYER_SLIDE;

				if (!state.keys[SDL_SCANCODE_S] && !state.keys[SDL_SCANCODE_DOWN])
				{
					obj.data.player.state = PlayerState::running;
				}
				
				break;
			}
		}
	}

	// ----- COLLISION DETECTION & GROUNDED CHECK -----
	bool foundGround = false;

	for (auto& layer : gs.layers)
	{
		for (GameObject& objB : layer)
		{
			if (&obj != &objB)
			{
				checkCollision(state, gs, res, obj, objB, deltaTime);

				if (objB.type == ObjectType::level)
				{
					// Create a small sensor under the player to check if touching ground
					const float inset = 1.0;

					SDL_FRect sensor
					{
						.x = obj.position.x + obj.collider.x,
						.y = obj.position.y + obj.collider.y + obj.collider.h,
						.w = obj.collider.w - inset * 2,
						.h = 1
					};

					SDL_FRect rectB
					{
						.x = objB.position.x + objB.collider.x,
						.y = objB.position.y + objB.collider.y,
						.w = objB.collider.w,
						.h = objB.collider.h
					};

					SDL_FRect rectC{ 0 };

					if (SDL_GetRectIntersectionFloat(&sensor, &rectB, &rectC))
					{
						foundGround = true;
					}
				}
			}
		}
	}

	// Update grounded status if it changed
	if (obj.grounded != foundGround)
	{
		obj.grounded = foundGround;

		if (foundGround && obj.type == ObjectType::player)
		{
			obj.velocity.y = 0;

			if (obj.velocity.x == 0)
			{
				obj.data.player.state = PlayerState::idle;
			}
			else
			{
				obj.data.player.state = PlayerState::running
			}
		}
	}
}

void checkCollision(const SDLState& state, GameState& gs, const Resources& res, GameObject& objA, GameObject& objB, float deltaTime)
{
	// Calculate collision rectangles based on position and collider offsets
	SDL_FRect rectA
	{
		.x = objA.position.x + objA.collider.x,
		.y = objA.position.y + objA.collider.y,
		.w = objA.collider.w + 32,
		.h = objA.collider.h + 32
	};

	SDL_FRect rectB
	{
		.x = objB.position.x + objB.collider.x,
		.y = objB.position.y + objB.collider.y,
		.w = objB.collider.w,
		.h = objB.collider.h
	};

	SDL_FRect rectC{ 0 }; // Intersection rectangle

	// Check if rectangles intersect
	if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
	{
		// If yes, handle collision response
		collisionResponse(state, gs, res, rectA, rectB, rectC, objA, objB, deltaTime);
	}
}

void collisionResponse(const SDLState& state, GameState& gs,const Resources& res,
	const SDL_FRect& rectA, const SDL_FRect& rectB, const SDL_FRect& rectC,
	GameObject& objA, GameObject& objB, float deltaTime)
{
	const auto genericResponse = [&]()
		{
			// Decide whether the collision is horizontal or vertical based on overlap dimensions
			if (rectC.w < rectC.h)
			{
				// Horizontal collision: resolve by pushing objA away horizontally
				if (objA.velocity.x > 0) // Moving right
				{
					objA.position.x -= rectC.w; // Push left by overlap width
				}
				else if (objA.velocity.x < 0) // Moving left
				{
					objA.position.x += rectC.w; // Push right by overlap width
				}
				objA.velocity.x = 0; // Stop horizontal velocity on collision
			}
			else
			{
				// Vertical collision: resolve by pushing objA away vertically
				if (objA.velocity.y > 0) // Moving down
				{
					objA.position.y -= rectC.h; // Push up by overlap height
				}
				else if (objA.velocity.y < 0) // Moving up
				{
					objA.position.y += rectC.h; // Push down by overlap height
				}
				objA.velocity.y = 0; // Stop vertical velocity on collision
			}
		};

	// Handle collisions depending on objA’s type
	if (objA.type == ObjectType::player)
	{
		switch (objB.type)
		{
			case ObjectType::level:
			{
				// Basic collision response with level geometry
				genericResponse();
				break;
			}

			case ObjectType::monster:
			{
				objA.data.player.state = PlayerState::caught;
				objA.velocity.x = 0;

				objB.data.monster.state = MonsterState::killing;
				objB.velocity.x = 0;

				break;
			}
		}
	}
	else if (objA.type == ObjectType::monster)
	{
		// Simple generic collision for enemies
		genericResponse();
	}
}

void handleKeyInput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown)
{
	const float JUMP_FORCE = -200.0f; // Negative to move upward (y decreases upward)

	if (obj.type == ObjectType::player)
	{
		switch (obj.data.player.state)
		{
			case PlayerState::idle:
			case PlayerState::running:
			{
				if ((key == SDL_SCANCODE_K || key == SDL_SCANCODE_UP) && keyDown)
				{
					// Player jumps only if key pressed down in idle state
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y = JUMP_FORCE;
					obj.grounded = false;
				}
				break;
			}
		}
	}
}

void drawObject(const SDLState& state, GameState& gs, const Resources& res, GameObject& obj, int width, int height, float deltaTime)
{
	float srcX = 0;

	// Determine the source X offset for the current frame of the animation
	if (obj.type != ObjectType::monster)
	{
		srcX = obj.currentAnimation != -1
			? obj.animations[obj.currentAnimation].currentFrame() * 32
			: (obj.spriteFrame - 1) * 32;
	}
	else
	{
		srcX = obj.currentAnimation != -1
			? obj.animations[obj.currentAnimation].currentFrame() * 64
			: (obj.spriteFrame - 1) * 64;
	}

	SDL_FRect src{         // Source image
		.x = srcX,
		.y = 0,
		.w = 32,
		.h = 32
	};
	
	SDL_FRect dst{         // Draw destination
		.x = obj.position.x - gs.mapViewport.x,
		.y = obj.position.y,
		.w = TILE_SIZE,
		.h = TILE_SIZE 
	};

	// Flip the sprite if facing left
	SDL_FlipMode flipMode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

	SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipMode);
}

void drawBackground(SDL_Renderer* renderer, SDL_Texture* texture, int width, int height)
{
	// Destination rect for rendering tiled background
	SDL_FRect dst
	{
		.x = 0,
		.y = 0, // Fixed Y offset for the background layer
		.w = static_cast<float>(width),
		.h = static_cast<float>(height)
	};

	// Render tiled texture to fill the horizontal area smoothly
	SDL_RenderTextureTiled(renderer, texture, nullptr, 1, &dst);
}
