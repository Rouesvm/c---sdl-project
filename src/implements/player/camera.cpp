#include "Game.hpp"
#include "math/Math.hpp"
#include "player/Camera.hpp"

Camera::Camera() :
    position(),
    last_position(),
    screen_position()
{

}

void Camera::update(const Vector2i& windowSize, const Vector2f& playerPosition, double deltaTime) {
    last_position = position;
    
    const Vector2f& windowFloatSize = Math::toVector2f(windowSize);
    const Vector2f& targetPosition = (playerPosition * Game::TILE_SIZE_IN_PIXELS) - (windowFloatSize / 2) / zoom;

    position.x += (targetPosition.x - position.x) * smoothing * deltaTime;    
    position.y += (targetPosition.y - position.y) * smoothing * deltaTime;

    screen_position.x = (position.x);
    screen_position.y = (position.y);
}
