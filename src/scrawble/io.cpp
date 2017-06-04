#ifdef WIN32
#include <conio.h>
#else
#include <curses.h>
#endif
#include <scrawble/bag.h>
#include <scrawble/board.h>
#include <scrawble/game.h>
#include <signal.h>
#include <cstdlib>

#define HIGHLIGHT 8
#define KB_DOWN 0x42
#define KB_LEFT 0x44
#define KB_RIGHT 0x43
#define KB_UP 0x41

#define PLAYER_RACK_ROW 32
#define MIN_PLAYER_RACK_COLUMN 18
#define MAX_PLAYER_RACK_COLUMN 42
#define MIN_BOARD_COLUMN 2
#define MAX_BOARD_COLUMN 58
#define MIN_BOARD_ROW 1
#define MAX_BOARD_ROW 29

namespace scrawble
{
    terminal_io::terminal_io() : pos_(MIN_PLAYER_RACK_COLUMN, PLAYER_RACK_ROW), flags_(FLAG_DIRTY), selected_()
    {
        timeout(-1);
        setlocale(LC_CTYPE, "");
        initscr();
        raw();
        noecho();
        start_color();

        init_pair(COLOR_MAGENTA, COLOR_BLACK, COLOR_MAGENTA);
        init_pair(COLOR_RED, COLOR_BLACK, COLOR_RED);
        init_pair(COLOR_GREEN, COLOR_BLACK, COLOR_GREEN);
        init_pair(COLOR_YELLOW, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_CYAN, COLOR_BLACK, COLOR_CYAN);
        init_pair(COLOR_BLUE, COLOR_BLACK, COLOR_BLUE);
        init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);
    }

    terminal_io::~terminal_io()
    {
        clear();
        endwin();
    }

    int terminal_io::current_score() const
    {
        int score = 0;
        for (auto entry : moves_) {
            score += entry.second.score();
        }
        return score;
    }

    void terminal_io::update(game &game)
    {
        int ch = getch();

        if (ch == 0x1B) {
            ch = getch();
            if (ch == 0x5B) {
                ch = getch();
            }
        }

        switch (ch) {
            case 'f':
                game.finish_turn(current_score());
                moves_.clear();
                flags_ |= FLAG_DIRTY;
                break;
            case KEY_UP:
            case KB_UP:
                pos_.y = std::max(MIN_BOARD_ROW, pos_.y - (pos_.y == PLAYER_RACK_ROW ? 3 : 2));
                flags_ |= FLAG_DIRTY;
                break;
            case KEY_DOWN:
            case KB_DOWN:
                pos_.y = std::min(PLAYER_RACK_ROW, pos_.y + (pos_.y == MAX_BOARD_ROW ? 3 : 2));
                flags_ |= FLAG_DIRTY;
                break;
            case KEY_LEFT:
            case KB_LEFT:
                if (pos_.y == PLAYER_RACK_ROW) {
                    pos_.x = std::max(MIN_PLAYER_RACK_COLUMN, pos_.x - 4);
                } else {
                    pos_.x = std::max(MIN_BOARD_COLUMN, pos_.x - 4);
                }
                flags_ |= FLAG_DIRTY;
                break;
            case KEY_RIGHT:
            case KB_RIGHT:
                if (pos_.y == PLAYER_RACK_ROW) {
                    pos_.x = std::min(MAX_PLAYER_RACK_COLUMN, pos_.x + 4);
                } else {
                    pos_.x = std::min(MAX_BOARD_COLUMN, pos_.x + 4);
                }
                flags_ |= FLAG_DIRTY;
                break;
            case KEY_ENTER:
            case '\n':
                if (!selected_.valid()) {
                    selected_ = pos_;
                } else {
                    place_selection(game.board(), game.player());
                }
                flags_ |= FLAG_DIRTY;
                break;
            case 's':
                game.player().shuffle();
                flags_ |= FLAG_DIRTY;
                break;
            case 'h':
                render_hints(game);
                break;
            case 'b':
                raise(SIGTSTP);
                break;
            case 'q':
            case 'x':
                game.quit();
                break;
        }
    }

    void terminal_io::render(game &game)
    {
        if ((flags_ & FLAG_DIRTY) == 0) {
            return;
        }

        flags_ &= ~(FLAG_DIRTY);

        move(0, 0);

        render(game.board());

        render(game.player());

        render_help();

        render_select();
    }

    void terminal_io::render_help()
    {
        file_reader reader("assets/help.txt");

        int row = 2;
        for (auto line : reader) {
            mvprintw(row++, 66, "%s", line.c_str());
        }
    }

    void terminal_io::render_hints(game &game)
    {
        int row = 13;
        auto hints = game.hints();
        int cols = hints.size() / 17 + 1;
        int col = 66;
        auto it = hints.begin();
        for (int c = 0; c < cols && it != hints.end(); c++) {
            int pos = 0;
            while (it != hints.end()) {
                mvprintw(row++, col, "%s", it->word().c_str());
                ++it;
                if (++pos == 17) {
                    break;
                }
            }
            row = 13;
            col += 10;
        }
        move(pos_.y, pos_.x);
    }

    void terminal_io::render_select()
    {
        if (selected_.valid()) {
            move(selected_.y, selected_.x);
            chgat(1, A_BOLD, COLOR_PAIR(HIGHLIGHT), NULL);
        }
        move(pos_.y, pos_.x);
        chgat(1, A_BOLD, COLOR_PAIR(HIGHLIGHT), NULL);
    }

    void terminal_io::render(player &player)
    {
        auto select = translate_rack(selected_);
        int index = 0;
        printw("                ╭───┬───┬───┬───┬───┬───┬───╮\n");
        printw("                ");
        for (auto tile : player.rack()) {
            printw("│ ");
            int attr = A_BOLD | COLOR_PAIR(COLOR_WHITE);
            if (index++ == select) {
                attr |= A_BLINK;
            }
            attron(attr);
            printw("%c ", tile.letter());
            attroff(attr);
        }
        printw("│\n");
        printw("                ╰───┴───┴───┴───┴───┴───┴───╯\n");
    }

    void terminal_io::render(board &board)
    {
        auto select = translate_board(selected_);

        for (int i = 0; i < board::size; i++) {
            if (i == 0) {
                printw("╭───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───╮\n");
            } else {
                printw("├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤\n");
            }
            for (int j = 0; j < board::size; j++) {
                printw("│ ");
                if (board.value(i, j).empty()) {
                    switch (board.bonus(i, j, true)) {
                        case 2:
                            render_bonus_square(2, COLOR_MAGENTA);
                            break;
                        case 3:
                            render_bonus_square(3, COLOR_RED);
                            break;
                        default:
                            switch (board.bonus(i, j, false)) {
                                case 2:
                                    render_bonus_square(2, COLOR_CYAN);
                                    break;
                                case 3:
                                    render_bonus_square(3, COLOR_YELLOW);
                                    break;
                                default:
                                    printw(" ");
                                    break;
                            }
                    }
                } else {
                    int attr = A_BOLD | COLOR_PAIR(COLOR_WHITE);
                    if (select.x == j && select.y == i) {
                        attr |= A_BLINK;
                    }
                    attron(attr);
                    printw("%c", board.value(i, j).letter());
                    attroff(attr);
                }
                printw(" ");
            }
            printw("│\n");
        }
        printw("╰───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───╯\n");
    }

    void terminal_io::place_selection(board &board, player &plr)
    {
        if (!selected_.valid()) {
            return;
        }

        if (selected_.y == PLAYER_RACK_ROW) {
            if (pos_.y == PLAYER_RACK_ROW) {
                place_selection_from_rack_to_rack(plr);
            } else {
                place_selection_from_rack_to_board(board, plr);
            }
        } else {
            if (pos_.y == PLAYER_RACK_ROW) {
                place_selection_from_board_to_rack(board, plr);
            } else {
                place_selection_from_board_to_board(board);
            }
        }

        selected_ = lexicon::point();
    }

    void terminal_io::place_selection_from_rack_to_rack(player &plr)
    {
        if (!selected_.valid()) {
            return;
        }

        int index1 = translate_rack(selected_);

        int index2 = translate_rack(pos_);

        plr.swap(index2, index2);
    }

    void terminal_io::place_selection_from_rack_to_board(board &board, player &plr)
    {
        if (!selected_.valid()) {
            return;
        }

        int index = translate_rack(selected_);

        auto point = translate_board(pos_);

        auto tile = plr.pop(index);

        moves_[point] = tile;

        tile = board.place(point.y, point.x, tile);

        plr.replace(index, tile);
    }

    void terminal_io::place_selection_from_board_to_rack(board &board, player &plr)
    {
        if (!selected_.valid()) {
            return;
        }

        auto point = translate_board(selected_);

        int index = translate_rack(pos_);

        auto tile = board.value(point.y, point.x);

        tile = plr.replace(index, tile);

        moves_[point] = tile;

        board.place(point.y, point.x, tile);
    }

    void terminal_io::place_selection_from_board_to_board(board &board)
    {
        if (!selected_.valid()) {
            return;
        }

        auto p1 = translate_board(selected_);
        auto p2 = translate_board(pos_);

        auto tile = board.value(p1.y, p1.x);

        moves_[p2] = tile;

        tile = board.place(p2.y, p2.x, tile);

        moves_[p1] = tile;

        board.place(p1.y, p1.x, tile);
    }

    lexicon::point terminal_io::translate_board(const lexicon::point &value) const
    {
        if (value.y > MAX_BOARD_ROW) {
            return lexicon::point();
        }
        return lexicon::point((value.x - MIN_BOARD_COLUMN) / 4, (value.y - MIN_BOARD_ROW) / 2);
    }

    int terminal_io::translate_rack(const lexicon::point &value) const {
        if (value.y != PLAYER_RACK_ROW) {
            return -1;
        }
        return (value.x - MIN_PLAYER_RACK_COLUMN) / 4;
    }

    void terminal_io::render_bonus_square(int bonus, int color) const
    {
        attron(COLOR_PAIR(color));
        printw("%d", bonus);
        attroff(COLOR_PAIR(color));
    }
}
