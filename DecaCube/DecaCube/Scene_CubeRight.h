#pragma once

#include "Components.h"
#include "Scene.h"
#include "Vec2.h"
#include <queue>

struct SpawnPoint {
    std::string type;
    float y;
    auto operator<=>(const SpawnPoint& other) const {
        return y <=> other.y;
    }
};

struct LevelConfig {
    float playerSpeed{ 200.f };
    float enemySpeed{ 200.f };
};

class Scene_CubeRight : public Scene
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
public:
    Scene_CubeRight(GameEngine* gameEngine, const std::string& levelPath);
    void		            update(sf::Time dt) override;
    void		            sDoAction(const Command& command) override;
    void		            sRender() override;
};
