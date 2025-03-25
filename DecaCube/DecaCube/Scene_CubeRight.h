#pragma once

#include "Components.h"
#include "Scene.h"
#include "Vec2.h"
#include <queue>

//struct SpawnPoint {
//    std::string type;
//    float y;
//    auto operator<=>(const SpawnPoint& other) const {
//        return y <=> other.y;
//    }
//};
//
//struct LevelConfig {
//    float playerSpeed{ 200.f };
//    float enemySpeed{ 200.f };
//};

class Scene_CubeRight : public Scene
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
    void sEnemyBehaviour();


    void flipper(std::shared_ptr<Entity> entity);

    std::vector<Vec2> getAvailableNodes(Vec2 pos, std::shared_ptr<Entity> entity);

    Vec2 pickBestNode(std::vector<Vec2> availableNodes);

    void enemyAwareMovement(std::shared_ptr<Entity> enemy);

    // helper functions
    void	                onEnd() override;
    void init(const std::string& path);
    void registerActions();
    void spawnPlayer(sf::Vector2f pos);
    void loadLevel(const std::string& path);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
    Vec2 midPixelToGrid(float midX, float midY, std::shared_ptr<Entity> entity);
    void loadFromFile(const std::string& path);
    void playerMovement();
    void adjustPlayerPosition();
    void snapToGrid(std::shared_ptr<Entity> entity);
    bool canMoveInDirection(std::string direction, std::shared_ptr<Entity> entity);
    sPtrEntt getCurrentTile(std::shared_ptr<Entity> entity);
    void checkIfPlayerInBounds();
    void fixPlayerPos();
    std::string getRotatedTileName(std::string name, int rotations);
    Vec2 rotateTilePosition(Vec2 prePos);
    Vec2 rotateEntityPosition(Vec2 prePos);
    void rotateEntireFace();
    int changeFace(int currentFace, bool isFlipper);

    bool alreadyTraveled(std::vector<Vec2> visitedNodes, Vec2 targetNode);

public:
    Scene_CubeRight(GameEngine* gameEngine, const std::string& levelPath);
    void		            update(sf::Time dt) override;
    void		            sDoAction(const Command& command) override;
    void		            sRender() override;
};
