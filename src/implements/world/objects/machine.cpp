#include "world/World.hpp"
#include "world/objects/Tile.hpp"

bool MachineIO::insertResource(Resource &extract, Resource &insert, int amount) {
    if (extract.amount <= 0) return false;
    if (insert.amount >= insert.max_size) return false;

    insert.add(extract.item_id, amount);
    extract.remove(amount);

    return true;
}

bool MachineIO::insert(World &world, Machine &extract, bool amount) {
    const TileSetting& setting = world.getTileSetting(extract.tile.id);
    const Vector2i offsetedPosition = extract.position;

    for (const TileIO& io : setting.ios) {
        SIDE rotation = io.side;
        TYPE type = io.type;
        Vector2i ioOffset{io.x, io.y};

        if (type != TYPE::OUTPUT) continue;

        ioOffset = extract.position.add(ioOffset);
        if (offsetedPosition != ioOffset) continue;
        const Vector2i& rotationOffset = direction(static_cast<int>(rotation));      
        ioOffset = ioOffset.add(rotationOffset);

        Machine* insert = world.getMachine(ioOffset);
        if (!insert) continue;
        const TileSetting& insertSetting = world.getTileSetting(insert->tile.id);
        if (insertSetting.inventory_size == 1) 
            MachineIO::insertResource(extract.slots[io.slot], insert->slots.front(), amount);
    }

    return true;
}