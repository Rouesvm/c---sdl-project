#pragma once

#include <array>
#include <vector>
#include <unordered_map>

#include "math/Vectors.hpp"
#include "world/objects/Tile.hpp"

class Renderer;
class World {
    private:
        std::unordered_map<Vector2i, Tile> tiles{};
    private:
        std::vector<TileSettings> tile_settings{};
        std::array<uint8_t, 128> textures{};
    public:
        World();
    public:
        void addTile(const Vector2i& position, Tile tile);
        void removeTile(const Vector2i& position);

        void render(Renderer& renderer);
        void update();
};