#include "BinaryPuzzle.h"

#include <iostream>

BinaryPuzzle::BinaryPuzzle(size_t size)
  : s(size)
{
  create_matrix();
}

void BinaryPuzzle::create_matrix()
{
  if (matrix)
  {
    matrix.reset();
  }

  matrix = std::make_unique<std::optional<bool>[]>(s * s);
}

BinaryPuzzle::BinaryPuzzle(BinaryPuzzle const &other)
{
  s = other.s;

  // Make the new matrix
  create_matrix();

  // Copy the other values
  for (int i = 0; i < s*s; i++)
  {
    matrix[i] = other.matrix[i];
  }
}

bool BinaryPuzzle::interpret_row(std::string data, size_t row)
{
  if (data.length() != size())
  {
    return false;
  }
  
  for (int i = 0; i < size(); i++)
  {
    auto character = data.at(i);
    auto &val = matrix[size() * row + i];

    if (character == '0' || character == '1')
    {
      val = (character - '0') != 0;
    }
    else if (character == 'x')
    {
      // row[i] is already nullopt
    }
    else
    {
      return false; // Invalid character
    }
  }

  return true;
}

std::vector<std::optional<bool>> BinaryPuzzle::get_line(size_t line, bool vertical) const
{
  std::vector<std::optional<bool>> result;

  int index = vertical ? line : (line * size());
  for (int i = 0; i < size(); i++)
  {
    result.push_back(matrix[index]);

    if (vertical)
    {
      index += size();
    }
    else
    {
      index++;
    }
  }

  return result;
}

void BinaryPuzzle::set_line(size_t linenum, bool vertical, std::vector<std::optional<bool>> const &line)
{
  int index = vertical ? linenum : (linenum * size());
  for (int i = 0; i < size(); i++)
  {
    matrix[index] = line[i];

    if (vertical)
    {
      index += size();
    }
    else
    {
      index++;
    }
  }
}

void BinaryPuzzle::print() const
{
  for (int j = 0; j < size(); j++)
  {
    for (int i = 0; i < size(); i++)
    {
      auto const &item = matrix[j*size() + i];
      if (!item)
      {
        std::cout << "x";
      }
      else
      {
        std::cout << *item ? "1" : "0";
      }
    }
    std::cout << std::endl;
  }
}

size_t BinaryPuzzle::size() const
{
  return s;
}