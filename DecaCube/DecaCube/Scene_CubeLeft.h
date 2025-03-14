#pragma once

#include "Components.h"
#include "Scene.h"
#include "Vec2.h"
#include <queue>

//struct SpawnPoint {
//	std::string type;
//	float y;
//	auto operator<=>(const SpawnPoint& other) const {
//		return y <=> other.y;
//	}
//};
//
//struct LevelConfig {
//	float playerSpeed{ 200.f };
//	float enemySpeed{ 200.f };
//};

class Scene_CubeLeft : public Scene
{
    sPtrEntt                            _player{ nullptr };
    sf::View                            _worldView; // camera
    sf::FloatRect                       _worldBounds;
    LevelConfig                         _config;
    bool                                _drawTextures{ true };
    bool                                _drawAABB{ false };
    const Vec2                          gridSize{ 40, 40 };
    bool                                _drawCam{ false };
    int                                 _prevRotation{ 0 };
    bool                                _initialized{ false };


    std::string                         _levelPath;
    std::string                         _nextControl;

    void sMovement(sf::Time dt);
    void sAnimation();
    void sCollision();
    void sEnemyFaceChange(sf::Time dt);

    // helper functions
    void	                onEnd() override;
    void init(const std::string& path);
    void registerActions();
    void spawnPlayer(sf::Vector2f pos);
    void loadLevel(const std::string& path);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
    void loadFromFile(const std::string& path);
    void playerMovement();
    void adjustPlayerPosition();
    void snapToGrid(std::shared_ptr<Entity> entity);
    bool canMoveInDirection(std::string direction);
    sPtrEntt getCurrentTile();
    void checkIfPlayerInBounds();
    void fixPlayerPos();
    std::string getRotatedTileName(std::string name, int rotations);
    Vec2 rotateTilePosition(Vec2 prePos);
    Vec2 rotateEntityPosition(Vec2 prePos);
    void rotateEntireFace();
    int changeFace(int currentFace, bool isFlipper);

public:
    Scene_CubeLeft(GameEngine* gameEngine, const std::string& levelPath);
    void		            update(sf::Time dt) override;
    void		            sDoAction(const Command& command) override;
    void		            sRender() override;
};
