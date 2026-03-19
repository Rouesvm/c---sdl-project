#pragma once

#include <array>
#include <vector>
#include <unordered_map>

#include "math/Vectors.hpp"

#include "world/objects/Tile.hpp"
#include "world/objects/Machine.hpp"

#include "management/RecipeManager.hpp"

class RenderContext;
class Renderer;
class World {
    public:
        int estimated_rendered_tiles = 0;
        int ticking_machines = 0;
    private:
        std::unordered_map<Vector2i, Tile> tiles{};
        std::unordered_map<Vector2i, Machine> machines{};
    private:
        std::unordered_map<std::string, MachineLogic> machine_logic{};
        std::vector<TileSetting> tile_settings{};
    private:
        RecipeManager recipe_manager{};
    public:
        std::array<uint8_t, 128> textures;
    public:
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
        const TileSetting& getTileSetting(int id) const {
            int tileSettingsSize = static_cast<int>(tile_settings.size());
            return id < tileSettingsSize ? tile_settings[id] : tile_settings[0];
        }
    public:
        RecipeManager& recipeManager() {
            return recipe_manager;
        }
    public:
        Machine* getMachine(const Vector2i& position);
        const Machine* getConstMachine(const Vector2i& position);
    public:
        Tile* getTile(const Vector2i& position);
        Tile* getMainTile(Vector2i* position);
    public:
        void addTile(const Vector2i& position, Tile tile);
        void removeTile(const Vector2i& position);
    private:
        bool tileConnects(Vector2i position, Vector2i myOut, int neighbourIndex, int tileId);
    public:
        void renderTile(Renderer& renderer, RenderContext& renderContext, const Vector2i& position);
        void render(Renderer& renderer);
        
        void update(double deltaTime);
};