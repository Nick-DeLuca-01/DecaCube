#include "Scene_CubeLeft.h"
#include "Scene_DecaCube.h"
#include "Scene_CubeRight.h"
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
#include <random>

namespace {
	std::random_device rd;
	std::mt19937 rng(rd());
}

void Scene_CubeFront::sMovement(sf::Time dt)
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

	for (auto e : _enemyData.enemyManager.getEntities("enemy")) {
		auto& etfm = e->getComponent<CTransform>();
		auto& ePathfinding = e->getComponent<CPathFinding>();

		etfm.prevPos = etfm.pos;

		etfm.pos += etfm.vel * dt.asSeconds();

		auto eDiff = etfm.pos - etfm.prevPos;

		if (eDiff.x > 0 || eDiff.y > 0) {
			ePathfinding.distanceRemainingPos -= eDiff;
		}
		else if (eDiff.x < 0 || eDiff.y < 0) {
			ePathfinding.distanceRemainingNeg -= eDiff;
		}

		if (ePathfinding.distanceRemainingPos.x <= 0 && ePathfinding.right) {
			ePathfinding.distanceRemainingPos.x = 0;
			etfm.vel.x = 0;
			ePathfinding.right = false;
			snapToGrid(e);
		}
		else if (ePathfinding.distanceRemainingPos.y <= 0 && ePathfinding.down) {
			ePathfinding.distanceRemainingPos.y = 0;
			etfm.vel.y = 0;
			ePathfinding.down = false;
			snapToGrid(e);
		}
		else if (ePathfinding.distanceRemainingNeg.x >= 0 && ePathfinding.left) {
			ePathfinding.distanceRemainingNeg.x = 0;
			etfm.vel.x = 0;
			ePathfinding.left = false;
			snapToGrid(e);
		}
		else if (ePathfinding.distanceRemainingNeg.y >= 0 && ePathfinding.up) {
			ePathfinding.distanceRemainingNeg.y = 0;
			etfm.vel.y = 0;
			ePathfinding.up = false;
			snapToGrid(e);
		}
	}
}

void Scene_CubeFront::sAnimation()
{
}

void Scene_CubeFront::sCollision()
{
	for (auto e : _entityManager.getEntities("item")) {
		auto overlap = Physics::getOverlap(e, _player);
		if (overlap.x > 0 && overlap.y > 0) {
			_playerData.score += e->getComponent<CScore>().score;
			_playerData.collectedItems.push_back(e->getComponent<CState>().state);
			e->destroy();

		}
	}
}

