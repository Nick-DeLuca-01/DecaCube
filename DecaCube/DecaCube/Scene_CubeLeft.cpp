#include "Scene_CubeLeft.h"
#include "Scene_DecaCube.h"
#include "Scene_CubeBack.h"
#include "Scene_CubeFront.h"
#include "Scene_CubeBottom.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

#include <cmath>

void Scene_CubeLeft::sMovement(sf::Time dt)
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

void Scene_CubeLeft::sAnimation()
{
}

void Scene_CubeLeft::sCollision()
{
	for (auto e : _entityManager.getEntities("item")) {
		auto overlap = Physics::getOverlap(e, _player);
		if (overlap.x > 0 && overlap.y > 0) {
			_playerData.score += e->getComponent<CScore>().score;
			_playerData.collectedItems.push_back(e->getComponent<CState>().state);
			e->destroy();
			std::cout << _playerData.score << " points\n"; //temporary debug line

		}
	}
}

void Scene_CubeLeft::onEnd()
{
	_game->changeScene("MENU", nullptr, false);
	_game->reset();
}

void Scene_CubeLeft::init(const std::string& path)
{
	loadLevel(path);
	registerActions();
}

void Scene_CubeLeft::registerActions()
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
			Vec2 prePos(gx, gy);

			Vec2 newPos = rotateTilePosition(prePos);

			gx = newPos.x;
			gy = newPos.y;

			e->addComponent<CTransform>(gridToMidPixel(gx, gy, e));
			e->getComponent<CTransform>().angle = 90 * _playerData.faceRotation; //each face gets rotated based on current face rotation
			e->addComponent<CState>(getRotatedTileName(name, _playerData.faceRotation));
		}
		else if (token == "Item")
		{
			std::string name;

			bool collected = false;

			float gridX, gridY, points;

			config >> name >> gridX >> gridY >> points;

			for (int i = 0; i < _playerData.collectedItems.size(); i++) {
				if (name == _playerData.collectedItems[i]) {
					collected = true;
				}
			}

			if (!collected) {
				auto e = _entityManager.addEntity("item");
				auto bb = e->addComponent<CAnimation>(Assets::getInstance().getAnimation(name)).animation.getBB();
				e->addComponent<CBoundingBox>(bb);

				Vec2 prePos(gridX, gridY);
				Vec2 newPos = rotateTilePosition(prePos);

				gridX = newPos.x;
				gridY = newPos.y;

				auto pixelPos = gridToMidPixel(gridX, gridY, e);
				e->addComponent<CTransform>(pixelPos);
				e->addComponent<CState>(name);
				e->addComponent<CScore>(points);
			}
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
	auto pixelPos = gridToMidPixel(_playerData.spawnPos.x, _playerData.spawnPos.y, _player);
	_player->addComponent<CTransform>(pixelPos);
	_player->addComponent<CState>("alive");
	_player->addComponent<CInput>();
}

void Scene_CubeLeft::playerMovement()
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

void Scene_CubeLeft::adjustPlayerPosition()
{
}

void Scene_CubeLeft::snapToGrid(std::shared_ptr<Entity> entity)
{
	auto& tfm = entity->getComponent<CTransform>();

	auto gridPosX = std::fmod(tfm.pos.x, 40.f);
	auto gridPosY = std::fmod(tfm.pos.y, 40.f);

	tfm.pos.x -= (gridPosX - 20);
	tfm.pos.y -= (gridPosY - 20);
}

bool Scene_CubeLeft::canMoveInDirection(std::string direction)
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

sPtrEntt Scene_CubeLeft::getCurrentTile()
{
	for (auto e : _entityManager.getEntities()) {
		auto overlap = Physics::getOverlap(_player, e);
		if (overlap.x > 0 && overlap.y > 0) {
			return e;

		}
	}
	return _player;
}

