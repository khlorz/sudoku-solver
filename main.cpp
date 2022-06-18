#include "SudokuContext.h"

void print_board(std::array<std::array<char, 9>, 9>& board)
{
	printf(" -----------------------------------\n");
	for (size_t row = 0; row < 9; ++row) {
		printf("|");
		for (size_t col = 0; col < 9; ++col) {
			printf(" %c |", board[row][col]);
		}
		printf("\n -----------------------------------\n");
	}
}

void print_board(SudokuBoard& board)
{
	printf(" -----------------------------------\n");
	for (size_t row = 0; row < 9; ++row) {
		printf("|");
		for (size_t col = 0; col < 9; ++col) {
			printf(" %c |", board.BoardTiles[row][col].TileNumber);
		}
		printf("\n -----------------------------------\n");
	}
}

int main()
{
	{ // Create a sudoku context that creates a randomly generated sudoku board
		SudokuContext sudoku;
		if (!sudoku.InitializeSudoku(SudokuDifficulty_Random)) {
			return 0; // Early exit in case there was some error or was unable to make a sudoku board
		}
		// Get the solution board
		const SudokuBoard* solution_board = sudoku.GetSolutionBoard();
		// Get the puzzle board
		SudokuBoard* puzzle_board = sudoku.GetPuzzleBoard();
		// You can also create a copy instead if you need to
		//SudokuBoard puzzle_board_copy = *sudoku.GetPuzzleBoard();		
	}

	{ // Create a sudoku context from a premade sudoku board (an array or vector)
		std::array<std::array<char, 9>, 9> pre_made_sudoku = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
		SudokuContext sudoku;
		if (!sudoku.InitializeSudoku(pre_made_sudoku)) {
			return 0;
		}
		const SudokuBoard* solution_board = sudoku.GetSolutionBoard();
		SudokuBoard* puzzle_board = sudoku.GetPuzzleBoard();
	}

	{ // Create a SudokuBoard object if you don't need the sudoku context shenanigans!
		// For now, creating a SudokuBoard object only accepts an array of an array of char
		SudokuBoard sudoku_board;
		std::array<std::array<char, 9>, 9> sudoku_board_array = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
		// Returns true if the board is valid, otherwise it returns false
		if (!sudoku_board.CreateSudokuBoard(sudoku_board_array)) {
			return 0;
		}
	}

	{   // Solving a sudoku puzzle
		// Sudoku solvers are inside the namespace 'SudokuSolvers'
		// You can specify the method of solving the puzzle from SolveMethod_ enum
		
		// If you want to solve the randomly generated sudoku board
		SudokuContext sudoku1;
		if (sudoku1.InitializeSudoku(SudokuDifficulty_Random)) {
			SudokuSolver::Solve(*sudoku1.GetPuzzleBoard(), SolveMethod_MRV) ? printf("The puzzle has been solved.\n") : printf("The puzzle was not solved.\n");
			//print_board(*sudoku1.GetPuzzleBoard());
		}

		// If you want to solve the pre-made sudoku board
		SudokuContext sudoku2;
		std::array<std::array<char, 9>, 9> sudoku_board_array1 = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
		if (sudoku2.InitializeSudoku(sudoku_board_array1)) {
			SudokuSolver::Solve(*sudoku2.GetPuzzleBoard(), SolveMethod_BruteForce) ? printf("The puzzle has been solved.\n") : printf("The puzzle was not solved.\n");
			//print_board(*sudoku2.GetPuzzleBoard());
		}

		SudokuBoard sudoku_board0;
		std::array<std::array<char, 9>, 9> sudoku_board_array2 = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
		// Returns true if the board is valid, otherwise it returns false
		if (sudoku_board0.CreateSudokuBoard(sudoku_board_array2)) {
			SudokuSolver::Solve(sudoku_board0, SolveMethod_BruteForce) ? printf("The puzzle has been solved.\n") : printf("The puzzle was not solved.\n");
		}

		// If you just want to directly solve the pre-made sudoku board (an array or vector)
		// Helpers for array of an array of char
		std::array<std::array<char, 9>, 9> sudoku_board1 = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
		SudokuSolver::Solve2(sudoku_board1, SolveMethod_BruteForce) ? printf("The puzzle has been solved.\n") : printf("The puzzle was not solved.\n");

		// Helpers for array of chars
		std::array<char, 81> sudoku_board2 = { { '0', '9', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '5', '7', '0', '4', '0', '0', '7', '0', '3', '0', '2', '0', '0', '9', '0', '4', '8', '0', '1', '5', '0', '0', '0', '3', '3', '0', '0', '8', '0', '0', '0', '0', '0', '9', '0', '2', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '6', '0', '8', '0', '0', '0', '6', '0', '0', '0', '3', '2', '1', '9' } };
		SudokuSolver::Solve3(sudoku_board2, SolveMethod_MRV) ? printf("The puzzle has been solved.\n") : printf("The puzzle was not solved.\n");
	}

	{   // Some utilities involving sudoku from the namespace 'SudokuUtilities'
		SudokuContext sudoku;
		sudoku.InitializeSudoku(SudokuDifficulty_Random);
		// Checks if the sudoku board has a unique solution (kind of slow for now)
		SudokuUtilities::IsUniqueBoard(*sudoku.GetPuzzleBoard()) ? printf("Solution is unique!\n") : printf("The board has multiple solutions...\n");
		// Fills the sudoku board with random numbers
		// See SudokuContext::CreateCompleteBoard() function definition for the SudokuUtilities::FillSudoku function application
		// You can check the puzzle difficulty
		printf("Sudoku Difficulty Score is %zu\n", SudokuUtilities::CheckPuzzleDifficulty(*sudoku.GetPuzzleBoard()));
	}

	std::cin.get();

	return 0;
}

/*
0 ? 2     = 0
1 ? 2     = 3
2 ? 2     = 6

*/





