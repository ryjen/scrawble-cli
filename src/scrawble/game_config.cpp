#include <scrawble/game_config.h>
#include <util/file_reader.h>

namespace scrawble
{
    void game_config::load()
    {
        load(DEFAULT_CONFIG_FILE);
    }

    void game_config::load(const std::string &filepath)
    {
        file_reader input(std::string(ASSETS_DIR) + filepath);

        auto j = input.to_json();

        dictionary_ = j["dictionary"];

        auto tiles = j["tiles"];

        for (auto tile : tiles) {
            int score = tile["score"];

            auto distributions = tile["letter_distributions"];

            for (auto it = distributions.begin(); it != distributions.end(); ++it) {
                letters_.emplace_back(score, it.value(), it.key()[0]);
            }
        }
    }
}