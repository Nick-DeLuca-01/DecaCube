#include "Scene_DecaCube.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

#include <cmath>

void Scene_DecaCube::sMovement(sf::Time dt)
{
	auto& ptfm = _player->getComponent<CTransform>();
	auto& pinput = _player->getComponent<CInput>();
	ptfm.prevPos = ptfm.pos; //set the previous position

	ptfm.pos += ptfm.vel * dt.asSeconds(); //move player
	auto difference = ptfm.pos - ptfm.prevPos; //get distance moved
	if (difference.x > 0 || difference.y > 0) { //player moved right or down
		pinput.distanceRemainingPos -= difference;
	}
	else if (difference.x < 0 || difference.y < 0) { //player moved left or up
		pinput.distanceRemainingNeg -= difference;
	}
	if (pinput.right && pinput.distanceRemainingPos.x <= 0) {

		pinput.distanceRemainingPos.x = 0;
		pinput.right = false;

		snapToGrid(_player);

	}
	else if (pinput.down && pinput.distanceRemainingPos.y <= 0) {
		pinput.distanceRemainingPos.y = 0;
		pinput.down = false;

		snapToGrid(_player);
	}
	else if (pinput.left && pinput.distanceRemainingNeg.x >= 0) {
		pinput.distanceRemainingNeg.x = 0;
		pinput.left = false;

		snapToGrid(_player);

	}
	else if (pinput.up && pinput.distanceRemainingNeg.y >= 0) {
		pinput.distanceRemainingNeg.y = 0;
		pinput.up = false;

		snapToGrid(_player);

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

void Scene_DecaCube::snapToGrid(std::shared_ptr<Entity> entity)
{
	auto& tfm = entity->getComponent<CTransform>();

	auto gridPosX = std::fmod(tfm.pos.x, 40.f);
	auto gridPosY = std::fmod(tfm.pos.y, 40.f);

	tfm.pos.x -= (gridPosX - 20);
	tfm.pos.y -= (gridPosY - 20);
	
}

bool Scene_DecaCube::canMoveInDirection(std::string direction)
{
	auto e = getCurrentTile();
	if (e->getTag() == "robert") {
		return true; //in case the player isn't overlapping any tiles
	}
	std::string tileType = e->getComponent<CState>().state;
	if (tileType == "DownEnd") {
		if (direction == "UP") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "DownLeftCorner") {
		if (direction == "UP" || direction == "RIGHT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "DownRightCorner") {
		if (direction == "UP" || direction == "LEFT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "DownWall") {
		if (direction == "DOWN") {
			return false;
		}
		else {
			return true;
		}
	}
	if (tileType == "LeftEnd") {
		if (direction == "RIGHT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "LeftRightHall") {
		if (direction == "LEFT" || direction == "RIGHT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "LeftWall") {
		if (direction == "LEFT") {
			return false;
		}
		else {
			return true;
		}
	}
	if (tileType == "NoAccess") {
		return false;
	}
	if (tileType == "NoWall") {
		return true;
	}
	if (tileType == "RightEnd") {
		if (direction == "LEFT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "RightWall") {
		if (direction == "RIGHT") {
			return false;
		}
		else {
			return true;
		}
	}
	if (tileType == "UpDownHall") {
		if (direction == "UP" || direction == "DOWN") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "UpEnd") {
		if (direction == "DOWN") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "UpLeftCorner") {
		if (direction == "DOWN" || direction == "RIGHT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "UpRightCorner") {
		if (direction == "DOWN" || direction == "LEFT") {
			return true;
		}
		else {
			return false;
		}
	}
	if (tileType == "UpWall") {
		if (direction == "UP") {
			return false;
		}
		else {
			return true;
		}
	}
	return true; //if there's a tile i missed somehow
}

sPtrEntt Scene_DecaCube::getCurrentTile()
{
	for (auto e : _entityManager.getEntities()) {
		auto overlap = Physics::getOverlap(_player, e); 
		if (overlap.x > 0 && overlap.y > 0) {
			return e;
		
		}
	}
	return _player; //if the player somehow is not overlapping a tile, just return the player, will be handled as overlapping nothing
}

void Scene_DecaCube::checkIfPlayerInBounds()
{
	auto pPos = _player->getComponent<CTransform>().pos;

	//each side of the square leads to another side of the square
	//only one exit on each side

	if (pPos.x < 0) {
		_game->changeScene("PLAY_LEFT", std::make_shared<Scene_CubeLeft>(_game, ))
	}
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
	if (_nextControl != "") {
		sDoAction(Command{ _nextControl, "START" });
	}
	sRender();
	checkIfPlayerInBounds();
}

void Scene_DecaCube::sDoAction(const Command& command)
{

	if (command.type() == "START") {
		_nextControl = command.name();
	}

	//code template from Dave Burchill, NBCC
	if (!_player->getComponent<CInput>().left && !_player->getComponent<CInput>().right && !_player->getComponent<CInput>().up && !_player->getComponent<CInput>().down) {
		bool validDirection = canMoveInDirection(_nextControl);
		if (_nextControl == "LEFT" && validDirection) {
			_player->getComponent<CInput>().left = true;
			_player->getComponent<CInput>().distanceRemainingNeg.x = -40;
			
		}
		else if (_nextControl == "RIGHT" && validDirection) {
			_player->getComponent<CInput>().right = true;
			_player->getComponent<CInput>().distanceRemainingPos.x = 40;
			
		}
		else if (_nextControl == "UP" && validDirection) {
			_player->getComponent<CInput>().up = true;
			_player->getComponent<CInput>().distanceRemainingNeg.y = -40;
			
		}
		else if (_nextControl == "DOWN" && validDirection) {
			_player->getComponent<CInput>().down = true;
			_player->getComponent<CInput>().distanceRemainingPos.y = 40;
			
		}
		_nextControl = "";

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
