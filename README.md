# BiomeDash

BiomeDash is a 2D side-scrolling game built in C++ using SDL3. The project focuses on building a modular game architecture with systems for rendering, input handling, collision detection, and game state management.

## 🎮 Features

### Core Gameplay Systems
- Entity-based architecture for game objects (Player, Monsters, Obstacles, Boosts, Levels)
- Player movement system with multiple states (running, sliding, jumping, death states)
- Collision detection and response system with custom hitboxes per player state

### Game Engine Architecture
- Modular game loop with separated update, render, and input handling logic
- State-based system supporting multiple screens:
  - Home screen
  - Gameplay
  - Pause states
  - Game over screen
  - Transition screens
  - Credits screen

### World & Level System
- Tile-based world system with chunk-based loading
- Dynamic level streaming and tile preloading
- Biome-based environment structure

### UI System
- Custom UI framework with buttons, labels, and animated buttons
- Interactive menus and in-game UI components

### Rendering
- Parallax background system for depth and motion effects
- Texture management system for player and world assets

## 🛠️ Tech Stack
- C++
- SDL3
- SDL3_image
- SDL3_ttf
- SDL3_mixer

## 🧱 Architecture Overview
The project is structured around modular systems:
- `GameState` manages global game flow and state transitions
- `GameObject` acts as a base class for all entities
- Derived entity classes handle specific behaviors (Player, Monster, etc.)
- Separate systems handle rendering, input, collision, and UI

## 📁 Project Structure (simplified)
