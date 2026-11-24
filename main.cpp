#include "Game.hpp"
#include <filesystem>

int main(int argc, char* argv[])
{
    try {
        namespace fs = std::filesystem;
        // Если assets уже есть в текущей рабочей папке (при запуске из Visual Studio), не трогаем
        if (!fs::exists("assets"))
        {
            if (argc > 0 && argv[0] != nullptr)
            {
                fs::path exeDir = fs::absolute(argv[0]).parent_path();
                // 1. Если assets рядом с .exe (в релизной распакованной папке)
                if (fs::exists(exeDir / "assets"))
                {
                    fs::current_path(exeDir);
                }
                // 2. Если .exe запущен из x64/Release или x64/Debug внутри репозитория
                else if (fs::exists(exeDir.parent_path() / "assets"))
                {
                    fs::current_path(exeDir.parent_path());
                }
                else if (fs::exists(exeDir.parent_path().parent_path() / "assets"))
                {
                    fs::current_path(exeDir.parent_path().parent_path());
                }
            }
        }
    }
    catch (...) {}

    Game game;
    game.run();
    return 0;
}