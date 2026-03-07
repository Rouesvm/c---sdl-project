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
    
    inline const Vector2f floorDiv(const Vector2f& a, const float b) {
        return {floorDiv(a.x, b), floorDiv(a.y, b)};
    }

    inline const Vector2i floorDiv(const Vector2i& a, const float b) {
        return {floorDiv(a.x, b), floorDiv(a.y, b)};
    }

    inline const Vector2f toVector2f(const Vector2i& position) {
        return Vector2f{
            static_cast<float>(position.x),
            static_cast<float>(position.y)
        };
    }

    inline const Vector2i toRoundedVector2i(const Vector2f& position) {
        return Vector2i{
            static_cast<int>(std::round(position.x)),
            static_cast<int>(std::round(position.y))
        };
    }

    inline const Vector2i toVector2i(const Vector2f& position) {
        return Vector2i{
            static_cast<int>((position.x)),
            static_cast<int>((position.y))
        };
    }
}
