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
    float x = 0, y = 0;
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;
    Uint8 a = 255;

public:
    UILabel(SDL_Renderer* ren, const std::string& fontPath, float fontSize)
        : renderer(ren)
    {
        // Initialize font
        font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
            SDL_Log("Failed to load font %s: %s", fontPath.c_str(), SDL_GetError());
            return;
        }

        // Create renderer text engine
        engine = TTF_CreateRendererTextEngine(renderer);
        if (!engine) {
            SDL_Log("Failed to create text engine: %s", SDL_GetError());
            return;
        }
    }

    ~UILabel() {
        if (textObj) TTF_DestroyText(textObj);
        if (engine) TTF_DestroyRendererTextEngine(engine);
        if (font) TTF_CloseFont(font);
    }

    void setText(const std::string& str) {
        currentText = str;

        if (textObj) TTF_DestroyText(textObj);

        // Create a TTF_Text object for this string
        textObj = TTF_CreateText(engine, font, str.c_str(), str.length());
        if (!textObj) {
            SDL_Log("Failed to create text object: %s", SDL_GetError());
        }

        // Set color
        TTF_SetTextColor(textObj, r, g, b, a);
    }

    void setColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255) {
        r = red; g = green; b = blue; a = alpha;
        if (textObj) TTF_SetTextColor(textObj, r, g, b, a);
    }

    void setPosition(float px, float py) { x = px; y = py; }

    void render() {
        if (textObj) {
            if (!TTF_DrawRendererText(textObj, x, y)) {
                SDL_Log("Failed to draw text: %s", SDL_GetError());
            }
        }
    }
};