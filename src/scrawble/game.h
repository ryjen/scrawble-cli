#ifndef SCRAWBLE_GAME_H
#define SCRAWBLE_GAME_H

#include <curses.h>
#include <scrawble/game_config.h>
#include <scrawble/game_logic.h>
#include <scrawble/io.h>
#include <scrawble/tile.h>
#include <util/file_reader.h>
#include <json.hpp>

namespace scrawble
{
    class game : public game_logic
    {
       public:
        game() : state_(Running)
        {
        }

        ~game()
        {
        }

        void update()
        {
            term_.update(*this);
        }

        void render()
        {
            term_.render(*this);
        }

        void quit()
        {
            state_ = Stopped;
        }

        bool is_over() const
        {
            return state_ == Stopped;
        }

        void init_dictionary(const std::string &fileName)
        {
            file_reader reader(std::string(game_config::ASSETS_DIR) + fileName);

            for (auto line : reader) {
                dictionary_.push(line);
            }
        }

       private:
        typedef enum { Running, Stopped } state_type;

        terminal_io term_;
        state_type state_;
    };
}

#endif
