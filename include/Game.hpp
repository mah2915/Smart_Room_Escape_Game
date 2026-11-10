#ifndef ESCAPE_GAME_HPP
#define ESCAPE_GAME_HPP
#include <array>
#include <chrono>
#include <functional>
#include <stack>
#include <string>
#include <vector>
namespace escape {
constexpr int RoomCount=8, KeyCount=4;
struct Puzzle { std::string question, answer, hint; int rewardKey=-1; int attempts=3; bool solved=false; };
struct Room { std::string name, description; Puzzle puzzle; };
struct Door { int destination; int key=-1; int puzzleRoom=-1; };
enum class Outcome { Playing, Escaped, TimedOut, Trapped, Quit };
class Game {
public:
 using Clock=std::chrono::steady_clock;
 using Now=std::function<Clock::time_point()>;
 explicit Game(int seconds=600, Now now=[] {return Clock::now();});
 std::string look() const;
 std::string move(int destination);
 std::string back();
 std::string solve(const std::string& answer);
 std::string hint();
 std::string inventory() const;
 std::string status() const;
 std::string history() const;
 std::string route(bool useDfs=false) const;
 std::vector<int> bfs() const;
 std::vector<int> dfs() const;
 Outcome outcome() const;
 void quit();
 int current() const {return current_;}
 int score() const {return score_;}
 int hintsLeft() const {return hints_;}
 int remaining() const;
 const std::array<Room,RoomCount>& rooms() const {return rooms_;}
private:
 std::array<Room,RoomCount> rooms_;
 std::array<std::vector<Door>,RoomCount> graph_;
 std::array<bool,KeyCount> keys_{};
 std::stack<int> backtrack_;
 std::vector<int> trail_{0};
 int current_=0,score_=0,hints_=3,limit_;
 Now now_; Clock::time_point start_;
 Outcome terminal_=Outcome::Playing;
 bool open(const Door& door) const;
 bool reachableWithRemainingPuzzles() const;
 std::string pathText(const std::vector<int>& path) const;
};
}
#endif
