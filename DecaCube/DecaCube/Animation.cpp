//
// Created by David Burchill on 2024-11-26.
//

#include "Animation.h"
#include "Utilities.h"


Animation::Animation(const std::string& name,
    const sf::Texture& t,
    std::vector<sf::IntRect> frames,
    sf::Time tpf,
    bool repeats)
    : _name(name)
    , _frames(frames)
    , _timePerFrame(tpf)
    , _isRepeating(repeats)
    , _countDown(sf::Time::Zero)
    , _sprite(t, _frames[0]) {
    centerOrigin(_sprite);

    std::cout << name << " tpf: " << _timePerFrame.asMilliseconds() << "ms\n";
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
    : _name(name)
    , _sprite(t)
    , _frames(frameCount)
    , _currentFrame(0)
    , _speed(speed)
{
    _size = Vec2(static_cast<float>(t.getSize().x) / frameCount, static_cast<float>(t.getSize().y));
    _sprite.setOrigin(_size.x / 2.f, _size.y / 2.f);
    _sprite.setTextureRect(sf::IntRect(std::floor(_currentFrame * _size.x), 0, _size.x, _size.y));
}


void Animation::update(sf::Time dt) {

    _countDown -= dt;
    if (_countDown < sf::Time::Zero) {
        _countDown = _timePerFrame;
        _currentFrame += 1;

        if (_currentFrame == _frames.size() && !_isRepeating)
            return;  // on the last frame of non-repeating animaton, leave it
        else
            _currentFrame = (_currentFrame % _frames.size());

        _sprite.setTextureRect(_frames[_currentFrame]);
        centerOrigin(_sprite);
    }
}


bool Animation::hasEnded() const {
    return (_currentFrame >= _frames.size());
}


const std::string& Animation::getName() const {
    return _name;
}


sf::Sprite& Animation::getSprite() {
    return _sprite;
}

sf::Vector2f Animation::getBB() const {
    return static_cast<sf::Vector2f>(_frames[_currentFrame].getSize());
}
