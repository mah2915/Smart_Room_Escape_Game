# Demonstration walkthrough

**Spoilers:** These commands complete the game when entered within ten minutes.

```text
go 1
solve piano
go 2
solve 5
go 3
solve 30
go 4
solve 60
go 5
solve echo
inventory
bfs
dfs
go 6
go 7
```

Before moving to Exit Hall, BFS reports `Observatory (5) -> Exit Hall (6) -> Outside (7)`. DFS can report a longer route because it explores neighbors in graph insertion order.

The final trail is Entrance → Library → Study → Gallery → Workshop → Observatory → Exit Hall → Outside. Five solved puzzles yield 500 points, seven moves cost 7, and the escape reward is 200 plus the time bonus: total `693 + remaining seconds / 5`.

To demonstrate other features, start a new game. Try `go 7` from Entrance to demonstrate nonadjacent movement rejection. Move to Library, try `go 2` before solving the puzzle, then use `hint` and submit a wrong answer to show penalties. Use `back` to return to Entrance and `history` to show both movements. In another game, submit three wrong Library answers to show an unrecoverable loss. The regression executable verifies the timeout boundary without waiting ten minutes.
