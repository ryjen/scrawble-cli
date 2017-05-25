#ifndef SCRAWBLE_GAME_CONFIG_H
#define SCRAWBLE_GAME_CONFIG_H

#include <scrawble/config.h>
#include <json.hpp>

namespace scrawble
{
    class game_config : public config
    {
       public:
        static constexpr const char *ASSETS_DIR = "assets/";
        static constexpr const char *DEFAULT_CONFIG_FILE = "english.json";

        void load(const std::string &filepath);

        void load();
    };
};

#endif
