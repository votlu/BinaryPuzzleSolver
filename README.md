# BinaryPuzzleSolver
Automatically solves binary puzzles (AKA Takuzu, Binairo).

Enter each puzzle row-by-row as 1x0xx01x where x is unknown, and 1 or 0 is a known value.  
The solver will start by applying the basic rules of the game to each row and column in order to simplify the puzzle.  
If it can't proceed, it will make an assumption about the first unknown and try solving again, if the assumption turns out to be invalid, it will try the other value.

Example: Enter below for the puzzle (you can C+P)  
x1x0  
xx0x  
x0xx  
11x0  

It will output  
0110  
1001  
0011  
1100
