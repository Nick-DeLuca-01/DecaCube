#include "Scene_CubeLeft.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

#include <cmath>

void Scene_CubeLeft::onEnd()
{
}

void Scene_CubeLeft::init(const std::string& path)
{
	loadLevel(path);
	registerActions();
}

void Scene_CubeLeft::registerActions()
{
}

void Scene_CubeLeft::spawnPlayer(sf::Vector2f pos)
{
}

void Scene_CubeLeft::loadLevel(const std::string& path)
{
	_entityManager = EntityManager();
	loadFromFile(path);
}

Vec2 Scene_CubeLeft::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float x = 0.f + gridX * gridSize.x;
	float y = 440.f - gridY * gridSize.y;

	sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getBB();

	return Vec2(x + spriteSize.x / 2.f, y - spriteSize.y / 2.f);
}

void Scene_CubeLeft::loadFromFile(const std::string& path)
{
	std::ifstream config(path);
	if (config.fail()) {
		std::cerr << "Failed to open file " << path << ".\n";
		config.close();
		exit(1);
	}

	std::string token = "";
	config >> token;
	while (config) {
		if (token == "Tile") {
			std::string name;
			float gx, gy;
			config >> name >> gx >> gy;

			auto e = _entityManager.addEntity("tile");
			auto bb = e->addComponent<CAnimation>(Assets::getInstance().getAnimation(name)).animation.getBB();
			e->addComponent<CBoundingBox>(bb);
			e->addComponent<CTransform>(gridToMidPixel(gx, gy, e));
			e->addComponent<CState>(name);
		}
		else
		{
			std::string buffer;
			std::getline(config, buffer);
		}
		config >> token;
	}
	_player = _entityManager.addEntity("robert");
	auto bb = _player->addComponent<CAnimation>(Assets::getInstance().getAnimation("Robert")).animation.getBB();
	_player->addComponent<CBoundingBox>(bb);
	auto pixelPos = gridToMidPixel(_playerSpawn.x, _playerSpawn.y, _player);
	_player->addComponent<CTransform>(pixelPos);
	_player->addComponent<CState>("alive");
	_player->addComponent<CInput>();

	std::cout << "DONE READING";
}

Scene_CubeLeft::Scene_CubeLeft(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) 
{
	init(_levelPath);
}

void Scene_CubeLeft::update(sf::Time dt)
{
	_entityManager.update();
	sRender();
}

void Scene_CubeLeft::sDoAction(const Command& command)
{
}

void Scene_CubeLeft::sRender()
{
	for (auto e : _entityManager.getEntities("tile")) {
		//render all tiles first
		auto& tfm = e->getComponent<CTransform>();
		auto& anim = e->getComponent<CAnimation>().animation;

		anim.getSprite().setRotation(tfm.angle);
		anim.getSprite().setPosition(tfm.pos.x, tfm.pos.y);
		_game->window().draw(anim.getSprite());

	}
	for (auto e : _entityManager.getEntities("robert")) {
		//render player
		auto& tfm = e->getComponent<CTransform>();
		auto& anim = e->getComponent<CAnimation>().animation;

		anim.getSprite().setRotation(tfm.angle);
		anim.getSprite().setPosition(tfm.pos.x, tfm.pos.y);

		//std::cout << tfm.pos.x << " " << tfm.pos.y << "\n";
		_game->window().draw(anim.getSprite());
	}
}
