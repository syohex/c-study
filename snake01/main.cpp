#include <cassert>
#include <list>
#include <vector>
#include <random>
#include <algorithm>
#include <optional>
#include <ncurses.h>

namespace {

enum Action {
    kLeft,
    kRight,
    kUp,
    kDown,
};

struct Point {
    int x;
    int y;

    bool operator==(const Point &other) const noexcept {
        return x == other.x && y == other.y;
    }
};

class Snake {
  public:
    Snake(const std::vector<Point> points) {
        for (auto &p : points) {
            points_.push_back(std::move(p));
        }
    }

    bool Contains(const Point &p) const {
        return std::find(points_.begin(), points_.end(), p) != points_.end();
    }

    void Display() {
        for (const auto &p : points_) {
            mvaddch(p.y, p.x, ACS_BLOCK);
        }
    }

    Point NextPoint(Action action) const {
        Point p = points_.front();
        switch (action) {
        case Action::kLeft:
            p.x -= 1;
            break;
        case Action::kRight:
            p.x += 1;
            break;
        case Action::kUp:
            p.y -= 1;
            break;
        case Action::kDown:
            p.y += 1;
            break;
        default:
            // never reach here
            assert(false);
            break;
        }

        return p;
    }

    bool IsColliding(const Point &point) const noexcept {
        return std::find(points_.begin(), points_.end(), point) != points_.end();
    }

    void Prepend(const Point &next) {
        points_.push_front(next);
    }

    void Move(const Point &next) {
        points_.push_front(next);
        points_.pop_back();
    }

  private:
    std::list<Point> points_;
};

class Board {
  public:
    explicit Board(const Snake &snake, int rows, int columns, int init_foods)
        : snake_(snake), rows_(rows), columns_(columns), mt_(rd_()), row_dist_(0, rows_), col_dist_(0, columns_) {
        for (int i = 0; i < init_foods; ++i) {
            GenerateNewFood();
        }
    }

    void Display() {
        snake_.Display();

        for (const auto &p : foods_) {
            mvaddch(p.y, p.x, ACS_DIAMOND);
        }
    }

    bool MoveSnake(Action action) {
        const Point next = snake_.NextPoint(action);
        if (!IsValidPoint(next)) {
            return false;
        }

        if (snake_.IsColliding(next)) {
            return false;
        }

        auto it = std::find(foods_.begin(), foods_.end(), next);
        if (it != foods_.end()) {
            snake_.Prepend(*it);
            foods_.erase(it);
            GenerateNewFood();

            return true;
        }

        snake_.Move(next);
        return true;
    }

  private:
    void GenerateNewFood() {
        while (true) {
            const Point p{col_dist_(mt_), row_dist_(mt_)};
            if (!snake_.Contains(p) && !ContainsInFoods(p)) {
                foods_.push_back(p);
                return;
            }
        }
    }

    bool ContainsInFoods(const Point &p) const {
        return std::find(foods_.begin(), foods_.end(), p) != foods_.end();
    }

    bool IsValidPoint(const Point &p) const noexcept {
        return (p.x >= 0 && p.x < columns_ && p.y >= 0 && p.y < rows_);
    }

    Snake snake_;
    std::list<Point> foods_;
    int rows_;
    int columns_;
    std::random_device rd_;
    std::mt19937 mt_;
    std::uniform_int_distribution<int> row_dist_;
    std::uniform_int_distribution<int> col_dist_;
};

Action ReadNextAction(Action prev) {
    int ch = getch();
    switch (ch) {
    case KEY_LEFT:
        if (prev != Action::kRight) {
            return Action::kLeft;
        }
        break;
    case KEY_RIGHT:
        if (prev != Action::kLeft) {
            return Action::kRight;
        }
        break;
    case KEY_UP:
        if (prev != Action::kDown) {
            return Action::kUp;
        }
        break;
    case KEY_DOWN:
        if (prev != Action::kUp) {
            return Action::kDown;
        }
        break;
    default:
        break;
    }

    return prev;
}

} // namespace

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    constexpr int FOOD_COUNT = 6;
    const std::vector<Point> init_snake_position{{2, 3}, {2, 2}};

    Snake snake(init_snake_position);
    Board board(snake, max_y, max_x, FOOD_COUNT);

    Action action = Action::kRight;
    while (true) {
        clear();
        board.Display();
        refresh();

        action = ReadNextAction(action);
        if (!board.MoveSnake(action)) {
            break;
        }
    }

    endwin();
    return 0;
}