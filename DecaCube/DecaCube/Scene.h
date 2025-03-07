#pragma once

#include "EntityManager.h"
#include "GameEngine.h"
#include "Command.h"
#include <map>
#include <string>
#include <vector>

struct PlayerData {
	int score = 0;
	Vec2 spawnPos{ 5, 5 };
	int lives = 3;
	bool sceneChanged = false;
	std::vector<std::string> collectedItems;
	int faceRotation = 0; //up = up: 0, left = up: 1, down = up: 2, right = up: 3
	sf::Time elapsedTime = sf::Time::Zero;
};

struct LevelConfig {
	float       playerSpeed{ 80.f };
	float       enemySpeed{ 80.f };

};


using CommandMap = std::map<int, std::string>;

class Scene_Breakout;

class Scene
{

protected:

	GameEngine* _game;
	EntityManager	_entityManager;
	CommandMap		_commands;
	bool			_isPaused{ false };
	bool			_hasEnded{ false };
	size_t			_currentFrame{ 0 };
	static PlayerData		_playerData;

	virtual void	onEnd() = 0;
	void			setPaused(bool paused);

public:
	Scene(GameEngine* gameEngine);
	virtual ~Scene();

	virtual void		update(sf::Time dt) = 0;
	virtual void		sDoAction(const Command& action) = 0;
	virtual void		sRender() = 0;

	void				simulate(int);
	void				doAction(Command);
	void				registerAction(int, std::string);
	const CommandMap	getActionMap() const;
};

