#include "Scene_Scoreboard.h"

void Scene_Scoreboard::init(std::string levelPath)
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	loadFromFile(levelPath);
}

void Scene_Scoreboard::onEnd()
{
	_game->changeScene("MENU", nullptr, true);
}

void Scene_Scoreboard::loadFromFile(std::string levelPath)
{
	scores.clear();
	std::ifstream scoreboard(levelPath);
	if (scoreboard.fail()) {
		std::cerr << "Failed to open file " << levelPath << "\n";
		scoreboard.close();
		exit(1);
	}
	while (scoreboard) {
		int points;
		std::string name;

		scoreboard >> points >> name;
		std::pair<int, std::string> score;
		score.first = points;
		score.second = name;

		scores.push_back(score);

		std::string buffer;
		std::getline(scoreboard, buffer);
	}

	std::sort(scores.begin(), scores.end(), [](auto& left, auto& right) {
		return left.first > right.first;
	});

	//for (int i = 0; i < scores.size(); i++) {
	//	std::cout << scores[i].first << " " << scores[i].second << "\n";
	//}
	scoreboard.close();
}

Scene_Scoreboard::Scene_Scoreboard(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _levelPath(levelPath)
{
	init(_levelPath);
}

void Scene_Scoreboard::update(sf::Time dt)
{
	_entityManager.update();
}

void Scene_Scoreboard::sDoAction(const Command& command)
{
	if (command.type() == "START") {
		if (command.name() == "QUIT") {
			onEnd();
		}
	}
}

void Scene_Scoreboard::sRender()
{
	sf::View view = _game->window().getView();
	view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
	_game->window().setView(view);

	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(255, 0, 0);

	std::string headerText = "TOP SCORES";
	std::string footerText = "GO BACK: ESC";

	for (int i = 0; i < 10; i++) {
		std::string text;
		sf::Text score;
		if (i < scores.size()) {
			text = std::to_string(scores[i].first) + "\t\t\t\t" + scores[i].second;
		}
		else {
			text = "-- NO SCORE YET --";
		}
		score.setString(text);
		score.setFont(Assets::getInstance().getFont("main"));
		score.setCharacterSize(40);
		score.setPosition(32, (i * 40) + 60);
		score.setFillColor(normalColor);
		
		_game->window().draw(score);
	}

	sf::Text header(headerText,
		Assets::getInstance().getFont("main"), 40);
	header.setFillColor(normalColor);
	header.setPosition(10, 10);

	sf::Text footer(footerText,
		Assets::getInstance().getFont("main"), 12);
	footer.setFillColor(normalColor);
	footer.setPosition(32, 460);

	_game->window().draw(header);
	_game->window().draw(footer);
}
