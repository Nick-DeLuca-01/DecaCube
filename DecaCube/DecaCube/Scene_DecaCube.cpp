#include "Scene_DecaCube.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

void Scene_DecaCube::sMovement()
{
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
}

void Scene_DecaCube::spawnPlayer(sf::Vector2f pos)
{
}

void Scene_DecaCube::playerMovement()
{
}

void Scene_DecaCube::adjustPlayerPosition()
{
}

void Scene_DecaCube::init(const std::string& path)
{
	loadLevel(path);
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
	sRender();
}

void Scene_DecaCube::sDoAction(const Command& command)
{
}

void Scene_DecaCube::sRender()
{
	for (auto e : _entityManager.getEntities()) {
		if (e->hasComponent<CAnimation>()) {
			auto& tfm = e->getComponent<CTransform>();
			auto& anim = e->getComponent<CAnimation>().animation;

			anim.getSprite().setRotation(tfm.angle);
			anim.getSprite().setPosition(tfm.pos.x, tfm.pos.y);
			_game->window().draw(anim.getSprite());
		}
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
