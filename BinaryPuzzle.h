#include <memory>
#include <optional>
#include <string>
#include <vector>

// Data structure representing a binary puzzle (binary sudoku)
// nullopt at a given index means it is blank, values of true or false mean 1 or 0
class BinaryPuzzle
{
public:
  // Construct a size x size matrix for the puzzle
  BinaryPuzzle(size_t size);

  BinaryPuzzle(BinaryPuzzle const &other);

  // Attempt to convert "xx1x00xx" into a row 
  bool interpret_row(std::string data, size_t row);

  // Get the specified row or column (`vertical`-dependent) and return it in a vector
  std::vector<std::optional<bool>> get_line(size_t index, bool vertical) const;

  void set_line(size_t index, bool vertical, std::vector<std::optional<bool>> const &line);

  void print() const;
  size_t size() const;

private:
  // Initializes (and resets) matrix to size s*s
  void create_matrix();

  std::unique_ptr<std::optional<bool>[]> matrix;
  size_t s;
};