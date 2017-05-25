#include <getopt.h>
#include <scrawble/game.h>
#include <scrawble/game_config.h>
#include <cstdio>

int main(int argc, char *argv[])
{
    scrawble::game game;
    scrawble::game_config config;

    static struct option long_options[] = {{"config", required_argument, 0, 'c'}, {0, 0, 0, 0}};

    int opt = getopt_long(argc, argv, "c:", long_options, NULL);

    std::cout << "Loading, please wait...\n";

    switch (opt) {
        case 'c':
            config.load(optarg);
            break;
        default:
            config.load();
            break;
    }

    game.init(config);

    do {
        game.render();

        game.update();
    } while (!game.is_over());

    return EXIT_SUCCESS;
}