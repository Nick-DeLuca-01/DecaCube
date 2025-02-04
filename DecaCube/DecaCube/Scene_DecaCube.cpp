#include "Scene_DecaCube.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

void Scene_DecaCube::sMovement()
{
}

void Scene_DecaCube::sAnimation()
{
}

void Scene_DecaCube::sCollision()
{
}

void Scene_DecaCube::onEnd()
{
    _game->changeScene("MENU", nullptr, false);
}

void Scene_DecaCube::registerActions()
{
}

void Scene_DecaCube::spawnPlayer(sf::Vector2f pos)
{
}

void Scene_DecaCube::playerMovement()
{
}

void Scene_DecaCube::adjustPlayerPosition()
{
}

void Scene_DecaCube::init(const std::string& path)
{
    loadLevel(path);
}

void Scene_DecaCube::loadLevel(const std::string& path)
{
    _entityManager = EntityManager();
    loadFromFile(path);

}

void Scene_DecaCube::loadFromFile(const std::string& path)
{
}

Vec2 Scene_DecaCube::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    float x = 0.f + gridX * gridSize.x;
    float y = 0.f - gridY * gridSize.y;

    sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getBB();

    return Vec2(x + spriteSize.x / 2.f, y - spriteSize.y / 2.f);
}

Scene_DecaCube::Scene_DecaCube(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()) {

}

void Scene_DecaCube::update(sf::Time dt)
{
}

void Scene_DecaCube::sDoAction(const Command& command)
{
}

void Scene_DecaCube::sRender()
{
}
