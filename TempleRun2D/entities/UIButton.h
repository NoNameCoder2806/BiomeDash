#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <string>

#include "Resources.h"

// ----- UIBUTTON CLASS -----
class UIButton
{
private:
    // Button name
    std::string name;

    // Button texture
    SDL_Texture* normalTexture = nullptr;
    SDL_Texture* clickedTexture = nullptr;

    // Position and size
    glm::vec2 position{ 0, 0 };   // SCREEN space
    glm::vec2 size{ 0, 0 };

    // Whether the object is visible and clicked
    bool visible = true;
    bool clicked = false;

public:
    UIButton() = default;

    UIButton(SDL_Texture* tex, glm::vec2 pos, glm::vec2 sz)
        : normalTexture(tex), position(pos), size(sz) {
    }

    UIButton(SDL_Renderer* renderer, Resources& res, const std::string& n, glm::vec2 pos, glm::vec2 sz)
        : name(n), position(pos), size(sz)
    {
        normalTexture = res.loadTexture(renderer, "data/textures/ui/" + name + "_normal.png");
        clickedTexture = res.loadTexture(renderer, "data/textures/ui/" + name + "_clicked.png");
    }

    // Getters
    std::string getName() { return name; }
    glm::vec2 getPosition() { return position; }
    glm::vec2 getSize() { return size; }
    bool isVisible() { return visible; }
    bool isClicked() { return clicked; }

    // Setters
    void setName(std::string n) { name = n; }
    void setTexture(SDL_Texture* tex) { normalTexture = tex; }
    void setPosition(glm::vec2 pos) { position = pos; }
    void setSize(glm::vec2 sz) { size = sz; }
    void setVisible(bool v) { visible = v; }

    // Helper functions
    bool isHovered(float mx, float my) const
    {
        return visible &&
            mx >= position.x && mx <= position.x + size.x &&
            my >= position.y && my <= position.y + size.y;
    }

    void updateClicked(float mx, float my, bool mouseDown)
    {
        clicked = mouseDown && isHovered(mx, my);
    }

    void render(const SDLState& sdl) const
    {
        // If the button is not visible, skip drawing
        if (!visible)
        {
            return;
        }

        // Calculate scaling based on reference resolution (e.g., 1920x1080)
        const float refWidth = 1920.0f;
        const float refHeight = 1080.0f;

        float scaleX = static_cast<float>(sdl.width) / refWidth;
        float scaleY = static_cast<float>(sdl.height) / refHeight;

        // Choose which texture to draw
        SDL_Texture* texToDraw = clicked ? clickedTexture : normalTexture;

        // Get the Textures width and height
        float texWidth = 0;
        float texHeight = 0;
        SDL_GetTextureSize(texToDraw, &texWidth, &texHeight);

        // Destination rect — use your set position directly
        SDL_FRect dst
        {
            .x = position.x,           // don't scale the position
            .y = position.y,           // don't scale the position
            .w = texWidth * scaleX,    // scale the size
            .h = texHeight * scaleY
        };

        SDL_RenderTexture(sdl.renderer, texToDraw, nullptr, &dst);

        // Debug
        //std::cout << "Drew the " << name << " button at (" << dst.x << ", " << dst.y << ") size (" << dst.w << "x" << dst.h << ")" << std::endl;
    }
};