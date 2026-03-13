#include <functional>
#include <vector>

#include "world/objects/Tile.hpp"

class Machine {
    public:
        std::vector<Resource> slots{};
        std::vector<TileIO> ios{};

        int inventory_size = 1;
        double ticks = 0;
        
        bool active = false;
    public:
        Machine(int inventory_size): slots(inventory_size) {};
    public:
        std::function<void(double, Machine&)> update;
};