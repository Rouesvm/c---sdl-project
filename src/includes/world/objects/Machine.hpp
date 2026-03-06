#include "world/objects/Tile.hpp"

#include <cstdint>
#include <functional>
#include <vector>

struct Slot {
    uint8_t item_id;
    uint8_t amount;
};

class Machine {
    private:
        std::vector<Slot> slots{};
        std::vector<TileIO> ios{};
        int inventory_size = 1;
        int ticks = 0;
        
        bool active = false;
    public:
        Machine(int inventory_size): slots(inventory_size) {};
    public:
        std::function<void(double)> update;
};