#pragma once

#include "Components.h"
#include "Scene.h"
#include <queue>


struct SpawnPoint {
    std::string     type;
    float           y;
    auto operator<=>(const SpawnPoint& other) const {
        return  y <=> other.y;
    }
};

struct LevelConfig {
    float       scrollSpeed{ 100.f };
    float       playerSpeed{ 200.f };
    float       enemySpeed{ 200.f };
    float       bulletSpeed{ 400.f };
    float       missileSpeed{ 150.f };
    sf::Time    fireInterval{ sf::seconds(5) };
    sf::Time    shieldUpTime{ sf::milliseconds(300) };
    sf::Time    shieldCooldownTime{ sf::milliseconds(4000) };
};

class Scene_DecaCube : public Scene
{
    sPtrEntt                            _player{ nullptr };
    sf::View                            _worldView; // camera
    sf::FloatRect                       _worldBounds;
    LevelConfig                         _config;
    std::priority_queue<SpawnPoint>     _spawnPoints;
    bool                                _drawTextures{ true };
    bool                                _drawAABB{ false };
    bool                                _drawCam{ false };

    //systems


    // helper functions
    void	                onEnd() override;


public:
    Scene_DecaCube(GameEngine* gameEngine, const std::string& levelPath);
    void		            update(sf::Time dt) override;
    void		            sDoAction(const Command& command) override;
    void		            sRender() override;
};