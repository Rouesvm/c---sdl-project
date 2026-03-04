#pragma once

#include "math/Vectors.hpp"
#include <cstdint>

struct Tile {
    uint16_t id;
    int8_t offset;

    bool isMultiTile() const {
        return id == 0 && offset != 0;
    }

    static Vector2i getMainTile(const Tile& t, const Vector2i& pos) {
        int8_t x = t.offset & 0xF;
        int8_t y = (t.offset >> 4) & 0xF;
        return { pos.x - x, pos.y - y };
    }

    void setMainTile(const Vector2i& main, const Vector2i& position) {
        int8_t dx = position.x - main.x;
        int8_t dy = position.y - main.y;
        offset = (dy << 4) | (dx & 0xF);
    }
};