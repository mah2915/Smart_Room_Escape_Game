# Smart Escape Room Game

A text-based C++17 escape game implementing the uploaded Data Structures proposal for FAST NUCES Karachi, Section 3J.

**Group:** 25K-0987 Maham Ali; 25K-0844 Taskeen Zainab.

Explore eight connected rooms, solve five puzzles, collect four reusable keys, and escape within ten minutes. The program uses only the standard C++ library. Puzzle content, room layout, scoring values and the ten-minute duration are implementation choices because the proposal does not specify them.

## Build and play on Windows

Install a modern MinGW-w64/GCC compiler with C++17 support and add its `bin` folder to PATH. In the project folder, run:

```bat
build.bat
build\smart_escape_room.exe
```

For VS Code, open this folder and use its terminal to run these commands. VS Code does not include a compiler. In Dev-C++, create a console C++ project, add `src/Game.cpp` and `src/main.cpp`, add `include` to compiler include directories and enable `-std=c++17`. Older bundled compilers may need updating.

Alternatively, with CMake and Visual Studio Build Tools:

```bat
cmake -S . -B build
cmake --build build --config Release
build\Release\smart_escape_room.exe
ctest --test-dir build -C Release --output-on-failure
```

For a single-configuration CMake generator, the executable is directly in `build`.

## Build on Linux or macOS

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/smart_escape_room
```

Without CMake:

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/Game.cpp src/main.cpp -o smart_escape_room
./smart_escape_room
```

## Commands

| Command | Action |
|---|---|
| `look` | Show current room, puzzle and adjacent door requirements |
| `go 1` | Move through an adjacent door using its numeric room ID |
| `solve piano` | Submit an answer to the current room's puzzle |
| `hint` | Use one of three shared hints |
| `inventory` | Show collected keys |
| `back` | Pop the previous room from the backtracking stack |
| `history` | Show every move, including backtracking |
| `bfs` | Find the shortest currently unlocked route to the exit |
| `dfs` | Find a currently unlocked route using depth-first exploration |
| `status` | Show score, remaining time and hints |
| `help` | Show commands and scoring |
| `quit` | End the game and show the final route |

Commands are lowercase. Answers ignore whitespace and letter case. Invalid movement and empty answers do not cost points or attempts. Puzzle rewards cannot be collected twice. Keys are automatically collected upon solving their puzzle and are never consumed. Doors work in both directions and unlock when their requirements are met.

## Rules and outcomes

- Three attempts per puzzle; wrong answers deduct 10 points.
- Correct answers award 100 points.
- Three hints shared across all puzzles; each costs 15 points. Repeated hints use another hint and repeat the same clue.
- Each successful move or backtracking step costs 1 point. Scores can be negative.
- Escaping awards 200 points plus `remaining seconds / 5` using integer division.
- The game ends on escape, timeout, quitting/EOF, or exhausting an essential puzzle's attempts so that the exit can no longer be reached.
- The 600-second wall-clock timer starts when the game starts. Time spent typing counts. Standard blocking console input means timeout is reported after Enter is pressed or input closes; an answer submitted after expiry is rejected.
- BFS and DFS respect current locks. If no unlocked exit route exists, solve more puzzles. They do not plan hypothetical future puzzle solutions.

## Project files

- `include/Game.hpp`: room, puzzle, door and game declarations.
- `src/Game.cpp`: graph, puzzle engine, inventory, timer, scoring and algorithms.
- `src/main.cpp`: console command interface and final outcome.
- `tests/test_game.cpp`: deterministic regression checks with an injectable clock.
- `docs/DESIGN.md`: proposal mapping and algorithm explanation.
- `docs/WALKTHROUGH.md`: spoiler solution for demonstration and marking.
- `CMakeLists.txt`, `build.bat`: cross-platform and Windows build entry points.

No external packages, graphics framework or platform-specific terminal APIs are needed. Windows execution should be checked with your installed compiler; automated verification was performed with GCC on Linux.
