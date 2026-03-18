#include <functional>
#include <vector>

#include "math/Vectors.hpp"
#include "world/objects/Tile.hpp"

class Machine {
    public:
        std::vector<Resource> slots{};
    public:
        Tile& tile;
    public:
        Vector2i position{};
        int inventory_size = 1;
    public:
        double ticks = 0;
        bool active = false;
    public:
        Machine(Tile& tileRef, int inventorySize): slots(inventorySize), tile(tileRef), inventory_size(inventorySize) {};
    public:
        std::function<void(double, World&, Machine&)> update{};
};

namespace MachineIO {
    static constexpr Vector2i DIR[4] = { 
        {0,-1}, {1,0}, {0,1}, {-1,0}
    };
    
    bool insertResource(Resource& extract, Resource& insert, int amount);
    bool insert(World& world, Machine& extract, bool amount);
};