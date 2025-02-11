#include "Scene_CubeRight.h"

void Scene_CubeRight::onEnd()
{
}

Scene_CubeRight::Scene_CubeRight(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) 
{
}

void Scene_CubeRight::update(sf::Time dt)
{
}

void Scene_CubeRight::sDoAction(const Command& command)
{
}

void Scene_CubeRight::sRender()
{
}