void Scene_CubeFront::sEnemyFaceChange(sf::Time dt)
{
	for (auto e : _enemyData.enemyManager.getEntities()) {
		auto& offScreen = e->getComponent<COffScreen>();
		auto& pathfinding = e->getComponent<CPathFinding>();
		if ((_player->getComponent<CLocation>().currentFace != e->getComponent<CLocation>().currentFace) || e->getComponent<CState>().state == "Flipper") {
			offScreen.secondsOffScreen += dt;
			if (offScreen.secondsOffScreen >= offScreen.sceneChangeThreshold) {
				//code to switch scene
				int newFace = changeFace(e->getComponent<CLocation>().currentFace, e->getComponent<CState>().state == "Flipper");

				offScreen.secondsOffScreen = sf::Time::Zero;
				e->getComponent<CLocation>().currentFace = newFace;
				if (newFace == _player->getComponent<CLocation>().currentFace && e->getComponent<CState>().state != "Flipper") { //if switching to player's scene, pick random direction to come from
					std::uniform_int_distribution<int> incomingDirection(1, 4);
					int selectedSide = incomingDirection(rng);
					Vec2 newPos{ 0, 0 };
					switch (selectedSide) {
					case 1:
						newPos = { 5, 10 };

						break;
					case 2:
						newPos = { 0, 5 };

						break;
					case 3:
						newPos = { 5, 0 };
						break;
					case 4:
						newPos = { 10, 5 };
						break;
					}
					pathfinding.directionFrom = (selectedSide - 1);
					newPos = gridToMidPixel(newPos.x, newPos.y, e);
					e->getComponent<CTransform>().pos = newPos;
					offScreen.offScreen = false;

				}
				else if (newFace == _player->getComponent<CLocation>().currentFace && e->getComponent<CState>().state == "Flipper") {
					//flipper is a special snowflake
					std::uniform_int_distribution<int> gridCoord(0, 10);
					Vec2 newPos{ 0, 0 };
					newPos.x = gridCoord(rng);
					newPos.y = gridCoord(rng);
					newPos = gridToMidPixel(newPos.x, newPos.y, e);
					auto pPos = _player->getComponent<CTransform>().pos;
					while (((newPos.x - pPos.x) <= 80 && (newPos.x - pPos.x) >= -80) && ((newPos.y - pPos.y <= 80) && (newPos.y - pPos.y >= -80))) {
						//flipper jumped to within 2 spaces of player, reposition to give player some breathing room
						newPos.x = gridCoord(rng);
						newPos.y = gridCoord(rng);
						newPos = gridToMidPixel(newPos.x, newPos.y, e);
					}
					e->getComponent<CTransform>().pos = newPos;
					pathfinding.directionFrom = 4; //can move any direction after appearing
					offScreen.offScreen = false;

				}
				else { //if switching to not player's scene, pick random location (in case player switches to that side)
					std::uniform_int_distribution<int> gridCoord(0, 10);
					Vec2 newPos{ 0, 0 };
					newPos.x = gridCoord(rng);
					newPos.y = gridCoord(rng);
					newPos = gridToMidPixel(newPos.x, newPos.y, e);
					e->getComponent<CTransform>().pos = newPos;
					pathfinding.directionFrom = 4; //can move any direction
					offScreen.offScreen = true;
				}
				sf::Time sec;
				if (e->getComponent<CState>().state == "Flipper") {
					sec = sf::seconds(15.f);
				}
				else {
					std::uniform_real_distribution<float> sceneChangeThreshold(_config.sceneChangeLower, _config.sceneChangeUpper);
					sec = sf::seconds(sceneChangeThreshold(rng));
				}
				offScreen.sceneChangeThreshold = sec;
			}
		}
	}
}

void Scene_CubeFront::sEnemyBehaviour()
{
	for (auto e : _enemyData.enemyManager.getEntities()) {
		auto state = e->getComponent<CState>().state;
		auto isVisible = e->getComponent<COffScreen>().offScreen != true;
		if (state == "Flipper" && isVisible) {
			flipper(e);
		}
		else if (state == "Gunner" && isVisible) {
			std::cout << "TODO";
		}
	}
}

void Scene_CubeFront::flipper(std::shared_ptr<Entity> entity)
{
	enemyAwareMovement(entity);
}

std::vector<Vec2> Scene_CubeFront::getAvailableNodes(Vec2 pos, std::shared_ptr<Entity> entity)
{
	std::vector<Vec2> availableNodes;

	//function won't be called if enemy isn't at a grid position

	bool canMove = false;

	auto pathfinding = entity->getComponent<CPathFinding>();

	auto ePos = entity->getComponent<CTransform>().pos;

	if (pathfinding.directionFrom != 0) {
		canMove = canMoveInDirection("UP", entity);
		if (canMove) {

			auto grid = midPixelToGrid(ePos.x, ePos.y - 40, entity);
			bool alreadyVisited = alreadyTraveled(pathfinding.visitedNodes, grid);
			if (!alreadyVisited) {
				availableNodes.push_back(grid);
			}
		}
	}
	if (pathfinding.directionFrom != 1) {
		canMove = canMoveInDirection("LEFT", entity);
		if (canMove) {

			auto grid = midPixelToGrid(ePos.x - 40, ePos.y, entity);
			bool alreadyVisited = alreadyTraveled(pathfinding.visitedNodes, grid);
			if (!alreadyVisited) {
				availableNodes.push_back(grid);
			}
		}
	}
	if (pathfinding.directionFrom != 2) {
		canMove = canMoveInDirection("DOWN", entity);
		if (canMove) {

			auto grid = midPixelToGrid(ePos.x, ePos.y + 40, entity);
			bool alreadyVisited = alreadyTraveled(pathfinding.visitedNodes, grid);
			if (!alreadyVisited) {
				availableNodes.push_back(grid);
			}
		}
	}
	if (pathfinding.directionFrom != 3) {
		canMove = canMoveInDirection("RIGHT", entity);
		if (canMove) {

			auto grid = midPixelToGrid(ePos.x + 40, ePos.y, entity);
			bool alreadyVisited = alreadyTraveled(pathfinding.visitedNodes, grid);
			if (!alreadyVisited) {
				availableNodes.push_back(grid);
			}
		}
	}

	Vec2 goBackPos = ePos;
	if (availableNodes.empty()) {
		switch (pathfinding.directionFrom) {
		case 0:
			goBackPos.y -= 40.f;
			break;
		case 1:
			goBackPos.x -= 40.f;
			break;
		case 2:
			goBackPos.y += 40.f;
			break;
		case 3:
			goBackPos.x += 40.f;
			break;
		}
		availableNodes.push_back(midPixelToGrid(goBackPos.x, goBackPos.y, entity));
	}

	return availableNodes;
}

