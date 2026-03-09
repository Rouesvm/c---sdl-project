#pragma once

#include <array>
#include <vector>
#include <unordered_map>

#include "math/Vectors.hpp"

#include "world/objects/Tile.hpp"
#include "world/objects/Machine.hpp"

class RenderContext;
class Renderer;
class World {
    private:
        std::unordered_map<Vector2i, Tile> tiles{};
        std::unordered_map<Vector2i, Machine> machines{};
    private:
        std::vector<TileSettings> tile_settings{};
    public:
        std::array<uint8_t, 128> textures{};

        static constexpr Vector2i TBLR_SRC_POSITIONS[16]{
            Vector2i(0,3), Vector2i(0,2), Vector2i(0,0), Vector2i(0,1),
            Vector2i(3,3), Vector2i(3,2), Vector2i(3,0), Vector2i(3,1),
            Vector2i(1,3), Vector2i(1,2), Vector2i(1,0), Vector2i(1,1),
            Vector2i(2,3), Vector2i(2,2), Vector2i(2,0), Vector2i(2,1)
        };

        static constexpr Vector2i CONVEYOR_SRC_POSITIONS[16]{
            Vector2i(0,3), Vector2i(0,3), Vector2i(1,3), Vector2i(0,3),
            Vector2i(2,3), Vector2i(1,1), Vector2i(1,0), Vector2i(3,1),
            Vector2i(3,3), Vector2i(0,1), Vector2i(0,0), Vector2i(1,1),
            Vector2i(3,3), Vector2i(0,3), Vector2i(1,3), Vector2i(2,1)
        };
    public:
        World();
    public:
        void addTile(const Vector2i& position, Tile tile);
        void removeTile(const Vector2i& position);

        void renderTile(Renderer& renderer, RenderContext& renderContext, const Vector2i& position);
        void render(Renderer& renderer);
        
        void update(double deltaTime);
};