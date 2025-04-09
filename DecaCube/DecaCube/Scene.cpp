#include "Scene.h"
#include "MusicPlayer.h"


Scene::Scene(GameEngine* gameEngine) : _game(gameEngine)
{}

Scene::~Scene()
{}

PlayerData Scene::_playerData;
EnemyData Scene::_enemyData;

void Scene::setPaused(bool paused)
{
	_isPaused = paused;
}


void Scene::simulate(int)
{}

void Scene::doAction(Command command)
{
	this->sDoAction(command);
}


const CommandMap Scene::getActionMap() const
{
	return _commands;
}

void Scene::registerAction(int inputKey, std::string command)
{
	_commands[inputKey] = command;
}

void Scene::changeMusic()
{
	int collectedItems = _playerData.collectedItems.size();

	switch (collectedItems) {
	case 2:
	case 3:
		changeMusic("danger1");
		break;
	case 4:
	case 5:
		changeMusic("danger2");
		break;
	case 6:
	case 7:
		changeMusic("danger3");
		break;
	case 8:
	case 9:
		changeMusic("danger4");
		break;
	case 10:
		changeMusic("danger5");
		break;
	}
}

void Scene::changeMusic(std::string songName)
{
	sf::Time playingOffset = MusicPlayer::getInstance().getPlayingOffset();
	MusicPlayer::getInstance().stop();
	MusicPlayer::getInstance().play(songName);
	MusicPlayer::getInstance().setPlayingOffset(playingOffset);
	MusicPlayer::getInstance().setVolume(80);
}
