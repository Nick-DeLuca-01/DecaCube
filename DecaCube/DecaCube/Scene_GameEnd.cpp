#include "Scene_GameEnd.h"

void Scene_GameEnd::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::Space, "SUBMIT");

	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < 26; i++) {
		initialsA.push_back(alphabet[i]);
		initialsB.push_back(alphabet[i]);
		initialsC.push_back(alphabet[i]);
	}
}

void Scene_GameEnd::addScoreToScoreboard()
{
	std::string scoreBoardPath = "../assets/scoreboard.txt";

	std::ofstream ofs;
	std::ifstream ifs;
	ifs.open(scoreBoardPath);
	if (!ifs) {
		std::cout << "File opening failed!";
		ifs.close();
	}
	else {
		ifs.close();
		ofs.open(scoreBoardPath, std::ios::app);
		ofs << "\n" << _playerData.score << "\t" << initialsA[optionIndexA] << initialsB[optionIndexB] << initialsC[optionIndexC];
		ofs.close();
	}
}

void Scene_GameEnd::onEnd()
{
	_playerData.lives = 5;
	_playerData.sceneChanged = false;
	_playerData.score = 0;
	_playerData.collectedItems.clear();
	_playerData.spawnPos = { 5, 5 };
	_playerData.faceRotation = 0;
	_playerData.elapsedTime = sf::Time::Zero;
	_playerData.invincibility = sf::Time::Zero;
	_enemyData.enemiesLoaded = false;
	_enemyData.flip = false;
	_game->changeScene("MENU", nullptr, false);
}

Scene_GameEnd::Scene_GameEnd(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_GameEnd::update(sf::Time dt)
{
	_entityManager.update();
}

void Scene_GameEnd::sDoAction(const Command& command)
{
	if (command.type() == "START") {
		if (_playerData.lives != 0) {

			if (command.name() == "RIGHT") {
				initialIndex = (initialIndex + 1) % 3;
			}
			if (command.name() == "LEFT") {
				initialIndex = (initialIndex + 2) % 3;
			}
			if (command.name() == "UP") {
				switch (initialIndex) {
				case 0:
					optionIndexA = (optionIndexA + 25) % 26;
					break;
				case 1:
					optionIndexB = (optionIndexB + 25) % 26;
					break;
				case 2:
					optionIndexC = (optionIndexC + 25) % 26;
					break;
				}
			}
			if (command.name() == "DOWN") {
				switch (initialIndex) {
				case 0:
					optionIndexA = (optionIndexA + 1) % 26;
					break;
				case 1:
					optionIndexB = (optionIndexB + 1) % 26;
					break;
				case 2:
					optionIndexC = (optionIndexC + 1) % 26;
					break;
				}
			}
			if (command.name() == "SUBMIT") {
				addScoreToScoreboard();
				onEnd();
			}
		}
		else {
			if (command.name() == "SUBMIT") {
				onEnd();
			}
		}
	}

}

void Scene_GameEnd::sRender()
{
	sf::View view = _game->window().getView();
	view.setCenter(_game->window().getSize().x / 2.f, _game->window().getSize().y / 2.f);
	_game->window().setView(view);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(0, 0, 0);

	static const sf::Color backgroundColor(255, 0, 0);

	std::string headerText;
	std::string footerText;

	if (_playerData.lives == 0) {
		headerText = "You lose...";
		footerText = "RETURN TO MAIN MENU: SPACE";
	}
	else {
		headerText = "Final score: " + std::to_string(_playerData.score);
		headerText = headerText + "\nEnter your initials!";

		footerText = "UP: W   LEFT: A   DOWN: S   RIGHT: D   SUBMIT SCORE: SPACE";

		initialA.setString(initialsA[optionIndexA]);
		initialB.setString(initialsB[optionIndexB]);
		initialC.setString(initialsC[optionIndexC]);

		initialA.setFont(Assets::getInstance().getFont("main"));
		initialB.setFont(Assets::getInstance().getFont("main"));
		initialC.setFont(Assets::getInstance().getFont("main"));

		initialA.setCharacterSize(32);
		initialB.setCharacterSize(32);
		initialC.setCharacterSize(32);

		if (initialIndex == 0) {
			initialA.setFillColor(selectedColor);
		}
		else {
			initialA.setFillColor(normalColor);
		}
		if (initialIndex == 1) {
			initialB.setFillColor(selectedColor);
		}
		else {
			initialB.setFillColor(normalColor);
		}
		if (initialIndex == 2) {
			initialC.setFillColor(selectedColor);
		}
		else {
			initialC.setFillColor(normalColor);
		}

		initialA.setPosition(180, 220);
		initialB.setPosition(220, 220);
		initialC.setPosition(260, 220);

		_game->window().draw(initialA);
		_game->window().draw(initialB);
		_game->window().draw(initialC);
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
