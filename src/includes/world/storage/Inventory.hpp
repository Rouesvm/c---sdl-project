#include <algorithm>
#include <cstdint>

struct Resource {
    uint8_t item_id = 0;
    uint8_t amount = 0;
    uint8_t max_size = 8;

    void add(uint8_t itemId, uint8_t addAmount) {
        if (amount >= max_size) return;

        if (amount == 0) {
            item_id = itemId;
        }

        if (item_id == itemId) {
            uint16_t sum = amount + addAmount;
            amount = static_cast<uint8_t>(std::min<uint16_t>(max_size, sum));
        }
    }

    void remove(uint8_t removeAmount) {
        amount -= std::min(amount, removeAmount);
    }
};
