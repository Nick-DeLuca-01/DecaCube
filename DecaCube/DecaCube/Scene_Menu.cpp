#include "Scene_Menu.h"
#include "Scene_DecaCube.h"
#include "Scene_Scoreboard.h"
#include "MusicPlayer.h"
#include <memory>
#include <random>
void Scene_Menu::onEnd()
{
	_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}

void Scene_Menu::init()
{
	MusicPlayer::getInstance().play("menuTheme");
	MusicPlayer::getInstance().setVolume(80);

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Right, "PLAY");
	registerAction(sf::Keyboard::A, "CYCLE");
	registerAction(sf::Keyboard::Left, "CYCLE");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "Deca-Cube";
	m_menuStrings.push_back("Play Game");
	m_menuStrings.push_back("Scoreboard");
	m_menuStrings.push_back("Exit Game");

	m_levelPaths.push_back("../assets/cubetop.txt");
	m_levelPaths.push_back("../assets/scoreboard.txt");
	m_levelPaths.push_back("../exit.txt");

	m_menuText.setFont(Assets::getInstance().getFont("main"));

	const size_t CHAR_SIZE{ 48 };
	m_menuText.setCharacterSize(CHAR_SIZE);

	getTips();
}

void Scene_Menu::getTips()
{
	std::ifstream tipsFile(tipsPath);
	if (tipsFile.fail()) {
		std::cerr << "Failed to open file " << tipsPath << "\n";
		tipsFile.close();
		exit(1);
	}
	std::string token = "";
	tipsFile >> token;
	while (tipsFile) {
		if (token == "TIP") {
			std::string newTip;
			std::getline(tipsFile, newTip);
			tips.push_back(newTip);
		}
		else {
			std::string buffer;
			std::getline(tipsFile, buffer);
		}
		tipsFile >> token;
	}
	tipsFile.close();
}

void Scene_Menu::pickRandomTip()
{
	std::uniform_int_distribution<int> tipIndex(0, tips.size() - 1);
	int newTip = tipIndex(rng);
	while (newTip == currentTip) {
		newTip = tipIndex(rng);
	}
	tip = tips[newTip];
	currentTip = newTip;
}

void Scene_Menu::update(sf::Time dt)
{
	_entityManager.update();
}


void Scene_Menu::sRender()
{

	sf::View view = _game->window().getView();
	view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
	_game->window().setView(view);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(255, 0, 0);

	sf::Text footer("UP: W   DOWN: S   PLAY: D   CYCLE TIPS: A   QUIT: ESC",
		Assets::getInstance().getFont("main"), 16);
	footer.setFillColor(normalColor);
	footer.setPosition(20, 455);


	m_menuText.setFillColor(normalColor);
	m_menuText.setString(m_title);
	m_menuText.setPosition(10, 10);
	
	_game->window().draw(m_menuText);

	for (size_t i{ 0 }; i < m_menuStrings.size(); ++i)
	{
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		m_menuText.setPosition(32, 32 + (i + 1) * 64);
		m_menuText.setString(m_menuStrings.at(i));
		_game->window().draw(m_menuText);
	}

	sf::Text currentTip(tip, Assets::getInstance().getFont("main"), 18);
	currentTip.setFillColor(normalColor);
	currentTip.setPosition(10, 360);
	_game->window().draw(currentTip);

	_game->window().draw(footer);
}


void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuStrings.size() - 1) % m_menuStrings.size();
		}
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			if (m_menuIndex == 0) {
				_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, m_levelPaths[m_menuIndex]));
				MusicPlayer::getInstance().play("danger0");
				MusicPlayer::getInstance().setVolume(80);
			}
			else if (m_menuIndex == 1) {
				_game->changeScene("SCOREBOARD", std::make_shared<Scene_Scoreboard>(_game, m_levelPaths[m_menuIndex]));
			}
			else if (m_menuIndex == 2) {
				onEnd();
			}
			
		}
		else if (action.name() == "CYCLE") {
			pickRandomTip();
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}

}
