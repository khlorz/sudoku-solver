#include "Sudoku.h"
#include <chrono>

void print_board(SudokuBoard& board)
{
	printf(" ------------------------------------\n");
	for (size_t row = 0; row < 9; ++row) {
		printf("| ");
		for (size_t col = 0; col < 9; ++col) {
			printf(" %c |", board[row][col]);
		}
		printf("\n ------------------------------------\n");
	}
}

int main()
{
	//// NOTE: SudokuBoard == std::array<std::array<char, 9>, 9>
	//{   // Create a random generated sudoku board
	//	SudokuContext sudoku;
	//	// Initialize and create a board;
	//	sudoku.InitializeGame(SudokuDifficulty_Hard);
	//	// Get the solution board
	//	const SudokuBoard& solution_board = sudoku.GetSolutionBoard();
	//	// Get the puzzle board
	//	const SudokuBoard& puzzle_board = sudoku.GetPuzzleBoard();
	//	// Get the difficulty of the board
	//	const SudokuDifficulty& board_difficulty = sudoku.GetBoardDifficulty();
	//}
	//{   // Solve a pre-made sudoku board
	//	// Solve the sudoku and store the solution board and puzzle board for later
	//	SudokuBoard sudoku_board0 = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
	//	SudokuContext sudoku;
	//	sudoku.InitializeGame(sudoku_board0);

	//	// You just want to solve the sudoku board
	//	// These functions will return true if the operation is successful, otherwise false
	//	// In addition to that, it will also return the completed board, so make sure to make a copy of the board if needed!
	//	// For the recommended type: SudokuBoard or std::array<std::array<char, 9>, 9>
	//	SudokuBoard sudoku_board1 = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
	//	SudokuSolver::Solve(sudoku_board1) ? printf("YEY!\n") : printf("AWW!\n");

	//	// Helpers for std::array<char, 81>
	//	std::array<char, 81> sudoku_board2 = { { '0', '9', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '5', '7', '0', '4', '0', '0', '7', '0', '3', '0', '2', '0', '0', '9', '0', '4', '8', '0', '1', '5', '0', '0', '0', '3', '3', '0', '0', '8', '0', '0', '0', '0', '0', '9', '0', '2', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '6', '0', '8', '0', '0', '0', '6', '0', '0', '0', '3', '2', '1', '9' } };
	//	SudokuSolver::Solve2(sudoku_board2, nullptr, SolveMethod_MRV) ? printf("YEY!\n") : printf("AWW!\n");

	//	// Helpers for std::vector<char>
	//	std::vector<char> sudoku_board3 = { { '0', '9', '0', '0', '3', '0', '0', '0', '0', '0', '0', '0', '5', '7', '0', '4', '0', '0', '7', '0', '3', '0', '2', '0', '0', '9', '0', '4', '8', '0', '1', '5', '0', '0', '0', '3', '3', '0', '0', '8', '0', '0', '0', '0', '0', '9', '0', '2', '0', '0', '0', '0', '0', '8', '0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '0', '0', '0', '6', '0', '8', '0', '0', '0', '6', '0', '0', '0', '3', '2', '1', '9' } };
	//	SudokuSolver::Solve3(sudoku_board3) ? printf("YEY!\n") : printf("AWW!\n");

	//	// Helpers for std::array<std::array<int, 9>, 9>
	//	std::array<std::array<int, 9>, 9> sudoku_board4 = { { { 0, 9, 0, 0, 3, 0, 0, 0, 0 }, { 0, 0, 0, 5, 7, 0, 4, 0, 0 }, { 7, 0, 3, 0, 2, 0, 0, 9, 0 }, { 4, 8, 0, 1, 5, 0, 0, 0, 3 }, { 3, 0, 0, 8, 0, 0, 0, 0, 0 }, { 9, 0, 2, 0, 0, 0, 0, 0, 8 }, { 0, 0, 0, 0, 0, 0, 0, 4, 0 }, { 0, 0, 0, 0, 6, 0, 8, 0, 0 }, { 0, 6, 0, 0, 0, 3, 2, 1, 9 } } };
	//	SudokuSolver::Solve4(sudoku_board4, nullptr, SolveMethod_MRV) ? printf("YEY!\n") : printf("AWW!\n");

	//	// Helpers for std::vector<int>
	//	std::vector<int> sudoku_board5 = { { 0, 9, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 4, 0, 0, 7, 0, 3, 0, 2, 0, 0, 9, 0, 4, 8, 0, 1, 5, 0, 0, 0, 3, 3, 0, 0, 8, 0, 0, 0, 0, 0, 9, 0, 2, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 6, 0, 8, 0, 0, 0, 6, 0, 0, 0, 3, 2, 1, 9 } };
	//	SudokuSolver::Solve5(sudoku_board5) ? printf("YEY!\n") : printf("AWW!\n");

	//	// Helpers for std::array<int, 81>
	//	std::array<int, 81> sudoku_board6 = { { 0, 9, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 5, 7, 0, 4, 0, 0, 7, 0, 3, 0, 2, 0, 0, 9, 0, 4, 8, 0, 1, 5, 0, 0, 0, 3, 3, 0, 0, 8, 0, 0, 0, 0, 0, 9, 0, 2, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 6, 0, 8, 0, 0, 0, 6, 0, 0, 0, 3, 2, 1, 9 } };
	//	SudokuSolver::Solve6(sudoku_board6, nullptr, SolveMethod_MRV) ? printf("YEY!\n") : printf("AWW!\n");
	//}
	//{   // You can also use the main solving functions if you know what to do before using it
	//	SudokuBoard puzzle_board = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
	//	// Brute Force solving method
	//	SudokuBoard puzzle_board1 = puzzle_board;
	//	SudokuBoardLogic puzzle_logic1;
	//	// Create the board logic
	//	if (!SudokuSolver::CreateBoardLogic(puzzle_board, puzzle_logic1)) {
	//		printf("Failed!"); // The board is invalid. Has duplicates in some rows/columns/cells
	//	}
	//	// Checks if the sudoku board has a unique solution or not. Can be skipped if you're sure that it would only have one solution
	//	if (!SudokuSolver::IsUniqueBoard(puzzle_board1, &puzzle_logic1)) {
	//		printf("Has more than 1 solution!");
	//	}
	//	if (!SudokuSolver::SolveBruteForceEX(puzzle_board, puzzle_logic1, 0, 0)) {
	//		printf("Failed!");
	//	}

	//	// Brute Force + MRV solving method
	//	SudokuBoard puzzle_board2 = puzzle_board;
	//	SudokuBoardLogic puzzle_logic2;
	//	if (!SudokuSolver::CreateBoardLogic(puzzle_board2, puzzle_logic2)) {
	//		printf("Failed!"); // The board is invalid. Has duplicates in some rows/columns/cells
	//	}
	//	BoardMRV board_mrv(puzzle_board2, puzzle_logic2);
	//	//board_mrv.CreateMRV(puzzle_board2, puzzle_logic2); // Use this if you made a default constructed BoardMRV
	//	if (!SudokuSolver::SolveMRVEX(puzzle_board2, board_mrv)) {
	//		printf("Failed!");
	//	}
	//}

	SudokuBoard puzzle_board = { { { '0', '9', '0', '0', '3', '0', '0', '0', '0' }, { '0', '0', '0', '5', '7', '0', '4', '0', '0' }, { '7', '0', '3', '0', '2', '0', '0', '9', '0' }, { '4', '8', '0', '1', '5', '0', '0', '0', '3' }, { '3', '0', '0', '8', '0', '0', '0', '0', '0' }, { '9', '0', '2', '0', '0', '0', '0', '0', '8' }, { '0', '0', '0', '0', '0', '0', '0', '4', '0' }, { '0', '0', '0', '0', '6', '0', '8', '0', '0' }, { '0', '6', '0', '0', '0', '3', '2', '1', '9' } } };
	print_board(puzzle_board);
	SudokuBoardLogic puzzle_logic;
	if (!SudokuSolver::CreateBoardLogic(puzzle_board, puzzle_logic)) {
		return -1;
	}
	BoardMRV board_mrv(puzzle_board, puzzle_logic);
	SudokuSolver::SolveHumanely(puzzle_board, puzzle_logic, board_mrv);
	print_board(puzzle_board);

	std::cin.get();

	return 0;
}
