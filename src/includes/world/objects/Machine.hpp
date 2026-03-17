#include <functional>
#include <vector>

#include "math/Vectors.hpp"
#include "world/objects/Tile.hpp"

class Machine {
    public:
        std::vector<Resource> slots{};
        std::vector<TileIO> ios{};

        int inventory_size = 1;
        double ticks = 0;
        
        bool active = false;

        Vector2i position{};
    public:
        Machine(int inventorySize): slots(inventorySize) {};
    public:
        std::function<void(double, World&, Machine&)> update;
};