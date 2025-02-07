#include "Scene_DecaCube.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

void Scene_DecaCube::sMovement(sf::Time dt)
{
	for (auto e : _entityManager.getEntities("robert")) {
		auto& tfm = e->getComponent<CTransform>();
		tfm.pos += tfm.vel * dt.asSeconds();
	}
	for (auto e : _entityManager.getEntities("enemy")) {

	}
}

void Scene_DecaCube::sAnimation()
{
}

void Scene_DecaCube::sCollision()
{
}

void Scene_DecaCube::onEnd()
{
	_game->changeScene("MENU", nullptr, false);
}

void Scene_DecaCube::registerActions()
{
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

}

void Scene_DecaCube::spawnPlayer(sf::Vector2f pos)
{
}

void Scene_DecaCube::playerMovement()
{
	if (_player->hasComponent<CState>() && _player->getComponent<CState>().state == "dead") {
		return;
	}
	Vec2 vel{ 0, 0 };
	auto& input = _player->getComponent<CInput>();
	if (input.left)
		vel.x -= 1;
	if (input.right)
		vel.x += 1;
	if (input.down)
		vel.y += 1;
	if (input.up)
		vel.y -= 1;
	
	_player->getComponent<CTransform>().vel = vel * _config.playerSpeed;
}

void Scene_DecaCube::adjustPlayerPosition()
{
	/*if (_player->getComponent<CState>().state == "dead")
		return;*/
}

void Scene_DecaCube::init(const std::string& path)
{
	loadLevel(path);
	registerActions();
}

void Scene_DecaCube::loadLevel(const std::string& path)
{
	_entityManager = EntityManager();
	loadFromFile(path);

}

void Scene_DecaCube::loadFromFile(const std::string& path)
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
		else if (token == "Player") {
			std::string name;
			float speed, lives, gx, gy;

			config >> name >> speed >> lives >> gx >> gy;

			_config.playerSpeed = speed;
			_lives = lives;

			_player = _entityManager.addEntity("robert");
			auto bb = _player->addComponent<CAnimation>(Assets::getInstance().getAnimation(name)).animation.getBB();
			_player->addComponent<CBoundingBox>(bb);
			auto pixelPos = gridToMidPixel(gx, gy, _player);
			_player->addComponent<CTransform>(pixelPos);
			_player->addComponent<CState>("alive");
			_player->addComponent<CInput>();

			
		}
		else
		{
			std::string buffer;
			std::getline(config, buffer);
		}
		config >> token;
	}
	std::cout << "DONE READING";
}

Vec2 Scene_DecaCube::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float x = 0.f + gridX * gridSize.x;
	float y = 440.f - gridY * gridSize.y;

	sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getBB();

	return Vec2(x + spriteSize.x / 2.f, y - spriteSize.y / 2.f);
}

Scene_DecaCube::Scene_DecaCube(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) {

	init(_levelPath);

}

void Scene_DecaCube::update(sf::Time dt)
{
	_entityManager.update();

	playerMovement();
	sMovement(dt);

	sRender();
}

void Scene_DecaCube::sDoAction(const Command& command)
{
	//code template from Dave Burchill, NBCC
	if (command.type() == "START") {
		if (command.name() == "LEFT")
			_player->getComponent<CInput>().left = true;
		else if (command.name() == "RIGHT")
			_player->getComponent<CInput>().right = true;
		else if (command.name() == "UP")
			_player->getComponent<CInput>().up = true;
		else if (command.name() == "DOWN")
			_player->getComponent<CInput>().down = true;
		
	}
	else if (command.type() == "END") {
		if (command.name() == "LEFT")
			_player->getComponent<CInput>().left = false;
		else if (command.name() == "RIGHT")
			_player->getComponent<CInput>().right = false;
		else if (command.name() == "UP")
			_player->getComponent<CInput>().up = false;
		else if (command.name() == "DOWN")
			_player->getComponent<CInput>().down = false;
	}
}

void Scene_DecaCube::sRender()
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

	//for (auto e : _entityManager.getEntities()) {
	//	if (e->hasComponent<CBoundingBox>()) {
	//		auto& box = e->getComponent<CBoundingBox>();
	//		auto& transform = e->getComponent<CTransform>();
	//		sf::RectangleShape rect;
	//		rect.setSize(sf::Vector2f(box.size.x, box.size.y));
	//		rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
	//		rect.setPosition(transform.pos.x, transform.pos.y);
	//		rect.setFillColor(sf::Color(0, 0, 0, 0));
	//		rect.setOutlineColor(sf::Color(0, 255, 0));
	//		rect.setOutlineThickness(1.f);
	//		_game->window().draw(rect);
	//	}
	//}

	_game->window().display();
}
