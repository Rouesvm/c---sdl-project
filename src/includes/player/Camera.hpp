#pragma once

#include "math/Vectors.hpp"

class Camera {
    private:
        float smoothing = 1.5f;
    public:
        float zoom = 3.0f;
        
        Vector2f position;
        Vector2f last_position;
        
        Vector2f screen_position;
    public:
        Camera();
    public:
        void update(const Vector2i& windowSize, const Vector2f& playerPosition, double deltaTime);
};