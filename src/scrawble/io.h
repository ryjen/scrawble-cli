#ifndef SCRAWBLE_INPUT_H
#define SCRAWBLE_INPUT_H

#include <scrawble/lexicon/move.h>

namespace scrawble
{
    class scrawble;
    class player;
    class game;
    class board;

    class io
    {
       public:
        virtual void update(game &game) = 0;
        virtual void render(game &game) = 0;
    };

    /**
     * other inputs can be defined
     */
    class terminal_io : public io
    {
       public:
        terminal_io();
        virtual ~terminal_io();
        void update(game &game);
        void render(game &game);

       private:
        void render(board &board);
        void render(player &player);
        void render_help();
        void render_select();
        void render_bonus_square(int bonus, int color) const;

        static const int FLAG_DIRTY = (1 << 0);

        lexicon::point pos_;
        int flags_;
    };
}

#endif
