#include <scrawble/game.h>
#include <scrawble/game_config.h>
#include <cstdio>

int main(int argc, char *argv[])
{
    scrawble::game game;
    scrawble::game_config config;

    std::cout << "Loading, please wait...\n";

    config.load();

    game.init(config);

    do {
        game.render();

        game.update();
    } while (!game.is_over());

    return EXIT_SUCCESS;
}