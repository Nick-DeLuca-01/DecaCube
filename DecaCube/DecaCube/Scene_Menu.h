#pragma once

#include "Scene.h"

class Scene_Menu : public Scene
{
private:
	std::vector<std::string>	m_menuStrings;
	sf::Text					m_menuText;
	std::vector<std::string>	m_levelPaths;
	int							m_menuIndex{ 0 };
	std::string					m_title;

	int currentTip = 0;

	std::string tip = "Collect all 10 items and return to the beginning!";
	std::vector<std::string> tips;

	std::string tipsPath = "../tips.txt";

	void init();
	void getTips();
	void pickRandomTip();
	void onEnd() override;
public:

	Scene_Menu(GameEngine* gameEngine);

	void update(sf::Time dt) override;

	void sRender() override;
	void sDoAction(const Command& action) override;


};

