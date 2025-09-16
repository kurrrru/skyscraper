# Skyscraper Puzzle Solver

## Project Overview

This project provides a C++ solver for the Skyscraper puzzle. The solver is designed to be efficient and can handle puzzles of various sizes. It employs a combination of constraint satisfaction and backtracking algorithms to find the solution. Additionally, the project includes a Python script to generate new Skyscraper puzzles, which can be used to test the solver.

## Key Features

  - **Skyscraper Puzzle Solver**: A robust solver for N x N Skyscraper puzzles.
  - **Efficient Algorithm**: Utilizes a powerful combination of pre-computation, constraint propagation, and backtracking with the Minimum Remaining Values (MRV) heuristic to find solutions quickly.
  - **Puzzle Generator**: Comes with a Python utility to generate random Skyscraper puzzles of a specified size, complete with the solution and the input string for the solver.

## Detailed Algorithm Description

The solver is based on a constraint satisfaction approach, which is significantly enhanced by a backtracking search algorithm. The core idea is to reduce the problem space by eliminating possibilities that violate the puzzle's rules before resorting to a brute-force search.

Here is a step-by-step breakdown of the algorithm:

### 1\. Pre-computation of Permutations

Before the search begins, the program pre-computes all possible permutations of skyscraper heights (1 to N) for a single row or column. For each permutation, it calculates the number of visible skyscrapers from the left and right sides. These permutations are then stored and grouped by their (left view, right view) clue pair. This initial step is a crucial optimization, as it allows the solver to instantly retrieve all valid line patterns for a given pair of clues.

### 2\. Initialization of Possibilities

Using the pre-computed data, the solver initializes a list of valid permutations for each row and each column based on the puzzle's input clues. For example, if the clues for the first row are (left=2, right=3), the solver assigns it the list of all permutations that have exactly 2 visible skyscrapers from the left and 3 from the right.

### 3\. Constraint Propagation (Pruning)

This is the core of the constraint satisfaction logic. The goal is to achieve consistency between the possibilities for rows and columns. The solver enters a loop where it iteratively prunes the lists of permutations:

  - **Row-to-Column Pruning**: For each cell `(r, c)`, the solver calculates the set of all possible values it can take based on the current list of permutations for its row `r`. It then uses this set to eliminate any permutations from the list for column `c` that do not contain one of these possible values at position `r`.
  - **Column-to-Row Pruning**: Similarly, it calculates the possible values for cell `(r, c)` based on the permutations for column `c` and uses them to prune the list of permutations for row `r`.

This process repeats until a full pass occurs where no permutations are eliminated, meaning the possibilities are in a consistent state. This step alone can solve simpler puzzles or significantly reduce the search space for more complex ones.

### 4\. Backtracking Search (Depth-First Search)

If the puzzle is not fully solved after the pruning phase (i.e., some rows/columns still have more than one possible permutation), the solver transitions to a recursive backtracking algorithm.

  - **Cell Selection**: It selects an unsolved cell to make a guess. To make the search more efficient, it uses the **Minimum Remaining Values (MRV)** heuristic, choosing the cell with the smallest number of possible values.
  - **Guessing**: It iterates through each possible value for the selected cell. For each guess, it temporarily updates the permutation lists for the corresponding row and column, keeping only those permutations that match the guessed value at that position.
  - **Recursion**: It then recursively calls the solve function with the updated state.
  - **Backtracking**: If a guess leads to a contradiction (e.g., a row or column ends up with no valid permutations), the function returns, and the solver undoes the guess (restores the original permutation lists) and tries the next possible value for the cell.

This recursive process continues until a valid solution is found where every row and column has exactly one permutation.

## Setup and Execution

Follow these steps to compile and run the Skyscraper puzzle solver on your local machine.

### Prerequisites

  - A C++ compiler that supports C++11 (e.g., `g++`)
  - `make`
  - `python3` (for the puzzle generator)

### Compilation

Open your terminal, navigate to the project's root directory, and run the `make` command. This will compile the source code and create an executable file named `solver`.

```bash
make
```

### Puzzle Generation

You can generate a new puzzle using the included Python script. The `Makefile` provides a convenient shortcut. To generate a 4x4 puzzle, for instance, run:

```bash
make gen-4
```

This will output the solution grid (for verification) and the corresponding clue string for the solver.

Example output for `make gen-4`:

```
# Skyscraper Puzzle (N=4)
# Solution board (for verification):
# 4 1 2 3
# 1 2 3 4
# 2 3 4 1
# 3 4 1 2

# Solver input string:
"1 4 3 2 4 1 2 3 1 3 2 4 2 1 4 3"
```

### Running the Solver

To run the solver, execute the compiled program and pass the clue string as a command-line argument. The clues must be enclosed in quotes and ordered as: top, bottom, left, right.

Using the example string from above:

```bash
./solver "1 4 3 2 4 1 2 3 1 3 2 4 2 1 4 3"
```

The solver will then process the input and print the solution to the console.

Example output:

```
Solving for N=4

Solution Found:
4 1 2 3
1 2 3 4
2 3 4 1
3 4 1 2
```

## Performance

While formal benchmarking has not been conducted, the solver demonstrates strong performance for moderately sized puzzles.

  - A 10x10 puzzle is typically solved within a few seconds.
  - A 11x11 puzzle is solved in approximately 2 minutes.
  - Performance for puzzles larger than 11x11 has not been tested.

The complexity of the puzzle, determined by the specific clue values, can also influence the solving time. Puzzles that require more extensive backtracking will naturally take longer to solve.