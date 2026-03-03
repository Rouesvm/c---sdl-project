#pragma once

#include <unordered_map>

#include "math/Vectors.hpp"
#include "world/objects/Tile.hpp"

class Renderer;
class World {
    private:
        std::unordered_map<Vector2i, Tile> tiles{};
    public:
        void addTile(Vector2i& position, Tile tile);
        void removeTile(Vector2i& position);

        void render(Renderer& renderer);
        void update();
};