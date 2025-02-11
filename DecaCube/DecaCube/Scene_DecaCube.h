#pragma once

#include "Components.h"
#include "Scene.h"
#include "Vec2.h"
#include <queue>


struct SpawnPoint {
    std::string     type;
    float           y;
    auto operator<=>(const SpawnPoint& other) const {
        return  y <=> other.y;
    }
};

struct LevelConfig {
    float       playerSpeed{ 200.f };
    float       enemySpeed{ 200.f };
    
};

class Scene_DecaCube : public Scene
{
    sPtrEntt                            _player{ nullptr };
    sf::View                            _worldView; // camera
    sf::FloatRect                       _worldBounds;
    LevelConfig                         _config;
    std::priority_queue<SpawnPoint>     _spawnPoints;
    const Vec2                          _playerSpawn{ 5, 5 };
    bool                                _drawTextures{ true };
    bool                                _drawAABB{ false };
    const Vec2                          gridSize{ 40, 40 };
    bool                                _drawCam{ false };
    int                                 _lives;

    std::string                         _levelPath;
    std::string                         _nextControl;


    //systems

    void sMovement(sf::Time dt);
    void sAnimation();
    void sCollision();


    // helper functions
    void	                onEnd() override;
    void registerActions();
    void spawnPlayer(sf::Vector2f pos);
    void playerMovement();
    void adjustPlayerPosition();
    void init(const std::string& path);
    void loadLevel(const std::string& path);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
    void loadFromFile(const std::string& path);
    void snapToGrid(std::shared_ptr<Entity> entity);


public:
    Scene_DecaCube(GameEngine* gameEngine, const std::string& levelPath);
    void		            update(sf::Time dt) override;
    void		            sDoAction(const Command& command) override;
    void		            sRender() override;
};