void Scene_CubeLeft::checkIfPlayerInBounds()
{
	auto& pPos = _player->getComponent<CTransform>().pos;

	//each side of the square leads to another side of the square
	//only one exit on each side

	if (pPos.x < 0) {
		auto& pinput = _player->getComponent<CInput>();
		pinput.distanceRemainingNeg = { 0, 0 };
		pinput.distanceRemainingPos = { 0, 0 };
		_playerData.spawnPos = { 10, 5 };
		_playerData.sceneChanged = true;
		switch (_playerData.faceRotation) {
		case 0:
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		case 1:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_FRONT", std::make_shared<Scene_CubeFront>(_game, "../assets/cubefront.txt"), false);
			break;
		case 2:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		case 3:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_BACK", std::make_shared<Scene_CubeBack>(_game, "../assets/cubeback.txt"), false);
			break;
		}
		

	}
	else if (pPos.x > 440) {
		auto& pinput = _player->getComponent<CInput>();
		pinput.distanceRemainingNeg = { 0, 0 };
		pinput.distanceRemainingPos = { 0, 0 };
		_playerData.spawnPos = { 0, 5 };
		_playerData.sceneChanged = true;
		switch (_playerData.faceRotation) {
		case 0:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		case 1:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_BACK", std::make_shared<Scene_CubeBack>(_game, "../assets/cubeback.txt"), false);
			break;
		case 2:
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		case 3:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_FRONT", std::make_shared<Scene_CubeFront>(_game, "../assets/cubefront.txt"), false);
			break;
		}
		
	}
	else if (pPos.y < 0) {
		auto& pinput = _player->getComponent<CInput>();
		pinput.distanceRemainingNeg = { 0, 0 };
		pinput.distanceRemainingPos = { 0, 0 };
		_playerData.spawnPos = { 5, 0 };
		_playerData.sceneChanged = true;
		switch (_playerData.faceRotation) {
		case 0:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_BACK", std::make_shared<Scene_CubeBack>(_game, "../assets/cubeback.txt"), false);
			break;
		case 1:
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		case 2:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_FRONT", std::make_shared<Scene_CubeFront>(_game, "../assets/cubefront.txt"), false);
			break;
		case 3:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		}
		
	}
	else if (pPos.y > 440) {
		auto& pinput = _player->getComponent<CInput>();
		pinput.distanceRemainingNeg = { 0, 0 };
		pinput.distanceRemainingPos = { 0, 0 };
		_playerData.spawnPos = { 5, 10 };
		_playerData.sceneChanged = true;
		switch (_playerData.faceRotation) {
		case 0:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_FRONT", std::make_shared<Scene_CubeFront>(_game, "../assets/cubefront.txt"), false);
			break;
		case 1:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		case 2:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_BACK", std::make_shared<Scene_CubeBack>(_game, "../assets/cubeback.txt"), false);
			break;
		case 3:
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		}
		
	}
}

void Scene_CubeLeft::fixPlayerPos()
{
	_playerData.sceneChanged = false;

	auto pixelPos = gridToMidPixel(_playerData.spawnPos.x, _playerData.spawnPos.y, _player);
	if (_prevRotation != _playerData.faceRotation && _initialized) {
		rotateEntireFace();
		
	}
	if (!_initialized) {
		_initialized = true;
	}
	_prevRotation = _playerData.faceRotation;
	_player->getComponent<CTransform>().pos = pixelPos;
	_player->getComponent<CLocation>().currentFace = 5;
	_nextControl = "";
}

std::string Scene_CubeLeft::getRotatedTileName(std::string name, int rotations)
{
	if (name == "DownEnd") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "LeftEnd";
		case 2:
			return "UpEnd";
		case 3:
			return "RightEnd";
		}
	}
	if (name == "DownLeftCorner") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "UpLeftCorner";
		case 2:
			return "UpRightCorner";
		case 3:
			return "DownRightCorner";
		}
	}
	if (name == "DownRightCorner") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "DownLeftCorner";
		case 2:
			return "UpLeftCorner";
		case 3:
			return "UpRightCorner";
		}
	}
	if (name == "DownWall") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "LeftWall";
		case 2:
			return "UpWall";
		case 3:
			return "RightWall";
		}
	}
	if (name == "LeftEnd") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "UpEnd";
		case 2:
			return "RightEnd";
		case 3:
			return "DownEnd";
		}
	}
	if (name == "LeftRightHall") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "UpDownHall";
		case 2:
			return "LeftRightHall";
		case 3:
			return "UpDownHall";
		}
	}
	if (name == "LeftWall") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "UpWall";
		case 2:
			return "RightWall";
		case 3:
			return "DownWall";
		}
	}
	if (name == "NoWall") {
		return name;
	}
	if (name == "NoAccess") {
		return name;
	}
	if (name == "RightEnd") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "DownEnd";
		case 2:
			return "LeftEnd";
		case 3:
			return "UpEnd";
		}
	}
	if (name == "RightWall") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "DownWall";
		case 2:
			return "LeftWall";
		case 3:
			return "UpWall";
		}
	}
	if (name == "UpDownHall") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "LeftRightHall";
		case 2:
			return "UpDownHall";
		case 3:
			return "LeftRightHall";
		}
	}
	if (name == "UpEnd") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "RightEnd";
		case 2:
			return "DownEnd";
		case 3:
			return "LeftEnd";
		}
	}
	if (name == "UpLeftCorner") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "UpRightCorner";
		case 2:
			return "DownRightCorner";
		case 3:
			return "DownLeftCorner";
		}
	}
	if (name == "UpRightCorner") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "DownRightCorner";
		case 2:
			return "DownLeftCorner";
		case 3:
			return "UpLeftCorner";
		}
	}
	if (name == "UpWall") {
		switch (rotations) {
		case 0:
			return name;
		case 1:
			return "RightWall";
		case 2:
			return "DownWall";
		case 3:
			return "LeftWall";
		}
	}
	return name;
}

