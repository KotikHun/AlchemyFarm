#pragma once

#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <cstdint>

// ============================================
// ЗВУКОВОЙ МЕНЕДЖЕР (ПРОЦЕДУРНЫЕ ЭФФЕКТЫ И МУЗЫКА)
// ============================================
// Генерирует приятные звуки и фоновую музыку без внешних аудиофайлов:
// удары инструментов, щелчки интерфейса, звон монет, алхимическая варка,
// и уютный эмбиент/музыка фермы.

class SoundManager {
public:
    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer popBuffer;
    sf::SoundBuffer axeBuffer;
    sf::SoundBuffer pickaxeBuffer;
    sf::SoundBuffer hoeBuffer;
    sf::SoundBuffer waterBuffer;
    sf::SoundBuffer brewBuffer;
    sf::SoundBuffer coinBuffer;
    sf::SoundBuffer sleepBuffer;
    sf::SoundBuffer musicBuffer;

    float volume = 80.f;       // 0 - 100 (звуковые эффекты)
    float musicVolume = 60.f;  // 0 - 100 (фоновая музыка)

    std::vector<std::unique_ptr<sf::Sound>> activeSounds;
    std::unique_ptr<sf::Sound> bgMusic;

    SoundManager();

    void setVolume(float vol) {
        volume = std::clamp(vol, 0.f, 100.f);
    }

    void setMusicVolume(float vol);
    void startMusic();
    void stopMusic();

    void update();

    void playClick();
    void playPop();
    void playAxe();
    void playPickaxe();
    void playHoe();
    void playWater();
    void playBrew();
    void playCoin();
    void playSleep();

private:
    void playBuffer(const sf::SoundBuffer& buffer, float pitch = 1.0f, float relVol = 1.0f);
    void generateSounds();
};
