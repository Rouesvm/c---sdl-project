#pragma once

#include "Vectors.hpp"

namespace Math {
    constexpr bool colliding(const float& a, const float& b, const float& sizeA, const float& sizeB) {
        return a + sizeA >= b && b + sizeB >= a;
    }

    template<typename VecA, typename VecB>
    inline bool isColliding(const VecA& a, const VecB& b, const VecA& sizeA, const VecB& sizeB) {
        return colliding(a.x, b.x, sizeA.x, sizeB.x) && colliding(a.y, b.y, sizeA.y, sizeB.y);
    }

    template<typename VecA, typename VecB>
    inline bool isColliding(const VecA& a, const VecB& b, const VecA& sizeA) {
        VecB defaultSize{1, 1};
        return isColliding(a, b, sizeA, defaultSize);
    }

    constexpr int floorDiv(const int a, const int b) {
        const int r = a % b;
        const int q = a / b;
        const int adjust = ((r != 0) & ((r ^ b) < 0));
        return q - adjust;
    }
    
    inline const Vector2f floorDiv(const Vector2f& a, const int b) {
        return {floorDiv(a.x, b), floorDiv(a.y, b)};
    }

    inline const Vector2i floorDiv(const Vector2i& a, const int b) {
        return {floorDiv(a.x, b), floorDiv(a.y, b)};
    }
}
