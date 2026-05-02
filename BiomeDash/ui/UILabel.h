#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class UILabel
{
private:
    TTF_TextEngine* engine = nullptr;
    TTF_Text* textObj = nullptr;
    TTF_Font* font = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::string currentText;
    
    float x = 0;
    float y = 0;
    
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;
    Uint8 a = 255;
    
    int textW = 0;  // Cached width
    int textH = 0;  // Cached height

public:
    UILabel(SDL_Renderer* ren, const std::string& fontPath, float fontSize)
        : renderer(ren)
    {
        // Load the font
        font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font)
        {
            SDL_Log("Failed to load font %s: %s", fontPath.c_str(), SDL_GetError());
            return;
        }

        // Create a text engine for this renderer
        engine = TTF_CreateRendererTextEngine(renderer);
        if (!engine)
        {
            SDL_Log("Failed to create text engine: %s", SDL_GetError());
            return;
        }
    }

    ~UILabel()
    {
        if (textObj) TTF_DestroyText(textObj);
        if (engine) TTF_DestroyRendererTextEngine(engine);
        if (font) TTF_CloseFont(font);
    }

    void setText(const std::string& str)
    {
        currentText = str;

        // Destroy previous text object if it exists
        if (textObj) TTF_DestroyText(textObj);

        // Create new text object
        textObj = TTF_CreateText(engine, font, str.c_str(), str.length());
        if (!textObj)
        {
            SDL_Log("Failed to create text object: %s", SDL_GetError());
            textW = textH = 0;
            return;
        }

        // Set color
        TTF_SetTextColor(textObj, r, g, b, a);

        // Build layout and glyphs
        TTF_UpdateText(textObj);

        // Cache width and height
        if (!TTF_GetTextSize(textObj, &textW, &textH))
        {
            SDL_Log("Failed to get text size: %s", SDL_GetError());
            textW = textH = 0;
        }

        // Debug
        //SDL_Log("Text size: %d x %d | text=\"%s\"", textW, textH, currentText.c_str());
    }

    void setColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255)
    {
        r = red; g = green; b = blue; a = alpha;
        if (textObj) TTF_SetTextColor(textObj, r, g, b, a);
    }

    void setPosition(float px, float py)
    {
        x = px;
        y = py;
    }

    void render()
    {
        if (textObj)
        {
            if (!TTF_DrawRendererText(textObj, x, y))
            {
                SDL_Log("Failed to draw text: %s", SDL_GetError());
            }
            else
            {
                /*SDL_Log("Rendering text \"%s\" at x=%.2f y=%.2f | size=%.2f x %.2f",
                    currentText.c_str(), x, y, getWidth(), getHeight());*/
            }
        }
    }

    // Accessors for cached width and height
    float getWidth() const { return static_cast<float>(textW); }
    float getHeight() const { return static_cast<float>(textH); }

    glm::vec2 getPosition() const { return { x, y }; }

    // Optional: access current text
    const std::string& getText() const { return currentText; }
};