#include "Game.hpp"

#include "management/AssetManager.hpp"
#include "math/Math.hpp"
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
    tiles[position] = Tile{0};
}

void World::render(Renderer& renderer) {
    Vector2f tileSize{Game::TILE_SIZE, Game::TILE_SIZE};
    Vector2f tilePosition{};

    const AssetManager& manager = Game::assetManager();

    const Vector2i& windowSizeInTiles = Math::floorDiv(Game::clientState().windowSize(), Game::TILE_SIZE);
    for (int x = 0; x < windowSizeInTiles.x; x++) {
        for (int y = 0; y < windowSizeInTiles.y; y++) {
            tilePosition.x = x;
            tilePosition.y = y;

            renderer.renderTexture(manager.getTexture("dirt"), (tilePosition * Game::TILE_SIZE) * renderer.zoom, tileSize * renderer.zoom);
        }
    }

    for (const auto&[position, block] : tiles) {
        if (block.id == 0) continue;

        tilePosition.x = position.x;
        tilePosition.y = position.y;

        const Texture* blockTexture = manager.getTexture("stone");
        renderer.renderTexture(blockTexture, (tilePosition * Game::TILE_SIZE) * renderer.zoom, tileSize * renderer.zoom);
    }
}   

void World::update() {

}