Vec2 Scene_CubeLeft::rotateTilePosition(Vec2 prePos)
{
	if (_playerData.faceRotation == 0) {
		return prePos;
	}
	Vec2 newPos = prePos;
	for (auto i = 0; i < _playerData.faceRotation; i++) {
		int tempX = newPos.x;
		newPos.x = newPos.y;
		newPos.y = 10 - tempX;
	}
	return newPos;
}

Vec2 Scene_CubeLeft::rotateEntityPosition(Vec2 prePos)
{
	Vec2 newPos = prePos;
	int i = _prevRotation;
	while (i != _playerData.faceRotation) {
		int tempY = newPos.y;
		newPos.y = newPos.x;
		newPos.x = 440 - tempY;

		i++;
		i %= 4;
	}
	return newPos;
}

void Scene_CubeLeft::rotateEntireFace()
{
	for (auto e : _entityManager.getEntities()) {
		auto& pos = e->getComponent<CTransform>().pos;
		Vec2 newPos = rotateEntityPosition(pos);
		pos = newPos;
		if (e->getTag() == "tile") {
			e->getComponent<CTransform>().angle = 90 * _playerData.faceRotation;
			std::string prevState = e->getComponent<CState>().state;
			int rotations;
			if (_playerData.faceRotation == 0) {
				rotations = 4 - _prevRotation;
			}
			else {
				rotations = _playerData.faceRotation - _prevRotation;
			}
			e->getComponent<CState>().state = getRotatedTileName(prevState, (rotations));
		}
	}
	for (auto e : _enemyData.enemyManager.getEntities()) {
		if (_player->getComponent<CLocation>().currentFace == e->getComponent<CLocation>().currentFace) {
			auto& pos = e->getComponent<CTransform>().pos;
			Vec2 newPos = rotateEntityPosition(pos);
			pos = newPos;
			e->getComponent<CTransform>().angle = 90 * _playerData.faceRotation;
		}
	}
}

Scene_CubeLeft::Scene_CubeLeft(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) 
{
	init(_levelPath);
}

void Scene_CubeLeft::update(sf::Time dt)
{
	_entityManager.update();
	_enemyData.enemyManager.update();
	if (_playerData.sceneChanged) {
		fixPlayerPos();
	}
	playerMovement();
	sMovement(dt);
	if (_nextControl != "") {
		sDoAction(Command{ _nextControl, "START" });
	}
	sRender();
	sCollision();
	checkIfPlayerInBounds();

	_playerData.elapsedTime += dt;

	if (_playerData.collectedItems.size() >= 10) {
		std::cout << "You win! Final score: " << _playerData.score;
		_playerData.lives = 3;
		_playerData.sceneChanged = false;
		_playerData.score = 0;
		_playerData.collectedItems.clear();
		_playerData.spawnPos = { 5, 5 };
		_playerData.faceRotation = 0;
		_playerData.elapsedTime = sf::Time::Zero;
		onEnd();
	}
}

void Scene_CubeLeft::sDoAction(const Command& command)
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
	for (auto e : _entityManager.getEntities("item")) {
		//render items on top of tiles but below other entities
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
	for (auto e : _enemyData.enemyManager.getEntities("enemy")) {
		if (_player->getComponent<CLocation>().currentFace == e->getComponent<CLocation>().currentFace) {
			auto& tfm = e->getComponent<CTransform>();
			auto& anim = e->getComponent<CAnimation>().animation;

			anim.getSprite().setRotation(tfm.angle);
			anim.getSprite().setPosition(tfm.pos.x, tfm.pos.y);

			_game->window().draw(anim.getSprite());
		}
	}
	sf::Text score("Score: " + std::to_string(_playerData.score), Assets::getInstance().getFont("main"), 32);
	score.setFillColor(sf::Color(0, 0, 0));
	score.setPosition(10, 440);

	_game->window().draw(score);
	auto curTime = _playerData.elapsedTime.asSeconds();

	int curTimeInt = std::trunc(std::round(curTime * 10) / 10);

	sf::Text time("Time: " + std::to_string(curTimeInt), Assets::getInstance().getFont("main"), 32);
	time.setFillColor(sf::Color(0, 0, 0));
	time.setPosition(300, 440);

	_game->window().draw(time);
	_game->window().display();
}