Vec2 Scene_CubeFront::pickBestNode(std::vector<Vec2> availableNodes)
{
	float minDistance = 1000.f;
	Vec2 closestNode;

	auto playerPos = _player->getComponent<CTransform>().pos;
	auto playerGrid = midPixelToGrid(playerPos.x, playerPos.y, _player);
	playerGrid.x = std::roundf(playerGrid.x);
	playerGrid.y = std::roundf(playerGrid.y); //rounded to avoid potential weird issues with enemy movement
	sf::Vector2f pGrid{ playerGrid.x, playerGrid.y };


	for (auto n : availableNodes) {
		sf::Vector2f vecN{ n.x, n.y };
		auto distance = dist(pGrid, vecN);
		if (distance < minDistance) {
			closestNode = { n.x, n.y };
			minDistance = distance;
		}
	}

	return closestNode;
}

void Scene_CubeFront::enemyAwareMovement(std::shared_ptr<Entity> enemy)
{
	auto& tfm = enemy->getComponent<CTransform>();

	auto& pathFinding = enemy->getComponent<CPathFinding>();

	std::vector<Vec2> availableNodes;

	if (pathFinding.distanceRemainingPos.x == 0.f && pathFinding.distanceRemainingPos.y == 0.f && pathFinding.distanceRemainingNeg.x == 0.f && pathFinding.distanceRemainingNeg.y == 0.f) {
		availableNodes = getAvailableNodes(tfm.pos, enemy);
		Vec2 bestNode = pickBestNode(availableNodes);
		pathFinding.targetGrid = bestNode;
		pathFinding.visitedNodes.push_back(bestNode);
		if (pathFinding.visitedNodes.size() > 7) {
			pathFinding.visitedNodes.erase(pathFinding.visitedNodes.begin()); //only tracks the last 7 visited nodes, to prevent going in circles
		}

		auto bestNodePix = gridToMidPixel(bestNode.x, bestNode.y, enemy);
		auto enemyPos = tfm.pos;

		auto distance = bestNodePix - enemyPos;
		if (distance.x > 0) {
			pathFinding.distanceRemainingPos.x = distance.x;
			tfm.vel.x += 1;
			pathFinding.right = true;
			pathFinding.directionFrom = 1;
		}
		else if (distance.x < 0) {
			pathFinding.distanceRemainingNeg.x = distance.x;
			tfm.vel.x -= 1;
			pathFinding.left = true;
			pathFinding.directionFrom = 3;
		}
		else if (distance.y > 0) {
			pathFinding.distanceRemainingPos.y = distance.y;
			tfm.vel.y += 1;
			pathFinding.down = true;
			pathFinding.directionFrom = 0;
		}
		else if (distance.y < 0) {
			pathFinding.distanceRemainingNeg.y = distance.y;
			tfm.vel.y -= 1;
			pathFinding.up = true;
			pathFinding.directionFrom = 2;
		}
		tfm.vel = tfm.vel * _config.enemySpeed;
	}
}

void Scene_CubeFront::onEnd()
{
	_game->changeScene("MENU", nullptr, false);
	_game->reset();
}

