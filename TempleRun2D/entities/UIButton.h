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
        normalTexture = res.loadTexture(renderer, "assets/buttons/" + name + "_normal.png");
        clickedTexture = res.loadTexture(renderer, "assets/buttons/" + name + "_clicked.png");
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

    void render(SDL_Renderer* renderer) const
    {
        // If the button is not visible we don't need to draw it
        if (!visible) 
        {
            return;
        }

        // Get the position of the button
        SDL_FRect dst{ position.x, position.y, size.x, size.y };

        // Store the texture to draw
        SDL_Texture* texToDraw = nullptr;

        // Choose the correct texture to draw
        if (clicked)
        {
            texToDraw = clickedTexture;
        } 
        else
        {
            texToDraw = normalTexture;
        }

        // Draw the texture
        SDL_RenderTexture(renderer, texToDraw, nullptr, &dst);
    }
};