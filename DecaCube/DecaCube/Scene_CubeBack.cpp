#include "Scene_CubeBack.h"

void Scene_CubeBack::onEnd()
{
}

Scene_CubeBack::Scene_CubeBack(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) {

}

void Scene_CubeBack::update(sf::Time dt)
{
}

void Scene_CubeBack::sDoAction(const Command& command)
{
}

void Scene_CubeBack::sRender()
{
}
