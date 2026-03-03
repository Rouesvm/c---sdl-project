#include "Game.hpp"

#include "management/AssetManager.hpp"
#include "math/Vectors.hpp"
#include "renderer/Renderer.hpp"

#include "world/World.hpp"

void World::addTile(Vector2i& position, Tile tile) {
    if (tiles.find(position) != tiles.end()) 
        return;
    tiles.emplace(position, std::move(tile));
}  

void World::removeTile(Vector2i& position) {
    if (tiles.find(position) == tiles.end())
        return;
    tiles.emplace(position, Tile{0});
}

void World::render(Renderer& renderer) {
    float zoom = 4.0f;

    Vector2f tileSize{8, 8};
    Vector2f tilePosition{};
    for (const auto&[position, block] : tiles) {
        const AssetManager& manager = Game::assetManager();

        tilePosition.x = position.x;
        tilePosition.y = position.y;

        renderer.renderTexture(manager.getTexture("dirt"), (tilePosition * tileSize) * zoom, tileSize * zoom);
    }
}   

void World::update() {

}