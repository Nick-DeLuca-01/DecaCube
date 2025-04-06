#include "Scene_Scoreboard.h"

void Scene_Scoreboard::init(std::string levelPath)
{
}

void Scene_Scoreboard::onEnd()
{
}



Scene_Scoreboard::Scene_Scoreboard(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _levelPath(levelPath)
{
	init(_levelPath);
}

void Scene_Scoreboard::update(sf::Time dt)
{
}

void Scene_Scoreboard::sDoAction(const Command& command)
{
}

void Scene_Scoreboard::sRender()
{
}
