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
	int lives = 5;
	bool sceneChanged = false;
	std::vector<std::string> collectedItems;
	int faceRotation = 0; //up = up: 0, left = up: 1, down = up: 2, right = up: 3
	sf::Time elapsedTime = sf::Time::Zero;
	sf::Time invincibility = sf::Time::Zero;
};

struct EnemyData {
	EntityManager enemyManager;
	bool enemiesLoaded = false;
	bool flip = false;
};

struct LevelConfig {
	float       playerSpeed{ 80.f };
	float       enemySpeed{ 60.f };
	float		sceneChangeLower{ 15.f };
	float		sceneChangeUpper{ 30.f };
	sf::Time	midDiffTime{ sf::seconds(150.f) };
	sf::Time	highDiffTime{ sf::seconds(300.f) };
	int			midDiffItems{ 5 };
	int			highDiffItems{ 10 };
	sf::Time	gunnerCDLow{ sf::seconds(5.f) };
	sf::Time	gunnerCDMid{ sf::seconds(3.f) };
	sf::Time	gunnerCDHigh{ sf::seconds(1.f) };
	sf::Time    sunMoonRememberLow{ sf::seconds(5.f) };
	sf::Time	sunMoonRememberMid{ sf::seconds(7.f) };
	sf::Time	sunMoonRememberHigh{ sf::seconds(10.f) };
	sf::Time	chargerCDLow{ sf::seconds(2.f) };
	sf::Time	chargerCDMid{ sf::seconds(1.f) };
	sf::Time	chargerCDHigh{ sf::seconds(0.5f) };
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
	static LevelConfig		_levelConfig;
	static EnemyData		_enemyData;

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
	void				changeMusic();
	void				changeMusic(std::string songName);
	void				startMusic(std::string songName);
	const CommandMap	getActionMap() const;
};

