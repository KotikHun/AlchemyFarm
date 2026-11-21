#include "Game.hpp"
#include <filesystem>

int main(int argc, char* argv[])
{
    if (argc > 0 && argv[0] != nullptr)
    {
        try {
            std::filesystem::path exeDir = std::filesystem::absolute(argv[0]).parent_path();
            if (!exeDir.empty()) {
                std::filesystem::current_path(exeDir);
            }
        } catch (...) {}
    }

    Game game;
    game.run();
    return 0;
}