void Scene_CubeFront::init(const std::string& path)
{
	loadLevel(path);
	registerActions();
}

void Scene_CubeFront::registerActions()
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

void Scene_CubeFront::spawnPlayer(sf::Vector2f pos)
{
}

void Scene_CubeFront::loadLevel(const std::string& path)
{
	_entityManager = EntityManager();
	loadFromFile(path);
}

Vec2 Scene_CubeFront::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float x = 0.f + gridX * gridSize.x;
	float y = 440.f - gridY * gridSize.y;

	sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getBB();

	return Vec2(x + spriteSize.x / 2.f, y - spriteSize.y / 2.f);
}

Vec2 Scene_CubeFront::midPixelToGrid(float midX, float midY, std::shared_ptr<Entity> entity)
{
	if (entity->getTag() == "pathfinder") {
		return Vec2((midX - 20.f) / gridSize.x, (440.f - (midY + 20.f)) / gridSize.y);
	}

	sf::Vector2f spriteSize = entity->getComponent<CAnimation>().animation.getBB();
	float x = midX - (spriteSize.x / 2.f);
	float y = midY + (spriteSize.y / 2.f);

	return Vec2(x / gridSize.x, (440.f - y) / gridSize.y);
}

void Scene_CubeFront::loadFromFile(const std::string& path)
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

	auto e = _entityManager.addEntity("pathfinder");
	sf::Vector2f smallBox = { 1,1 };
	e->addComponent<CBoundingBox>(smallBox);
	e->addComponent<CTransform>(pixelPos);
}

void Scene_CubeFront::playerMovement()
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

void Scene_CubeFront::adjustPlayerPosition()
{
}

void Scene_CubeFront::snapToGrid(std::shared_ptr<Entity> entity)
{
	auto& tfm = entity->getComponent<CTransform>();

	auto gridPosX = std::fmod(tfm.pos.x, 40.f);
	auto gridPosY = std::fmod(tfm.pos.y, 40.f);

	tfm.pos.x -= (gridPosX - 20);
	tfm.pos.y -= (gridPosY - 20);
}

bool Scene_CubeFront::canMoveInDirection(std::string direction, std::shared_ptr<Entity> entity)
{
	auto e = getCurrentTile(entity);
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
	return true;
}

sPtrEntt Scene_CubeFront::getCurrentTile(std::shared_ptr<Entity> entity)
{
	for (auto e : _entityManager.getEntities()) {
		auto overlap = Physics::getOverlap(entity, e);
		if (overlap.x > 0 && overlap.y > 0) {
			return e;

		}
	}
	return _player;
}

void Scene_CubeFront::checkIfPlayerInBounds()
{
	auto& pPos = _player->getComponent<CTransform>().pos;

	//each side of the square leads to another side of the square
	//only one exit on each side

	if (pPos.x < 0) {
		auto& pinput = _player->getComponent<CInput>();
		pinput.distanceRemainingNeg = { 0, 0 };
		pinput.distanceRemainingPos = { 0, 0 };
		_playerData.spawnPos = { 5, 0 };
		_playerData.sceneChanged = true;
		switch (_playerData.faceRotation) {
		case 0:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_LEFT", std::make_shared<Scene_CubeLeft>(_game, "../assets/cubeleft.txt"), false);
			break;
		case 1:
			_playerData.faceRotation = (_playerData.faceRotation + 2) % 4;
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		case 2:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_RIGHT", std::make_shared<Scene_CubeRight>(_game, "../assets/cuberight.txt"), false);
			break;
		case 3:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		}
		

	}
	else if (pPos.x > 440) {
		auto& pinput = _player->getComponent<CInput>();
		pinput.distanceRemainingNeg = { 0, 0 };
		pinput.distanceRemainingPos = { 0, 0 };
		_playerData.spawnPos = { 5, 0 };
		_playerData.sceneChanged = true;
		switch (_playerData.faceRotation) {
		case 0:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_RIGHT", std::make_shared<Scene_CubeRight>(_game, "../assets/cuberight.txt"), false);
			break;
		case 1:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		case 2:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_LEFT", std::make_shared<Scene_CubeLeft>(_game, "../assets/cubeleft.txt"), false);
			break;
		case 3:
			_playerData.faceRotation = (_playerData.faceRotation + 2) % 4;
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
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
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		case 1:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_LEFT", std::make_shared<Scene_CubeLeft>(_game, "../assets/cubeleft.txt"), false);
			break;
		case 2:
			_playerData.faceRotation = (_playerData.faceRotation + 2) % 4;
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		case 3:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_RIGHT", std::make_shared<Scene_CubeRight>(_game, "../assets/cuberight.txt"), false);
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
			_playerData.faceRotation = (_playerData.faceRotation + 2) % 4;
			_game->changeScene("PLAY_BOTTOM", std::make_shared<Scene_CubeBottom>(_game, "../assets/cubebottom.txt"), false);
			break;
		case 1:
			_playerData.faceRotation = (_playerData.faceRotation + 1) % 4;
			_game->changeScene("PLAY_RIGHT", std::make_shared<Scene_CubeRight>(_game, "../assets/cuberight.txt"), false);
			break;
		case 2:
			_game->changeScene("PLAY", std::make_shared<Scene_DecaCube>(_game, "../assets/cubetop.txt"), false);
			break;
		case 3:
			_playerData.faceRotation = (_playerData.faceRotation + 3) % 4;
			_game->changeScene("PLAY_LEFT", std::make_shared<Scene_CubeLeft>(_game, "../assets/cubeleft.txt"), false);
			break;
				
		}
		
	}
}

