//
// Created by David Burchill on 2022-11-29.
//

#include "MusicPlayer.h"
#include <stdexcept>


MusicPlayer::MusicPlayer() {
    m_filenames["menuTheme"] = "../assets/music/menu.wav";
    m_filenames["winTheme"] = "../assets/music/win.wav";
    m_filenames["loseTheme"] = "../assets/music/lose.wav";
    m_filenames["danger0"] = "../assets/music/decacube_danger0.wav";
    m_filenames["danger1"] = "../assets/music/decacube_danger1.wav";
    m_filenames["danger2"] = "../assets/music/decacube_danger2.wav";
    m_filenames["danger3"] = "../assets/music/decacube_danger3.wav";
    m_filenames["danger4"] = "../assets/music/decacube_danger4.wav";
    m_filenames["danger5"] = "../assets/music/decacube_danger5.wav";
}

void MusicPlayer::addSong(const std::string& name, const std::string& path) {
    m_filenames[name] = path;
}

MusicPlayer& MusicPlayer::getInstance() {
    static MusicPlayer instance;          // Meyers Singleton implementation
    return instance;
}


void MusicPlayer::play(String theme) {
    if (!m_music.openFromFile(m_filenames[theme]))
        throw std::runtime_error("Music could not open file");

    m_music.setVolume(m_volume);
    m_music.setLoop(true);
    m_music.play();
}


void MusicPlayer::stop() {
    m_music.stop();
}


void MusicPlayer::setPaused(bool paused) {
    if (paused)
        m_music.pause();
    else
        m_music.play();
}


void MusicPlayer::setVolume(float volume) {
    m_volume = volume;
    m_music.setVolume(m_volume);
}
