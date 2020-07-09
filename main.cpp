#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include "BinaryPuzzle.h"

// Apply 1x1, 11x, x11, etc. rules to the whole matrix until it doesn't reduce further
void reducePuzzle(BinaryPuzzle &puzzle, size_t size, int &rnum_unknowns);

// Apply 1x1, 11x, x11, etc. rules to the line
void solveLine(std::vector<std::optional<bool>> &line, int &rnum_unknowns);

bool IsValidPuzzle(BinaryPuzzle &puzzle);
bool IsValidLine(std::vector<std::optional<bool>> const& line);

struct Assumption
{
  int x, y;
  bool value;
};

int main()
{
  std::string line;

  std::cout << "Input style: 0xxxx1xxx0x1" << std::endl;
  std::cout << "1 or 0 for known value, x for unknown" << std::endl;

  std::cout << "Input row 1 of puzzle:" << std::endl;
  std::cin >> line;

  auto size = line.length();

  if (size % 2 != 0)
  {
    std::cout << "Size must be even" << std::endl;
    return 1;
  }

  std::cout << "Ok, puzzle size should be " << size << "x" << size << std::endl;

  if (size <= 0)
  {
    std::cout << "Nothing to solve" << std::endl;
    return 1;
  }

  std::vector<BinaryPuzzle> puzzle_stack;

  // Initial puzzle input
  {
    BinaryPuzzle puzzle{size};

    std::cout << "Input remaining rows:" << std::endl;

    if (!puzzle.interpret_row(line, 0))
    {
      std::cout << "Failed to load row 0" << std::endl;
    }

    for (int i = 1; i < size; i++)
    {
      std::cin >> line;
      if (!puzzle.interpret_row(line, i))
      {
        std::cout << "Failed to load row" << std::endl;
        return 1;
      }
    }

    puzzle_stack.push_back(puzzle);
  }

  std::cout << "----------" << std::endl;

  int puzzle_stack_index{};
  int num_unknowns{-1};
  std::vector<Assumption> assumptions;

  while (num_unknowns != 0)
  {
    reducePuzzle(puzzle_stack.back(), size, num_unknowns);

    if (!IsValidPuzzle(puzzle_stack.back()))
    {
      if (puzzle_stack_index > 0)
      {
        if (assumptions.back().value)
        {
          puzzle_stack_index--;
          puzzle_stack.pop_back();
          assumptions.pop_back();
        }
        else
        {
          auto &assumption = assumptions.back();
          // Pop the current puzzle and push a copy of the previous
          puzzle_stack.pop_back();
          puzzle_stack.push_back(puzzle_stack[puzzle_stack_index - 1]);

          assumption.value = true;
          auto line = puzzle_stack.back().get_line(assumption.y, false);
          line[assumption.x] = assumption.value;
          puzzle_stack.back().set_line(assumption.y, false, line);
        }

        assumptions.pop_back();
      }
      else
      {
        std::cout << "Puzzle cannot be solved" << std::endl;
        return 1;
      }
    }
    else if (num_unknowns > 0)
    {
      std::cout << "Need to make assumption" << std::endl;
      bool made_assumption{};

      for (int j = 0; j < size && !made_assumption; j++)
      {
        auto line = puzzle_stack.back().get_line(j, false);

        for (int i = 0; i < size; i++)
        {
          if (!line[i].has_value())
          {
            // Push a copy of the current puzzle for the assumption
            puzzle_stack.push_back(puzzle_stack.back());
            assumptions.push_back({i, j, false});
            puzzle_stack_index++;

            line[i] = false;
            puzzle_stack.back().set_line(j, false, line);

            std::cout << "Assumed (" << i << ", " << j << ") is 0" << std::endl;

            made_assumption = true;
            break;
          }
        }
      }

      if (!made_assumption)
      {
        std::cout << "Failed to make assumption" << std::endl;
        return 1;
      }
    }

    std::cout << "Puzzle at index " << puzzle_stack_index << ":" << std::endl;
    puzzle_stack.back().print();
  }

  std::cout << "Solved" << std::endl;

  puzzle_stack.back().print();

  return 0;
}

bool IsValidPuzzle(BinaryPuzzle &puzzle)
{
  for (int i = 0; i < puzzle.size(); i++)
  {
    auto row = puzzle.get_line(i, false);
    auto col = puzzle.get_line(i, true);

    if (!IsValidLine(row) || !IsValidLine(col))
    {
      return false;
    }
  }

  return true;
}

