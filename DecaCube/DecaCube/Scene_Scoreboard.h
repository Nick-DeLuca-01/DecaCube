#pragma once

#include "Scene.h"
#include <string>
#include <fstream>

class Scene_Scoreboard : public Scene
{
private:

	std::string _levelPath;

	std::vector<std::pair<int, std::string>> scores;

	void init(std::string levelPath);
	void onEnd() override;
	void loadFromFile(std::string levelPath);

public:
	Scene_Scoreboard(GameEngine* gameEngine, const std::string& levelPath);
	void update(sf::Time dt) override;
	void sDoAction(const Command& command) override;
	void sRender() override;
};

