#include "Scene_CubeLeft.h"

void Scene_CubeLeft::onEnd()
{
}

Scene_CubeLeft::Scene_CubeLeft(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) 
{
}

void Scene_CubeLeft::update(sf::Time dt)
{
}

void Scene_CubeLeft::sDoAction(const Command& command)
{
}

void Scene_CubeLeft::sRender()
{
}
