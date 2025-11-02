#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Resources.h"
#include "SDLState.h"
#include "entities/Obstacle.h"
#include "entities/GameObject.h"

namespace BiomeName
{
    constexpr const char* Test_Level = "Test_Level";
}

struct ObstacleData
{
    std::unique_ptr<Obstacle> obj; // pointer instead of value
    std::string path;

    ObstacleData(const Resources& res) : obj(std::make_unique<Obstacle>(res)) {}
};

struct FloorData
{
    GameObject obj;
    std::string path;
};

struct Biome {
    std::string name;
    std::string background;
    std::string parallaxBackgrounds;
    bool isTransition;

    std::unordered_map<int, Obstacle> tripped;
    std::unordered_map<int, Obstacle> wall;
    std::unordered_map<int, Obstacle> burnt;
    std::unordered_map<int, Obstacle> spike;
    std::unordered_map<int, GameObject> floor;

    Biome()
        : name("Swamp"),
        background("data/backgrounds/" + name + "/bg.png"),
        isTransition(false)
    {
    }

    Biome(std::string n, std::string bg, bool i)
        : name(n), background(bg), isTransition(i)
    {
        loadBiome(n);
    }

    void readObstacles(std::string s, ObstacleType t)
    {
        std::istringstream iss(s);

        int i;
        Obstacle o;
        glm::vec2 size;
        glm::vec2 offset;

        iss >> i >> size.x >> size.y >> offset.x >> offset.y;

        o.setImageSize(size);
        o.setImageOffset(offset);
        o.clearCollider();

        SDL_FRect rect;
        while (iss >> rect.x >> rect.y >> rect.w >> rect.h) // keep reading!
        {
            o.addCollider(rect);
        }

        o.setObstacleType(t);

        if (t == ObstacleType::tripped)      tripped[i] = o;
        else if (t == ObstacleType::wall)    wall[i] = o;
        else if (t == ObstacleType::burnt)   burnt[i] = o;
        else if (t == ObstacleType::spike)   spike[i] = o;
    }

    void readFloorTiles(std::string s)
    {
        std::istringstream iss(s);

        int i;
        GameObject o;
        glm::vec2 size;
        glm::vec2 offset;
        std::vector<SDL_FRect> rects;

        //iss >> i >> size.x >> size.y >> offset.x >> offset.y >> rect.x >> rect.y >> rect.w >> rect.h;

        iss >> i >> size.x >> size.y >> offset.x >> offset.y;

        o.setImageSize(size);
        o.setImageOffset(offset);
        o.clearCollider();

        SDL_FRect rect;
        while (iss >> rect.x >> rect.y >> rect.w >> rect.h)
        {
            o.addCollider(rect);
        }

        floor[i] = o;
    }

    void loadBiome(std::string name)
    {
        // Clear all the previous objects in the maps
        tripped = {};
        wall = {};
        burnt = {};
        spike = {};
        floor = {};

        // Create the path based on the Biome name
        std::string path = "data/biomes/" + name + ".txt";

        // Open the file
        std::ifstream fin;
        fin.open(path);
        if (!fin)
        {
            std::cerr << "Error: could not open biome file " << path << "\n";
            return;
        }

        // Iterate through each line of the file
        std::string line;
        while (std::getline(fin, line))
        {
            // Check whether the line is empty
            if (line.empty())
            {
                continue;
            }

            // Name
            if (line.find("name=") == 0)
            {
                name = line.substr(5);
            }

            // Background path
            if (line.find("background=") == 0)
            {
                background = line.substr(11);
            }

            // Is it a transitional biome 
            if (line.find("isTransition=") == 0)
            {
                std::string value = line.substr(13);
                isTransition = (value == "1");
            }

            // Store the parallax backgrounds
            if (line.find("parallaxBackgrounds") == 0)
            {
                parallaxBackgrounds = line.substr(20);
            }

            // Tripped obstacles
            if (line.find("[tripped]") == 0)
            {
                while (std::getline(fin, line) && line != "")
                {
                    readObstacles(line, ObstacleType::tripped);
                }
            }

            // Wall obstacles
            if (line.find("[wall]") == 0)
            {
                while (std::getline(fin, line) && line != "")
                {
                    readObstacles(line, ObstacleType::wall);
                }
            }

            // Burnt obstacles
            if (line.find("[burnt]") == 0)
            {
                while (std::getline(fin, line) && line != "")
                {
                    readObstacles(line, ObstacleType::burnt);
                }
            }

            // Spike obstacles
            if (line.find("[spike]") == 0)
            {
                while (std::getline(fin, line) && line != "")
                {
                    readObstacles(line, ObstacleType::spike);
                }
            }

            // Floor tiles
            if (line.find("[floor]") == 0)
            {
                while (std::getline(fin, line) && line != "")
                {
                    readFloorTiles(line);
                }
            }

            // Check whether this is the ending line
            if (line == "end")
            {
                return;
            }
        }
    }

    void loadTextures(Resources& res, SDL_Renderer* renderer) 
    {
        // Load all the textures
        for (auto& [id, obj] : tripped) 
        {
            obj.setTexture(res.loadTexture(renderer, "data/textures/biomes/" + name + "/" + std::to_string(id) + ".png"));
        }
        for (auto& [id, obj] : wall) 
        {
            obj.setTexture(res.loadTexture(renderer, "data/textures/biomes/" + name + "/" + std::to_string(id) + ".png"));
        }
        for (auto& [id, obj] : burnt) 
        {
            obj.setTexture(res.loadTexture(renderer, "data/textures/biomes/" + name + "/" + std::to_string(id) + ".png"));
        }
        for (auto& [id, obj] : floor) 
        {
            obj.setTexture(res.loadTexture(renderer, "data/textures/biomes/" + name + "/" + std::to_string(id) + ".png"));
        }
    }
};