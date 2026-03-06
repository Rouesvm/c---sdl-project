#pragma once

#include "math/Vectors.hpp"
#include <cstdint>
#include <vector>

struct Tile {
    uint16_t id = 0;
    int8_t offset = 0;

    bool isAir() const {
        return !isSolid();
    }

    bool isSolid() const {
        return isMainTile() || isMultiTile();
    }

    bool isMainTile() const {
        return id != 0 && offset == 0;
    }

    bool isMultiTile() const {
        return id == 0 && offset != 0;
    }

    Vector2i getOffset() const {
        if (!isMultiTile())
            return {0, 0};
        
        int8_t x = (int8_t)(offset & 0xF);
        int8_t y = (int8_t)((offset >> 4) & 0xF);

        if (x >= 8) x -= 16;
        if (y >= 8) y -= 16;
        return { x, y };
    }

    Vector2i getMainTile(const Vector2i& pos) const {
        Vector2i offset = getOffset();
        return { pos.x - offset.x, pos.y - offset.y };
    }

    void setMainTile(const Vector2i& main, const Vector2i& position) {
        int8_t dx = position.x - main.x;
        int8_t dy = position.y - main.y;
        offset = ((dy & 0xF) << 4) | (dx & 0xF);
    }
};

enum struct SIDE {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

enum struct TYPE {
    INPUT = 0,
    OUTPUT = 1
};

struct TileIO {
    TYPE type;
    SIDE side;
    uint8_t x = 0;
    uint8_t y = 0;
};

struct TileSettings {
    bool is_machine = false;
    bool is_multi_tiled = false;

    uint8_t size_x = 1;
    uint8_t size_y = 1;

    uint8_t inventory_size = 0;

    std::vector<TileIO> ios{};
};