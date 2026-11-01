#include "SoundManager.hpp"
#include <cmath>
#include <cstdlib>

static constexpr unsigned int SAMPLE_RATE = 44100;
static constexpr double PI = 3.14159265358979323846;

SoundManager::SoundManager()
{
    generateSounds();
    startMusic();
}

void SoundManager::update()
{
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](const std::unique_ptr<sf::Sound>& s) {
                return s->getStatus() == sf::Sound::Status::Stopped;
            }),
        activeSounds.end()
    );
}

void SoundManager::startMusic()
{
    if (!bgMusic)
    {
        bgMusic = std::make_unique<sf::Sound>(musicBuffer);
        bgMusic->setLooping(true);
    }
    bgMusic->setVolume(musicVolume * 0.40f);
    if (bgMusic->getStatus() != sf::Sound::Status::Playing)
    {
        bgMusic->play();
    }
}

void SoundManager::setMusicVolume(float vol)
{
    musicVolume = std::clamp(vol, 0.f, 100.f);
    if (bgMusic)
    {
        bgMusic->setVolume(musicVolume * 0.40f);
        if (musicVolume <= 0.1f && bgMusic->getStatus() == sf::Sound::Status::Playing)
        {
            bgMusic->pause();
        }
        else if (musicVolume > 0.1f && bgMusic->getStatus() != sf::Sound::Status::Playing)
        {
            bgMusic->play();
        }
    }
}

void SoundManager::stopMusic()
{
    if (bgMusic)
    {
        bgMusic->stop();
    }
}

void SoundManager::playBuffer(const sf::SoundBuffer& buffer, float pitch, float relVol)
{
    if (volume <= 0.1f) return;
    try
    {
        auto sound = std::make_unique<sf::Sound>(buffer);
        sound->setVolume(volume * relVol);
        sound->setPitch(pitch);
        sound->play();
        activeSounds.push_back(std::move(sound));
    }
    catch (...)
    {
    }
}

void SoundManager::playClick()
{
    playBuffer(clickBuffer, 0.95f + ((rand() % 10) - 5) * 0.02f, 0.8f);
}

void SoundManager::playPop()
{
    playBuffer(popBuffer, 0.95f + ((rand() % 10) - 5) * 0.02f, 0.9f);
}

void SoundManager::playAxe()
{
    playBuffer(axeBuffer, 0.93f + ((rand() % 14) - 7) * 0.02f, 1.0f);
}

void SoundManager::playPickaxe()
{
    playBuffer(pickaxeBuffer, 0.95f + ((rand() % 12) - 6) * 0.02f, 0.95f);
}

void SoundManager::playHoe()
{
    playBuffer(hoeBuffer, 0.96f + ((rand() % 8) - 4) * 0.02f, 0.85f);
}

void SoundManager::playWater()
{
    playBuffer(waterBuffer, 0.98f + ((rand() % 6) - 3) * 0.02f, 0.9f);
}

void SoundManager::playBrew()
{
    playBuffer(brewBuffer, 1.0f, 1.0f);
}

void SoundManager::playCoin()
{
    playBuffer(coinBuffer, 0.98f + ((rand() % 6) - 3) * 0.02f, 1.0f);
}

void SoundManager::playSleep()
{
    playBuffer(sleepBuffer, 1.0f, 1.0f);
}

