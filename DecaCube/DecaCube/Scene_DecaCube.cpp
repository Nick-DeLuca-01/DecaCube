#include "Scene_DecaCube.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include "GameEngine.h"

void Scene_DecaCube::onEnd()
{
    _game->changeScene("MENU", nullptr, false);
}

Scene_DecaCube::Scene_DecaCube(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine), _worldView(gameEngine->window().getDefaultView()) {

}

void Scene_DecaCube::update(sf::Time dt)
{
}

void Scene_DecaCube::sDoAction(const Command& command)
{
}

void Scene_DecaCube::sRender()
{
}
