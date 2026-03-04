#pragma once

#include "math/Vectors.hpp"
#include <cstdint>

struct Tile {
    uint16_t id;
    int8_t offset;

    bool isSolid() const {
        return id != 0 && offset == 0;
    }

    bool isMultiTile() const {
        return id == 0 && offset != 0;
    }

    static Vector2i getMainTile(const Tile& t, const Vector2i& pos) {
        int8_t x = (int8_t)((t.offset << 4) >> 4);
        int8_t y = (int8_t)(t.offset >> 4);  
        return { pos.x - x, pos.y - y };
    }

    void setMainTile(const Vector2i& main, const Vector2i& position) {
        int8_t dx = position.x - main.x;
        int8_t dy = position.y - main.y;
        offset = ((dy & 0xF) << 4) | (dx & 0xF);
    }
};