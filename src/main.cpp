#include "Game.hpp"
#include <iostream>
#include <sstream>
int main() {
 escape::Game game;
 std::cout<<"SMART ESCAPE ROOM\nEscape in 600 seconds. Solve each puzzle within 3 attempts.\n"
          <<"Commands: look, go <room ID>, back, solve <answer>, hint, inventory,\n"
          <<"          status, history, bfs, dfs, help, quit\n"<<game.look();
 std::string line;
 while(game.outcome()==escape::Outcome::Playing) {
  std::cout<<"\n"<<game.status()<<"\n> "<<std::flush;
  if(!std::getline(std::cin,line)) {game.quit();break;}
  if(game.outcome()!=escape::Outcome::Playing) break;
  std::istringstream input(line);std::string cmd;input>>cmd;
  if(cmd=="look") std::cout<<game.look();
  else if(cmd=="go") {int id;std::string extra;if(!(input>>id)||(input>>extra))std::cout<<"Usage: go <room ID>";else std::cout<<game.move(id);}
  else if(cmd=="solve") {std::string answer;std::getline(input,answer);std::cout<<game.solve(answer);}
  else if(cmd=="back") std::cout<<game.back();
  else if(cmd=="hint") std::cout<<game.hint();
  else if(cmd=="inventory") std::cout<<game.inventory();
  else if(cmd=="status") std::cout<<game.status();
  else if(cmd=="history") std::cout<<game.history();
  else if(cmd=="bfs"||cmd=="dfs") std::cout<<game.route(cmd=="dfs");
  else if(cmd=="quit") game.quit();
  else if(cmd=="help") std::cout<<"look | go <ID> | back | solve <answer> | hint | inventory | status | history | bfs | dfs | quit\nSolve puzzles to collect reusable keys. Use room IDs shown under Doors.\n+100 per puzzle; -10 per wrong answer; -15 per hint; -1 per move/back.\nEscape: +200 plus remaining seconds / 5. Three hints total.\nBFS/DFS use only doors unlocked with your current keys and solved puzzles.";
  else if(!cmd.empty()) std::cout<<"Unknown command. Type help.";
  std::cout<<'\n';
 }
 switch(game.outcome()) {
 case escape::Outcome::Escaped:std::cout<<"YOU ESCAPED!\n";break;
 case escape::Outcome::TimedOut:std::cout<<"TIME UP. You did not escape.\n";break;
 case escape::Outcome::Trapped:std::cout<<"GAME OVER. An essential puzzle has no attempts left.\n";break;
 default:std::cout<<"Game ended.\n";
 }
 std::cout<<game.status()<<'\n'<<game.history()<<'\n';
}
