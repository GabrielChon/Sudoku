This is a sudoku solver in C.

It takes a 9x9 file (.txt) containing numbers from 0-9 (0 works like a space) and checks if it is the right format.

If the file is right, it checks if there is no error on the file (numbers in same row, column or square must be unique)

Then, it tries to solve the puzzle.


USAGE:

gcc sudoku.c
./sudoku.c file.txt
