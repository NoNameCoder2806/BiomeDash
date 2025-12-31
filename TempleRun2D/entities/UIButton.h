#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include <string>

// ----- UIBUTTON CLASS -----
class UIButton
{
private:
    // Button texture
    SDL_Texture* texture = nullptr;

    // Position and size
    glm::vec2 position{ 0, 0 };   // SCREEN space
    glm::vec2 size{ 0, 0 };

    // Whether the object is visible
    bool visible = true;

public:
    UIButton() = default;

    UIButton(SDL_Texture* tex, glm::vec2 pos, glm::vec2 sz)
        : texture(tex), position(pos), size(sz) {
    }

    void setTexture(SDL_Texture* tex) { texture = tex; }
    void setPosition(glm::vec2 pos) { position = pos; }
    void setSize(glm::vec2 sz) { size = sz; }
    void setVisible(bool v) { visible = v; }

    bool isHovered(float mx, float my) const
    {
        return visible &&
            mx >= position.x && mx <= position.x + size.x &&
            my >= position.y && my <= position.y + size.y;
    }

    bool isClicked(float mx, float my, bool mouseDown) const
    {
        return mouseDown && isHovered(mx, my);
    }

    void render(SDL_Renderer* renderer) const
    {
        if (!visible || !texture) return;

        SDL_FRect dst
        {
            position.x,
            position.y,
            size.x,
            size.y
        };

        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
};