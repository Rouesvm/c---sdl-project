#include "world/objects/Tile.hpp"

class MachineLogic {
    public:
        bool check_tile = false;
    public:
        Resource current_tile;
        int amount_of_tile = 0;
    public:
        std::function<void(Machine&)> update;
        std::function<void(double, Machine&)> onPlace;
};

class DrillType : public MachineLogic {
};

class FurnaceType : public MachineLogic {
};