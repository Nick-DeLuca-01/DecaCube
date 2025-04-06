#pragma once

#include "Scene.h"
#include <string>
#include <fstream>

class Scene_Scoreboard : public Scene
{
private:

	std::string _levelPath;

	sf::Text scoreA;
	sf::Text scoreB;
	sf::Text scoreC;
	sf::Text scoreD;
	sf::Text scoreE;

	std::map<int, std::string> scores;

	void init(std::string levelPath);
	void onEnd() override;

public:
	Scene_Scoreboard(GameEngine* gameEngine, const std::string& levelPath);
	void update(sf::Time dt) override;
	void sDoAction(const Command& command) override;
	void sRender() override;
};