void Scene_CubeFront::fixPlayerPos()
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
	_player->getComponent<CLocation>().currentFace = 4;
	_nextControl = "";
	for (auto enemy : _enemyData.enemyManager.getEntities()) {
		if (enemy->getComponent<CLocation>().currentFace == _player->getComponent<CLocation>().currentFace) {
			enemy->getComponent<COffScreen>().offScreen = false;
		}
	}
}

std::string Scene_CubeFront::getRotatedTileName(std::string name, int rotations)
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

Vec2 Scene_CubeFront::rotateTilePosition(Vec2 prePos)
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

Vec2 Scene_CubeFront::rotateEntityPosition(Vec2 prePos)
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

void Scene_CubeFront::rotateEntireFace()
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

int Scene_CubeFront::changeFace(int currentFace, bool isFlipper)
{
	std::uniform_int_distribution<int> faces(1, 6);
	int newFace = faces(rng);
	while (newFace == currentFace || ((newFace + currentFace) == 7 && !isFlipper)) { //face values are set up so that opposing face values always add to seven, similar to most dice
		//if the roll is the current face or the opposite face, reroll
		//if enemy rolling is the flipper, ignore opposite face check
		newFace = faces(rng);
	}
	if (isFlipper && _player->getComponent<CLocation>().currentFace != currentFace) { //if Flipper isn't on player's face, switch to player's face. otherwise we use the previous calc
		newFace = _player->getComponent<CLocation>().currentFace;
	}
	return newFace;
}

bool Scene_CubeFront::alreadyTraveled(std::vector<Vec2> visitedNodes, Vec2 targetNode)
{
	for (auto node : visitedNodes) {
		if (targetNode == node) {
			return true;
		}
	}
	return false;
}

Scene_CubeFront::Scene_CubeFront(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()), _levelPath(levelPath) 
{
	init(_levelPath);
}

void Scene_CubeFront::update(sf::Time dt)
{
	_entityManager.update();
	_enemyData.enemyManager.update();
	if (_playerData.sceneChanged) {
		fixPlayerPos();
	}
	playerMovement();
	sEnemyFaceChange(dt);
	sEnemyBehaviour();
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

void Scene_CubeFront::sDoAction(const Command& command)
{
	if (command.type() == "START") {
		_nextControl = command.name();
	}

	//code template from Dave Burchill, NBCC
	if (!_player->getComponent<CInput>().left && !_player->getComponent<CInput>().right && !_player->getComponent<CInput>().up && !_player->getComponent<CInput>().down) {
		bool validDirection = canMoveInDirection(_nextControl, _player);
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

void Scene_CubeFront::sRender()
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
