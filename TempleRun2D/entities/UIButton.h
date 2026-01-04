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
    glm::vec2 size{ 0, 0 };
    glm::vec2 margin{ 0, 0 };

    // Whether the object is visible and clicked
    bool visible = true;
    bool clickable = true;
    bool clicked = false;

public:
    UIButton() = default;

    UIButton(SDL_Texture* tex, glm::vec2 sz, glm::vec2 mg)
        : normalTexture(tex), size(sz), margin(mg)
    {

    }

    UIButton(SDL_Renderer* renderer, Resources& res, const std::string& n, glm::vec2 sz, glm::vec2 mg)
        : name(n), size(sz), margin(mg)
    {
        normalTexture = res.loadTexture(renderer, "data/textures/ui/" + name + "_normal.png");
        clickedTexture = res.loadTexture(renderer, "data/textures/ui/" + name + "_clicked.png");
    }

    // Getters
    std::string getName() { return name; }
    glm::vec2 getMargin() { return margin; }
    glm::vec2 getSize() { return size; }
    bool isVisible() { return visible; }
    bool isClicked() { return clicked; }

    // Setters
    void setName(std::string n) { name = n; }
    void setTexture(SDL_Texture* tex) { normalTexture = tex; }
    void setMargin(glm::vec2 mg) { margin = mg; }
    void setSize(glm::vec2 sz) { size = sz; }
    void setVisible(bool v) { visible = v; }

    // Helper functions
    bool isHovered(float mouseX, float mouseY, const SDLState& sdl) const
    {
        // If the button is not visible or not clickable, we return false
        if (!visible || !clickable)
        {
            return false;
        }

        // Scale size based on reference resolution
        const float refWidth = 1920.0f;
        const float refHeight = 1080.0f;
        float scaleX = static_cast<float>(sdl.width) / refWidth;
        float scaleY = static_cast<float>(sdl.height) / refHeight;

        float dstW = size.x * scaleX;
        float dstH = size.y * scaleY;

        float dstX = margin.x * (sdl.width - dstW);
        float dstY = margin.y * (sdl.height - dstH);

        // Check if mouse is inside the rectangle
        return mouseX >= dstX && mouseX <= dstX + dstW &&
            mouseY >= dstY && mouseY <= dstY + dstH;
    }

    void updateClicked(float mouseX, float mouseY, bool mouseDown, const SDLState& sdl)
    {
        // Exit if the button is not clickable
        if (!clickable)
        {
            return;
        }

        clicked = mouseDown && isHovered(mouseX, mouseY, sdl);
    }

    void render(const SDLState& sdl) const
    {
        if (!visible)
        {
            return;
        }

        // Save world viewport
        SDL_Rect oldViewport;
        SDL_GetRenderViewport(sdl.renderer, &oldViewport);

        // Debug
        //std::cout << "Old viewport: x=" << oldViewport.x << ", y=" << oldViewport.y << ", w=" << oldViewport.w << ", h=" << oldViewport.h << std::endl;

        // Switch to screen space
        SDL_SetRenderViewport(sdl.renderer, nullptr);

        // Scale size based on reference resolution
        const float refWidth = 1920.0f;
        const float refHeight = 1080.0f;

        float scaleX = static_cast<float>(sdl.width) / refWidth;
        float scaleY = static_cast<float>(sdl.height) / refHeight;

        float dstW = size.x * scaleX;  // size in screen pixels
        float dstH = size.y * scaleY;

        float dstX = margin.x * (sdl.width - dstW);  // 0-1 margin
        float dstY = margin.y * (sdl.height - dstH);

        // Make sure it never goes off-screen
        //if (dstX + dstW > sdl.width) dstX = sdl.width - dstW;
        //if (dstY + dstH > sdl.height) dstY = sdl.height - dstH;

        SDL_FRect dst
        {
            .x = dstX,
            .y = dstY,
            .w = dstW,
            .h = dstH
        };

        SDL_Texture* texToDraw = clicked ? clickedTexture : normalTexture;
        SDL_RenderTexture(sdl.renderer, texToDraw, nullptr, &dst);
        
        // Restore world viewport
        SDL_SetRenderViewport(sdl.renderer, &oldViewport);

        // Debug
        //std::cout << "Button: " << name << std::endl;
        //std::cout << "Size: " << dst.w << ", " << dst.h << std::endl;
        //std::cout << "Margin: " << margin.x << ", " << margin.y << std::endl;
        //std::cout << "Position: " << dstX << ", " << dstY << std::endl;
        //std::cout << "Screen size: " << sdl.width << ", " << sdl.height << std::endl;
    }
};