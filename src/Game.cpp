#include "Game.hpp"
#include <algorithm>
#include <cctype>
#include <queue>
#include <sstream>
#include <stdexcept>
namespace escape {
namespace {
const std::array<std::string,KeyCount> keyNames={"Brass Key","Silver Key","Gear Key","Exit Key"};
std::string normalize(std::string s) {
 std::string result;
 for(unsigned char c:s) if(!std::isspace(c)) result+=static_cast<char>(std::tolower(c));
 return result;
}
}
Game::Game(int seconds,Now now):limit_(seconds),now_(std::move(now)),start_(now_()) {
 if(seconds<=0) throw std::invalid_argument("Time limit must be positive");
 rooms_={{
 {"Entrance","The entrance has sealed behind you. Find the exit through the building.",{}},
 {"Library","Dusty books surround a brass-key puzzle.",{"What has keys but cannot open locks? (one word)","piano","It is a musical instrument.",0}},
 {"Study","A silver key rests inside a mathematical lock.",{"Solve: 3x + 7 = 22. Enter x.","5","Subtract 7, then divide by 3.",1}},
 {"Gallery","Paintings conceal a mechanism controlling the workshop door.",{"Continue the sequence: 2, 6, 12, 20, ?","30","The differences are 4, 6, 8, then 10.",-1}},
 {"Workshop","A gear key lies behind a number lock.",{"Enter the smallest positive number divisible by 3, 4 and 5.","60","Find the least common multiple.",2}},
 {"Observatory","The final key is sealed in a star-shaped chest.",{"I speak without a mouth and hear without ears. What am I?","echo","You hear it when a sound reflects.",3}},
 {"Exit Hall","The final door needs the Exit Key. A corridor loops back to the gallery.",{}},
 {"Outside","Fresh air! You escaped the building.",{}}
 }};
 auto connect=[this](int a,int b,int key=-1,int puzzle=-1){graph_[a].push_back({b,key,puzzle}); graph_[b].push_back({a,key,puzzle});};
 connect(0,1); connect(0,3); connect(1,2,0); connect(2,3,0); connect(3,4,-1,3);
 connect(4,5,1); connect(5,6,2,2); connect(3,6,2); connect(6,7,3);
}
bool Game::open(const Door& d) const {
 return (d.key<0||keys_[d.key])&&(d.puzzleRoom<0||rooms_[d.puzzleRoom].puzzle.solved);
}
int Game::remaining() const {
 const auto elapsed=std::chrono::duration_cast<std::chrono::seconds>(now_()-start_).count();
 return static_cast<int>(std::max<long long>(0,limit_-elapsed));
}
bool Game::reachableWithRemainingPuzzles() const {
 // Compute a fixed point: collect every still-obtainable key and puzzle unlock.
 auto keys=keys_; std::array<bool,RoomCount> solved{},seen{};
 for(int i=0;i<RoomCount;++i) solved[i]=rooms_[i].puzzle.solved;
 seen[current_]=true; bool changed=true;
 while(changed) {
  changed=false;
  for(int i=0;i<RoomCount;++i) if(seen[i]) {
   const auto& p=rooms_[i].puzzle;
   if(!p.question.empty()&&(p.solved||p.attempts>0)) {
    if(!solved[i]) {solved[i]=true;changed=true;}
    if(p.rewardKey>=0&&!keys[p.rewardKey]) {keys[p.rewardKey]=true;changed=true;}
   }
   for(const auto& d:graph_[i]) if(!seen[d.destination]&&(d.key<0||keys[d.key])&&(d.puzzleRoom<0||solved[d.puzzleRoom])) {
    seen[d.destination]=true;changed=true;
   }
  }
 }
 return seen[7];
}
Outcome Game::outcome() const {
 if(terminal_!=Outcome::Playing) return terminal_;
 if(remaining()==0) return Outcome::TimedOut;
 if(!reachableWithRemainingPuzzles()) return Outcome::Trapped;
 return Outcome::Playing;
}
std::string Game::look() const {
 std::ostringstream out; const auto& r=rooms_[current_];
 out<<"\n["<<current_<<"] "<<r.name<<"\n"<<r.description<<"\nDoors:\n";
 for(const auto& d:graph_[current_]) {
  out<<"  "<<d.destination<<" - "<<rooms_[d.destination].name;
  if(open(d)) out<<" [open]";
  else {out<<" [locked:"; if(d.key>=0) out<<" "<<keyNames[d.key]; if(d.puzzleRoom>=0) out<<" solve "<<rooms_[d.puzzleRoom].name<<" puzzle";out<<"]";}
  out<<"\n";
 }
 if(!r.puzzle.question.empty()) {
  if(r.puzzle.solved) out<<"Puzzle solved.\n";
  else out<<"Puzzle ("<<r.puzzle.attempts<<" attempts left): "<<r.puzzle.question<<"\n";
 }
 return out.str();
}
std::string Game::move(int destination) {
 if(outcome()!=Outcome::Playing) return "The game has ended.";
 for(const auto& d:graph_[current_]) if(d.destination==destination) {
  if(!open(d)) return "That door is locked. Check its requirements with look.";
  backtrack_.push(current_); current_=destination;trail_.push_back(current_);score_-=1;
  if(current_==7) {score_+=200+remaining()/5;terminal_=Outcome::Escaped;}
  return look();
 }
 return "No connected door has that room ID.";
}
std::string Game::back() {
 if(outcome()!=Outcome::Playing) return "The game has ended.";
 if(backtrack_.empty()) return "No previous room to return to.";
 current_=backtrack_.top();backtrack_.pop();trail_.push_back(current_);--score_;return look();
}
std::string Game::solve(const std::string& answer) {
 if(outcome()!=Outcome::Playing) return "The game has ended.";
 auto& p=rooms_[current_].puzzle;
 if(p.question.empty()) return "There is no puzzle in this room.";
 if(p.solved) return "You already solved this puzzle.";
 if(p.attempts==0) return "No attempts remain.";
 if(normalize(answer).empty()) return "Enter an answer; no attempt used.";
 if(normalize(answer)!=p.answer) {--p.attempts;score_-=10;return "Incorrect. Attempts left: "+std::to_string(p.attempts);}
 p.solved=true;score_+=100;
 if(p.rewardKey>=0) {keys_[p.rewardKey]=true;return "Correct! +100 points. Collected "+keyNames[p.rewardKey]+".";}
 return "Correct! +100 points. The workshop door is now unlocked.";
}
std::string Game::hint() {
 if(outcome()!=Outcome::Playing) return "The game has ended.";
 const auto& p=rooms_[current_].puzzle;
 if(p.question.empty()||p.solved||p.attempts==0) return "No active puzzle here.";
 if(hints_==0) return "No hints remain.";
 --hints_;score_-=15;return p.hint+" (-15 points; "+std::to_string(hints_)+" hints left)";
}
std::string Game::inventory() const {
 std::string s="Keys (reusable):"; bool any=false;
 for(int i=0;i<KeyCount;++i) if(keys_[i]) {s+="\n  "+keyNames[i];any=true;}
 return any?s:s+" none";
}
std::string Game::status() const {return "Score: "+std::to_string(score_)+" | Time left: "+std::to_string(remaining())+"s | Hints: "+std::to_string(hints_);}
std::string Game::pathText(const std::vector<int>& p) const {
 std::string s;for(int id:p) {if(!s.empty()) s+=" -> ";s+=rooms_[id].name+" ("+std::to_string(id)+")";}return s;
}
std::string Game::history() const {return "Route taken: "+pathText(trail_);}
std::vector<int> Game::bfs() const {
 std::array<int,RoomCount> parent;parent.fill(-1);std::queue<int> q;q.push(current_);parent[current_]=current_;
 while(!q.empty()) {int u=q.front();q.pop();for(const auto& d:graph_[u]) if(open(d)&&parent[d.destination]==-1) {parent[d.destination]=u;q.push(d.destination);}}
 if(parent[7]==-1) return {};
 std::vector<int> path;for(int u=7;;u=parent[u]) {path.push_back(u);if(u==current_) break;}
 std::reverse(path.begin(),path.end());return path;
}
std::vector<int> Game::dfs() const {
 std::array<bool,RoomCount> seen{};std::vector<int> path;
 std::function<bool(int)> visit=[&](int u){seen[u]=true;path.push_back(u);if(u==7)return true;
  for(const auto& d:graph_[u]) if(open(d)&&!seen[d.destination]&&visit(d.destination))return true;
  path.pop_back();return false;};
 visit(current_);return path;
}
std::string Game::route(bool useDfs) const {
 auto path=useDfs?dfs():bfs();
 return path.empty()?"No route to the exit using currently unlocked doors. Explore and solve more puzzles.":(useDfs?"DFS route: ":"BFS shortest unlocked route: ")+pathText(path);
}
void Game::quit() {if(outcome()==Outcome::Playing) terminal_=Outcome::Quit;}
}
