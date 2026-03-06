#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#include "math/Vectors.hpp"
#include "world/objects/Tile.hpp"

class Renderer;
class World {
    private:
        std::unordered_map<Vector2i, Tile> tiles{};
        std::vector<bool> is_machine{};
    public:
        std::array<int, 128> textures{};
    public:
        World();
    public:
        void addTile(const Vector2i& position, Tile tile);
        void removeTile(const Vector2i& position);

        void render(Renderer& renderer);
        void update();
};