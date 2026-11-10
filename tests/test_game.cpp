#include "Game.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
using namespace escape;
void check(bool condition,const char* message) {if(!condition) throw std::runtime_error(message);}
void unlock(Game& g) {
 g.move(1);g.solve("  PiAnO  ");g.move(2);g.solve("5");g.move(3);g.solve("30");
 g.move(4);g.solve("60");g.move(5);g.solve("echo");
}
int main() {
 try {
 auto time=Game::Clock::time_point{};
 auto now=[&]{return time;};
 Game g(600,now);
 check(g.current()==0&&g.outcome()==Outcome::Playing,"initial state");
 check(g.bfs().empty()&&g.dfs().empty(),"locked exit unreachable");
 g.move(7);check(g.current()==0&&g.score()==0,"nonadjacent blocked");
 g.back();check(g.current()==0,"empty backtrack");
 g.move(1);g.move(2);check(g.current()==1,"key door blocked");
 g.solve(" ");check(g.rooms()[1].puzzle.attempts==3,"empty answer free");
 g.solve("wrong");check(g.rooms()[1].puzzle.attempts==2&&g.score()==-11,"wrong penalty");
 g.hint();check(g.hintsLeft()==2&&g.score()==-26,"hint penalty");
 g.solve(" PIANO ");check(g.rooms()[1].puzzle.solved,"normalized answer");
 int score=g.score();g.solve("piano");check(g.score()==score,"no duplicate reward");
 g.move(2);g.back();check(g.current()==1,"stack back");
 g.back();check(g.current()==0,"stack unwinds");
 check(g.history().find("Study (2) -> Library (1) -> Entrance (0)")!=std::string::npos,"full travel history");
 Game winner(600,now);unlock(winner);
 check(winner.bfs()==std::vector<int>({5,6,7}),"BFS shortest route");
 auto path=winner.dfs();check(!path.empty()&&path.front()==5&&path.back()==7,"DFS exit route");
 winner.move(6);winner.move(7);
 check(winner.outcome()==Outcome::Escaped&&winner.score()==813,"win scoring");
 score=winner.score();winner.hint();winner.move(6);winner.solve("echo");
 check(winner.score()==score&&winner.current()==7,"terminal state immutable");
 Game exhausted(600,now);exhausted.move(1);
 exhausted.solve("bad");exhausted.solve("bad");exhausted.solve("bad");
 check(exhausted.outcome()==Outcome::Trapped,"attempt exhaustion loss");
 Game hints(600,now);hints.hint();check(hints.hintsLeft()==3,"no hint spent without puzzle");
 hints.move(1);hints.hint();hints.hint();hints.hint();score=hints.score();hints.hint();
 check(hints.hintsLeft()==0&&hints.score()==score,"global hint cap");
 Game timed(10,now);time+=std::chrono::seconds(10);
 check(timed.remaining()==0&&timed.outcome()==Outcome::TimedOut,"deadline boundary");
 timed.move(1);check(timed.current()==0,"expired action rejected");
 time=Game::Clock::time_point{};Game quit(600,now);quit.quit();check(quit.outcome()==Outcome::Quit,"quit state");
 bool rejected=false;try {Game bad(0,now);}catch(const std::invalid_argument&) {rejected=true;}
 check(rejected,"invalid timer rejected");
 std::cout<<"All game regression checks passed.\n";
 } catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return EXIT_FAILURE;}
}