void SoundManager::generateSounds()
{
    const std::vector<sf::SoundChannel> monoMap = { sf::SoundChannel::Mono };

    // 1. Щелчок UI (Click) - 0.035 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.035);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double freq = 900.0 - 550.0 * (t / 0.035);
            double env = std::exp(-t * 110.0);
            double s = std::sin(2.0 * PI * freq * t) * env;
            samples[i] = static_cast<std::int16_t>(s * 20000.0);
        }
        (void)clickBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 2. Всплывающий звук (Pop) - 0.05 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.05);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double freq = 320.0 + 400.0 * (t / 0.05);
            double env = std::sin((t / 0.05) * PI);
            double s = std::sin(2.0 * PI * freq * t) * env;
            samples[i] = static_cast<std::int16_t>(s * 22000.0);
        }
        (void)popBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 3. Удар топора по дереву (Axe) - 0.12 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.12);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double env = std::exp(-t * 32.0);
            double noise = ((rand() % 2000) / 1000.0 - 1.0) * 0.45;
            double tone = std::sin(2.0 * PI * 130.0 * t) * 0.55;
            double s = (noise + tone) * env;
            samples[i] = static_cast<std::int16_t>(s * 26000.0);
        }
        (void)axeBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 4. Удар кирки по камню (Pickaxe) - 0.11 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.11);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double pingEnv = std::exp(-t * 50.0);
            double crunchEnv = std::exp(-t * 28.0);
            double ping = std::sin(2.0 * PI * 1750.0 * t) * 0.5 * pingEnv;
            double crunch = (((rand() % 2000) / 1000.0 - 1.0)) * 0.5 * crunchEnv;
            double s = ping + crunch;
            samples[i] = static_cast<std::int16_t>(s * 25000.0);
        }
        (void)pickaxeBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 5. Вспашка земли мотыгой (Hoe) - 0.10 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.10);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double env = std::sin((t / 0.10) * PI) * std::exp(-t * 12.0);
            double noise = (((rand() % 2000) / 1000.0 - 1.0)) * 0.5;
            double low = std::sin(2.0 * PI * 110.0 * t) * 0.5;
            double s = (noise + low) * env;
            samples[i] = static_cast<std::int16_t>(s * 21000.0);
        }
        (void)hoeBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 6. Полив водой (Water) - 0.16 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.16);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double env = std::sin((t / 0.16) * PI);
            double bubbleFreq = 480.0 + 160.0 * std::sin(2.0 * PI * 28.0 * t);
            double tone = std::sin(2.0 * PI * bubbleFreq * t) * 0.6;
            double noise = (((rand() % 2000) / 1000.0 - 1.0)) * 0.4;
            double s = (tone + noise) * env;
            samples[i] = static_cast<std::int16_t>(s * 20000.0);
        }
        (void)waterBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 7. Алхимическая варка / магия (Brew) - 0.38 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.38);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double env = std::sin((t / 0.38) * PI);

            // 4 арпеджио-ноты (C6, E6, G6, C7)
            double noteFreq = 1046.5;
            if (t > 0.08) noteFreq = 1318.5;
            if (t > 0.18) noteFreq = 1568.0;
            if (t > 0.28) noteFreq = 2093.0;

            double chime = std::sin(2.0 * PI * noteFreq * t) * 0.55;
            double bubble = std::sin(2.0 * PI * (380.0 + 80.0 * std::sin(2.0 * PI * 40.0 * t)) * t) * 0.35;
            double s = (chime + bubble) * env;
            samples[i] = static_cast<std::int16_t>(s * 22000.0);
        }
        (void)brewBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 8. Звон золотых монет (Coin) - 0.22 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.22);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double env = std::exp(-t * 18.0);
            double tone1 = std::sin(2.0 * PI * 1320.0 * t) * 0.55;
            double tone2 = std::sin(2.0 * PI * 1760.0 * t) * 0.45;
            double s = (tone1 + tone2) * env;
            samples[i] = static_cast<std::int16_t>(s * 24000.0);
        }
        (void)coinBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 9. Ночная колыбельная / сон (Sleep) - 0.70 сек
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 0.70);
        std::vector<std::int16_t> samples(total);
        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            double env = std::sin((t / 0.70) * PI);
            // Аккорд E-G#-B
            double c1 = std::sin(2.0 * PI * 329.63 * t) * 0.35;
            double c2 = std::sin(2.0 * PI * 415.30 * t) * 0.35;
            double c3 = std::sin(2.0 * PI * 493.88 * t) * 0.30;
            double s = (c1 + c2 + c3) * env;
            samples[i] = static_cast<std::int16_t>(s * 20000.0);
        }
        (void)sleepBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }

    // 10. Фоновая музыка (атмосферная мирная ферма) - 16.0 секунд бесшовный цикл
    {
        unsigned int total = static_cast<unsigned int>(SAMPLE_RATE * 16.0);
        std::vector<std::int16_t> samples(total, 0);

        struct Chord {
            double root;
            std::vector<double> notes;
        };

        Chord chords[4] = {
            { 130.81, { 261.63, 329.63, 392.00, 493.88 } }, // Cmaj7
            { 110.00, { 220.00, 261.63, 329.63, 392.00 } }, // Am7
            {  87.31, { 174.61, 220.00, 261.63, 329.63 } }, // Fmaj7
            {  98.00, { 196.00, 246.94, 293.66, 349.23 } }  // G7
        };

        // Мелодические колокольчики каждые 0.5s
        std::vector<double> melodyNotes = {
            523.25, 659.25, 783.99, 987.77,  659.25, 783.99, 987.77, 1046.50, // C bar
            440.00, 523.25, 659.25, 783.99,  523.25, 659.25, 783.99, 880.00,  // Am bar
            349.23, 440.00, 523.25, 659.25,  440.00, 523.25, 659.25, 698.46,  // F bar
            392.00, 493.88, 587.33, 698.46,  493.88, 587.33, 783.99, 523.25   // G bar
        };

        for (unsigned int i = 0; i < total; i++)
        {
            double t = static_cast<double>(i) / SAMPLE_RATE;
            int chordIdx = static_cast<int>(t / 4.0) % 4;
            double chordTime = std::fmod(t, 4.0);

            // 1. Теплый мягкий бас
            double bass = std::sin(2.0 * PI * chords[chordIdx].root * t) * 0.28;

            // 2. Мягкий гармонический пад
            double pad = 0.0;
            double padEnv = std::sin((chordTime / 4.0) * PI);
            for (double freq : chords[chordIdx].notes)
            {
                pad += std::sin(2.0 * PI * freq * t) * 0.08 * padEnv;
                pad += std::sin(2.0 * PI * (freq * 2.0) * t) * 0.02 * padEnv;
            }

            // 3. Мелодические щипковые ноты
            int noteIdx = static_cast<int>(t / 0.5) % 32;
            double noteTime = std::fmod(t, 0.5);
            double noteFreq = melodyNotes[noteIdx];
            double chimeEnv = std::exp(-noteTime * 7.5);
            double chime = (std::sin(2.0 * PI * noteFreq * noteTime) * 0.16 +
                            std::sin(2.0 * PI * noteFreq * 2.0 * noteTime) * 0.04) * chimeEnv;

            // 4. Легкий шелест природы
            double nature = (((rand() % 2000) / 1000.0 - 1.0)) * 0.008;

            double mixed = bass + pad + chime + nature;
            // Бесшовное сглаживание по краям
            double loopEnv = 1.0;
            if (t < 0.1) loopEnv = t / 0.1;
            else if (t > 15.9) loopEnv = (16.0 - t) / 0.1;

            samples[i] = static_cast<std::int16_t>(std::clamp(mixed * loopEnv * 20000.0, -32767.0, 32767.0));
        }

        (void)musicBuffer.loadFromSamples(samples.data(), samples.size(), 1, SAMPLE_RATE, monoMap);
    }
}
