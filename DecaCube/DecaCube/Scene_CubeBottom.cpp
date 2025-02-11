#include "Scene_CubeBottom.h"

void Scene_CubeBottom::onEnd()
{
}

Scene_CubeBottom::Scene_CubeBottom(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) {

}

void Scene_CubeBottom::update(sf::Time dt)
{
}

void Scene_CubeBottom::sDoAction(const Command& command)
{
}

void Scene_CubeBottom::sRender()
{
}