void reducePuzzle(BinaryPuzzle &puzzle, size_t size, int &rnum_unknowns)
{
  int prev_num_unknowns{};
  int num_unknowns{};

  do
  {
    prev_num_unknowns = num_unknowns;
    num_unknowns = 0;

    // Solve the matrix's rows and columns
    for (int i = 0; i < size; i++)
    {
      int row_unknowns{};
      auto row = puzzle.get_line(i, false);
      solveLine(row, row_unknowns);
      puzzle.set_line(i, false, row);

      int col_unknowns{};
      auto column = puzzle.get_line(i, true);
      solveLine(column, col_unknowns);
      puzzle.set_line(i, true, column);

      // Tally up the unknowns
      num_unknowns += col_unknowns + row_unknowns;
    }
  // We repeat until we run out of unknowns, or our iteration didn't have an effect
  } while (num_unknowns > 0 && num_unknowns != prev_num_unknowns);

  rnum_unknowns = num_unknowns;
}

void solveLine(std::vector<std::optional<bool>> &line, int &rnum_unknowns)
{
  auto size = line.size();

  // Count the numbers of each so we can possibly deduce the rest when we're done
  int num_unknowns{};
  int num_1s{};
  int num_0s{};

  for (int i = 0; i < size; i++)
  {
    if (line[i])
    {
      if (!*line[i])
      {
        num_0s++;
      }
      else
      {
        num_1s++;
      }
    }
    else
    {
      // 11x / 00x => 110 / 001
      // Check the last two values, if they're set and equal, our value is the opposite
      if (i > 1 && line[i - 1] && line[i - 2] && *line[i - 1] == *line[i - 2])
      {
        line[i] = !*line[i - 1];
      }
      // x11 / x00 => 011 / 100
      // Check the next two values
      else if (i < size - 2 && line[i + 1] && line[i + 2] && *line[i + 1] == *line[i + 2])
      {
        line[i] = !*line[i + 1];
      }
      // 1x1 / 0x0 => 101 / 010
      // Check the next and previous values
      else if (i > 0 && i < size - 1 && line[i - 1] && line[i + 1] && *line[i - 1] == *line[i + 1])
      {
        line[i] = !*line[i - 1];
      }

      // If we still haven't solved for it, it's unknown
      if (!line[i])
      {
        num_unknowns++;
      }
      else
      {
        *line[i] ? num_1s++ : num_0s++;
      }
    }
  }

  if (num_unknowns > 0 && (num_0s >= size / 2 || num_1s >= size / 2))
  {
    for (int i = 0; i < size; i++)
    {
      if (!line[i])
      {
        line[i] = (num_0s >= size / 2);
        num_unknowns--;
      }
    }
  }

  rnum_unknowns = num_unknowns;
}

bool IsValidLine(std::vector<std::optional<bool>> const& line)
{
  int num_1s{}, num_0s{}, num_unknowns{};

  for (int i = 0; i < line.size(); i++)
  {
    if (!line[i])
    {
      num_unknowns++;

      // No need to check for surrounding values
    }
    else if (line[i].value())
    {
      num_1s++;

      // Check for surrounding 1s
      int surrounding_1s{};

      if (i - 1 >= 0 && line[i - 1] && line[i - 1].value())
      {
        surrounding_1s++;

        if (i - 2 >= 0 && line[i - 2] && line[i - 2].value())
        {
          surrounding_1s++;
        }
      }

      if (i + 1 < line.size() && line[i + 1] && line[i + 1].value())
      {
        surrounding_1s++;

        if (i + 2 < line.size() && line[i + 2] && line[i + 2].value())
        {
          surrounding_1s++;
        }
      }

      if (surrounding_1s > 1) return false;
    }
    else
    {
      num_0s++;

      // Check for surrounding 0s
      int surrounding_0s{};

      if (i - 1 >= 0 && line[i - 1] && !line[i - 1].value())
      {
        surrounding_0s++;

        if (i - 2 >= 0 && line[i - 2] && !line[i - 2].value())
        {
          surrounding_0s++;
        }
      }

      if (i + 1 < line.size() && line[i + 1] && !line[i + 1].value())
      {
        surrounding_0s++;

        if (i + 2 < line.size() && line[i + 2] && !line[i + 2].value())
        {
          surrounding_0s++;
        }
      }

      if (surrounding_0s > 1) return false;
    }
  }

  if (num_1s > line.size() / 2) return false;
  if (num_0s > line.size() / 2) return false;

  return true;
}