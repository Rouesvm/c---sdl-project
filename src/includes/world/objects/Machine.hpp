#include <functional>
#include <vector>

#include "math/Vectors.hpp"
#include "world/objects/Tile.hpp"

class Machine {
    public:
        std::vector<Resource> slots{};
    public:
        Vector2i position{};
        int id = 0;
        int inventory_size = 1;
    public:
        double ticks = 0;
        bool active = false;
    public:
        Machine(int inventorySize): slots(inventorySize) {};
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