import sys
import random
import argparse
from typing import List

def count_visible(line: List[int]) -> int:
    """Counts the number of visible buildings in a single line."""
    max_height = 0
    seen_count = 0
    for height in line:
        if height > max_height:
            max_height = height
            seen_count += 1
    return seen_count

def generate_solution_grid(n: int) -> List[List[int]]:
    """
    Generates a random N x N Latin square to serve as the puzzle's solution.
    """
    # 1. Create a base Latin square.
    base_row = list(range(1, n + 1))
    grid = [base_row[i:] + base_row[:i] for i in range(n)]

    # 2. Shuffle the grid to introduce randomness.
    # The number of shuffle operations scales with the grid size.
    for _ in range(n * 5):
        # Swap two random rows
        r1, r2 = random.sample(range(n), 2)
        grid[r1], grid[r2] = grid[r2], grid[r1]

        # Swap two random columns
        c1, c2 = random.sample(range(n), 2)
        for i in range(n):
            grid[i][c1], grid[i][c2] = grid[i][c2], grid[i][c1]
            
        # Relabel two random values throughout the grid
        v1, v2 = random.sample(range(1, n + 1), 2)
        for r in range(n):
            for c in range(n):
                if grid[r][c] == v1:
                    grid[r][c] = v2
                elif grid[r][c] == v2:
                    grid[r][c] = v1
                    
    return grid

def calculate_clues(grid: List[List[int]]) -> List[int]:
    """
    Calculates the peripheral clues from a given solution grid.
    """
    n = len(grid)
    clues_top = [0] * n
    clues_bottom = [0] * n
    clues_left = [0] * n
    clues_right = [0] * n

    for i in range(n):
        row = grid[i]
        clues_left[i] = count_visible(row)
        clues_right[i] = count_visible(row[::-1])

    for j in range(n):
        col = [grid[i][j] for i in range(n)]
        clues_top[j] = count_visible(col)
        clues_bottom[j] = count_visible(col[::-1])
        
    # Concatenate clues in the required order for the C++ solver.
    return clues_top + clues_bottom + clues_left + clues_right


def main():
    """
    Main execution block.
    """
    parser = argparse.ArgumentParser(
        description="Generate a random Skyscraper puzzle input.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        "size",
        type=int,
        nargs='?',
        default=4,
        help="The size of the grid (e.g., 4 for a 4x4 grid). Default is 4."
    )

    args = parser.parse_args()
    n = args.size

    if not 3 <= n <= 15:
        print("Error: Size must be between 3 and 15.", file=sys.stderr)
        return

    solution_grid = generate_solution_grid(n)
    clues = calculate_clues(solution_grid)

    print(f"# Skyscraper Puzzle (N={n})")
    print(f"# Solution board (for verification):")
    for row in solution_grid:
        print(f"# {' '.join(map(str, row))}")
    print(f"\n# Solver input string:")
    
    clues_str = ' '.join(map(str, clues))
    print(f'"{clues_str}"')


if __name__ == "__main__":
    main()