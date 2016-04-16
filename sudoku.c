//
// From: https://see.stanford.edu/materials/icspacs106b/H19-RecBacktrackExamples.pdf
//
// A straightforward port from Forth to C!
//
// http://www.telegraph.co.uk/news/science/science-news/9359579/Worlds-hardest-sudoku-can-you-crack-it.html


#include <stdio.h>
#include <stdbool.h>
#include <time.h>


// Some useful constants

#define UNASSIGNED 0
#define BOX_SIZE 3
#define BOARD_SIZE 9
#define CELL_COUNT (BOARD_SIZE * BOARD_SIZE)


int puzzle[CELL_COUNT] = {
	8, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 3, 6, 0, 0, 0, 0, 0,
	0, 7, 0, 0, 9, 0, 2, 0, 0,
	0, 5, 0, 0, 0, 7, 0, 0, 0,
	0, 0, 0, 0, 4, 5, 7, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 3, 0,
	0, 0, 1, 0, 0, 0, 0, 6, 8,
	0, 0, 8, 5, 0, 0, 0, 1, 0,
	0, 9, 0, 0, 0, 0, 4, 0, 0,
};


// Rather than compare against zero, make the code more meaningful to readers.

bool is_assigned(int number) {
	return number != UNASSIGNED;
}

bool is_unassigned(int number) {
	return number == UNASSIGNED;
}


// Puzzle access methods

int grid_peek(int n) {
	return puzzle[n];
}

void grid_poke(int number, int n) {
	puzzle[n] = number;
}


// Move and grid position translation methods

struct coord
{
	int x, y;
};

int move2x(int n) {
	return n % BOARD_SIZE;
}

int move2y(int n) {
	return n / BOARD_SIZE;
}

struct coord move2xy(int n) {
	struct coord xy;
	xy.x = move2x(n);
	xy.y = move2y(n);
	return xy;
}

int xy2move(int x, int y) {
	return y * BOARD_SIZE + x;
}


// Row, column and box start positions

int move2row_start(int n) {
	return move2y(n) * BOARD_SIZE;
}

int move2column_start(int n) {
	return move2x(n);
}

int box_side_start(int n) {
	return n - (n % BOX_SIZE);
}

int move2box_start(int n) {
	struct coord xy = move2xy(n);
	int xbox = box_side_start(xy.x);
	int ybox = box_side_start(xy.y);
	return xy2move(xbox, ybox);
}


// Function: is_used_in_row
// ------------------------
// Returns a boolean which indicates whether any assigned entry
// in the specified row matches the given number.

bool is_used_in_row(int number, int n) {
	int row_start = move2row_start(n);
	int offset = row_start;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		if (grid_peek(offset++) == number) {
			return true;
		}
	}
	return false;
}


// Function: is_used_in_column
// ---------------------------
// Returns a boolean which indicates whether any assigned entry
// in the specified column matches the given number.

bool is_used_in_column(int number, int n) {
	int column_start = move2column_start(n);
	int offset = column_start;
	for (int i = 0; i < BOARD_SIZE; ++i) {
		if (grid_peek(offset) == number) {
			return true;
		}
		offset += BOARD_SIZE;
	}
	return false;
}


// Function: is_used_in_box
// ------------------------
// Returns a boolean which indicates whether any assigned entry
// within the specified 3x3 box matches the given number.

bool is_used_in_box(int number, int n) {
	int box_start = move2box_start(n);
	for (int i = 0; i < BOARD_SIZE; ++i) {
		int x_box = i % BOX_SIZE;
		int y_box = i / BOX_SIZE;
		int offset = xy2move(x_box, y_box) + box_start;
		if (grid_peek(offset) == number) {
			return true;
		}
	}
	return false;
}


// Function: is_available
// ----------------------
// Returns a boolean which indicates whether it will be legal to assign
// number to the given row, column location.As assignment is legal if it that
// number is not already used in the row, column, or box.

bool is_available(int number, int n) {
	return !is_used_in_row(number, n)
		&& !is_used_in_column(number, n)
		&& !is_used_in_box(number, n);
}



// Function: solve
// ---------------
// Takes a partially filled - in grid and attempts to assign values to all
// unassigned locations in such a way to meet the requirements for sudoku
// solution(non - duplication across rows, columns, and boxes).The function
// operates via recursive backtracking : it finds an unassigned location with
// the grid and then considers all digits from 1 to "board-size" in a loop.If a digit
// is found that has no existing conflicts, tentatively assign it and recur
// to attempt to fill in rest of grid.If this was successful, the puzzle is
// solved.If not, unmake that decision and try again.If all digits have
// been examined and none worked out, return false to backtrack to previous
// decision point.

bool solve(int n) {

	if (n == CELL_COUNT)
		return true;							// success!

	if (is_assigned(grid_peek(n)))
		return solve(n + 1);					//  if it's already assigned, skip

	for (int i = 1; i <= BOARD_SIZE; ++i) {		// consider all digits
		if (is_available(i, n)) {
			grid_poke(i, n);					// make tentative assignment
			if (solve(n + 1)) {
				return true;					// recur, if success, yay!
			}
		}
	}

	grid_poke(UNASSIGNED, n);					// failure, unmake & try again

	return false;								// this triggers backtracking
}


// Board display

void print_board_element(int n) {
	int number = grid_peek(n);
	if (is_assigned(number)) {
		printf(" %d ", number);
	} else {
		printf(" - ");
	}
}

void print_box_break_vertical() {
	printf("|");
}

void print_box_break_horizontal() {
	printf(" --------+---------+--------");
}

void print_board() {
	for (int i = 0; i < CELL_COUNT; ++i) {
		int x = move2x(i);
		if (x == 0) {
			int y = move2y(i);
			if (y % BOX_SIZE == 0) {
				printf("\n\n");
				print_box_break_horizontal();
			}
			printf("\n\n");
		} else {
			if (x % BOX_SIZE == 0) {
				print_box_break_vertical();
			}
		}
		print_board_element(i);
	}
	printf("\n\n");
	print_box_break_horizontal();
	printf("\n");
}


void game() {

	clock_t start = clock();
	bool solved = solve(0);
	clock_t finish = clock();

	if (solved) {

		print_board();

		clock_t elapsed = finish - start;
		double seconds = (elapsed % CLOCKS_PER_SEC) / (double)CLOCKS_PER_SEC + (elapsed / CLOCKS_PER_SEC);

		printf("\nTime taken %f\n", seconds);
	} else {
		printf("No solution exists\n");
	}
}


int main() {
	game();
}
