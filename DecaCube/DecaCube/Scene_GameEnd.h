#pragma once

#include "Scene.h"
#include <string>
#include <fstream>

class Scene_GameEnd : public Scene
{
private:
	std::vector<char> initialsA;
	std::vector<char> initialsB;
	std::vector<char> initialsC;

	int optionIndexA = 0;
	int optionIndexB = 0;
	int optionIndexC = 0;

	int initialIndex = 0;

	sf::Text gameStatusText;

	sf::Text initialA;
	sf::Text initialB;
	sf::Text initialC;

	void init();
	void addScoreToScoreboard();
	void onEnd() override;

public:
	Scene_GameEnd(GameEngine* gameEngine);
	void update(sf::Time dt) override;
	void sDoAction(const Command& command) override;
	void sRender() override;
};

