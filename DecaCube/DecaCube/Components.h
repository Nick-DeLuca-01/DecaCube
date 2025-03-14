//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_COMPONENTS_H
#define BREAKOUT_COMPONENTS_H


#include "Animation.h"
#include <memory>
#include <SFML/Graphics.hpp>
#include "Utilities.h"


struct Component
{
    bool		has{ false };
    Component() = default;
};

struct CAnimation : public Component {
    Animation   animation;

    CAnimation() = default;
    CAnimation(const Animation& a) : animation(a) {}

};


struct CSprite : public Component {
    sf::Sprite sprite;

    CSprite() = default;
    CSprite(const sf::Texture& t)
        : sprite(t) {
        centerOrigin(sprite);
    }
    CSprite(const sf::Texture& t, sf::IntRect r)
        : sprite(t, r) {
        centerOrigin(sprite);
    }
};



struct CTransform : public Component
{

    Vec2	pos{ 0.f, 0.f };
    Vec2	prevPos{ 0.f, 0.f };
    Vec2	scale{ 1.f, 1.f };
    Vec2	vel{ 0.f, 0.f };

    float   angVel{ 0 };
    float	angle{ 0.f };

    CTransform() = default;
    CTransform(const Vec2& p) : pos(p) {}
    CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a)
        : pos(p), prevPos(p), vel(v), scale(sc), angle(a) {}

};

struct CLocation : public Component
{
    int currentFace{ 1 };
    
    CLocation() = default;
    CLocation(int face) : currentFace(face){}
};

struct CSight : public Component
{
    bool seesPlayer{ false };
    CSight() = default;
    CSight(bool see) : seesPlayer(see) {}
};

struct COffScreen : public Component 
{
    bool offScreen{ true };
    sf::Time secondsOffScreen{ sf::Time::Zero };
    sf::Time sceneChangeThreshold{ sf::Time::Zero };
    COffScreen() = default;
    COffScreen(bool isNotVisible) : offScreen(isNotVisible) {}
    COffScreen(bool isNotVisible, sf::Time seconds) : offScreen(isNotVisible), sceneChangeThreshold(seconds) {}
};

struct CCollision : public Component
{
    float radius{ 0.f };

    CCollision() = default;
    CCollision(float r)
        : radius(r) {}
};


struct CBoundingBox : public Component
{
    sf::Vector2f size{ 0.f, 0.f };
    sf::Vector2f halfSize{ 0.f, 0.f };

    CBoundingBox() = default;
    CBoundingBox(const sf::Vector2f& s) : size(s), halfSize(0.5f * s)
    {}
    CBoundingBox(float w, float h) : size(sf::Vector2f{ w,h }), halfSize(0.5f * size)
    {}
};



struct CInput : public Component
{
    bool up{ false };
    bool left{ false };
    bool right{ false };
    bool down{ false };

    bool spinr{ false };
    bool spinl{ false };

    Vec2 distanceRemainingPos{ 0, 0 }; //for left and down directions
    Vec2 distanceRemainingNeg{ 0, 0 }; //for right and up directions


    CInput() = default;
};


struct CScore : public Component
{
    int score{ 0 };
    CScore(int s = 0) : score(s) {}
};


struct CState : public Component {
    std::string state{ "none" };

    CState() = default;
    CState(const std::string& s) : state(s) {}
};

#endif //BREAKOUT_COMPONENTS_H
