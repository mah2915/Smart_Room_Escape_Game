# Design and proposal coverage

## Requirements

| Proposal detail | Implementation |
|---|---|
| Text-based C++ game, standard library, Windows | Portable C++17 console interface; Windows batch build and CMake |
| Rooms as graph vertices, doors as edges | Eight rooms and nine bidirectional connections in an adjacency-list graph |
| Arrays for room details, answers, keys and scores | Fixed `std::array` collections of rooms and keys; puzzle records and integer score |
| Locked doors with meaningful requirements | Brass, Silver, Gear and Exit keys; Gallery puzzle controls Workshop door |
| Riddles, mathematics and number puzzles | Five puzzles across Library, Study, Gallery, Workshop and Observatory |
| Limited attempts | Three per puzzle; essential puzzle exhaustion produces an explicit loss |
| Key collection and inventory | Automatic key rewards, fixed boolean key array and inventory command |
| Limited hints and deductions | Three shared hints, 15-point penalty per use |
| Movement history and return | `std::stack<int>` for backtracking and a separate chronological vector for complete history |
| Time limit and scoring | Monotonic `steady_clock`, 600 seconds, rewards and penalties |
| BFS using a queue | `std::queue<int>` plus parent array reconstructs the shortest unlocked exit route |
| DFS route exploration | Recursive DFS with a visited array, returning one valid exit path |
| Clear win/loss and final path | Escape, timeout, unrecoverable puzzle failure or quit; status and travel route at the end |

## Building layout

| Connection | Requirement |
|---|---|
| Entrance (0) — Library (1) | Open |
| Entrance (0) — Gallery (3) | Open |
| Library (1) — Study (2) | Brass Key from Library |
| Study (2) — Gallery (3) | Brass Key from Library |
| Gallery (3) — Workshop (4) | Gallery puzzle solved |
| Workshop (4) — Observatory (5) | Silver Key from Study |
| Observatory (5) — Exit Hall (6) | Gear Key and Study puzzle solved |
| Gallery (3) — Exit Hall (6) | Gear Key from Workshop |
| Exit Hall (6) — Outside (7) | Exit Key from Observatory |

The cycles permit exploration in different orders. The Gear Key shortcut is useful after returning to the Gallery. Door locks belong to edges; both directions check the same requirement. Unlocks are permanent because keys are reusable and solved puzzles stay solved.

## State and algorithms

`Game` owns all state. The interface exposes actions but does not allow external code to mutate room records. A successful normal move pushes the old room onto the stack. `back` pops it without pushing a replacement; both actions append to the chronological trail. This preserves a complete final travel log while allowing repeated backtracking.

BFS visits each reachable room once and records its predecessor; reversing predecessors from Outside gives a minimum-edge route. DFS explores neighbors in insertion order and removes dead ends from its candidate path. Both run in O(V + E) time and O(V) auxiliary space, excluding the graph. The travel history uses O(M) space for M moves.

The loss detector computes a reachability fixed point using the current keys and every puzzle that still has an attempt. It repeatedly explores accessible doors and marks accessible, still-solvable puzzles as hypothetically solved. If Outside remains unreachable even under that optimistic assumption, the player is trapped. This avoids declaring a loss merely because BFS cannot yet pass a locked door. Actual BFS/DFS never use hypothetical keys.

A clock callback enables exact timer testing without sleeps. All game-changing actions check the outcome before changing state. Time is measured with a monotonic clock rather than system/calendar time. The interactive loop uses portable blocking `getline`, so it cannot redraw a countdown or announce timeout while the player is idle at the prompt. Expiry is checked before processing the next command.
