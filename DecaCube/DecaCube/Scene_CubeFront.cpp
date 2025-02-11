#include "Scene_CubeFront.h"

void Scene_CubeFront::onEnd()
{
}

Scene_CubeFront::Scene_CubeFront(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) 
{
}

void Scene_CubeFront::update(sf::Time dt)
{
}

void Scene_CubeFront::sDoAction(const Command& command)
{
}

void Scene_CubeFront::sRender()
{
}
