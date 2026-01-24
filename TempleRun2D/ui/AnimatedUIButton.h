#pragma once
#include "UIButton.h"
#include "Animation.h"

class AnimatedUIButton : public UIButton
{
protected:
    Animation animation;       // Single animation for this button
    glm::vec2 frameSize{ 0, 0 };

public:
    AnimatedUIButton() = default;

    // Constructors inherit UIButton behavior
    AnimatedUIButton(SDL_Texture* tex, glm::vec2 sz, glm::vec2 mg, glm::vec2 frameSz)
        : UIButton(tex, sz, mg), frameSize(frameSz)
    {
    }

    AnimatedUIButton(SDL_Renderer* renderer, Resources& res, const std::string& n, glm::vec2 sz, glm::vec2 mg, glm::vec2 frameSz)
        : UIButton(renderer, res, n, sz, mg), frameSize(frameSz)
    {
    }

    // Set animation
    void setFrameSize(glm::vec2 fs) 
    { 
        frameSize = fs; 
    }

    void setAnimation(const Animation& anim)
    {
        animation = anim;
    }

    // Update the animation step
    void stepAnimation(float deltaTime)
    {
        animation.step(deltaTime);
    }

    int getFrame() const
    {
        return animation.currentFrame();
    }

    // Override render to draw animation
    void render(const SDLState& sdl) const
    {
        if (!isVisible()) return;

        SDL_Rect oldViewport;
        SDL_GetRenderViewport(sdl.renderer, &oldViewport);
        SDL_SetRenderViewport(sdl.renderer, nullptr);

        const float refWidth = 1920.0f;
        const float refHeight = 1080.0f;
        float scaleX = static_cast<float>(sdl.width) / refWidth;
        float scaleY = static_cast<float>(sdl.height) / refHeight;

        float dstW = getSize().x * scaleX; // on-screen size
        float dstH = getSize().y * scaleY;

        float dstX = getMargin().x * (sdl.width - dstW);
        float dstY = getMargin().y * (sdl.height - dstH);

        SDL_FRect dst{ dstX, dstY, dstW, dstH };

        SDL_Texture* tex = getTexture();
        if (tex)
        {
            int frame = animation.currentFrame();
            SDL_FRect src{ frame * (int)frameSize.x, 0, (int)frameSize.x, (int)frameSize.y };
            SDL_RenderTexture(sdl.renderer, tex, &src, &dst);
        }

        SDL_SetRenderViewport(sdl.renderer, &oldViewport);
    }
};
