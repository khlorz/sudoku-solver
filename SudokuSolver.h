#pragma once

#include <vector>
#include <array>
#include <bitset>
#include <iostream>
#include <algorithm>

using SolveMethod      = int;
using SudokuDifficulty = int;
using SudokuBoard      = std::array<std::array<char, 9>, 9>;

enum SudokuDifficulty_
{
    SudokuDifficulty_Random    = 0,
    SudokuDifficulty_Easy      = 1,
    SudokuDifficulty_Normal    = 2,
    SudokuDifficulty_Hard      = 3,
    SudokuDifficulty_ChadBrain = 4
    //SudokuDifficulty_Custom    = 5     // Still work on progress
};

enum SolveMethod_
{
    SolveMethod_BruteForce = 1,    // Typical sudoku solver method with simple backtracking method
    SolveMethod_MRV        = 2     // Sudoku solver method that uses minimum remaining values for faster solving
};

// This class is used for finding MRVs and for faster solving of the sudoku
class SudokuBoardLogic
{
private:
    std::array<std::bitset<9>, 9> RowOccurences;
    std::array<std::bitset<9>, 9> ColOccurences;
    std::array<std::bitset<9>, 9> CellOccurences;

public:
    SudokuBoardLogic();

    //Query
    constexpr bool IsEmpty() noexcept;

    // Getters
    std::bitset<9>& GetRowOccurences(uint16_t row)  noexcept;
    std::bitset<9>& GetColumnOccurences(uint16_t col)  noexcept;
    std::bitset<9>& GetCellOccurences(uint16_t cell) noexcept;
    std::bitset<9>  GetTileOccurences(uint16_t row, uint16_t col) const noexcept;

    // Setters
    void ResetAll();
    void SetCellNumber(uint16_t row, uint16_t col, uint16_t number) noexcept;
    void ResetCellNumber(uint16_t row, uint16_t col, uint16_t number) noexcept;

};

// Tile's Minimum Remaining Value
class BoardMRV
{
private:
    struct TileMRV
    {
        bool            Filled;
        uint16_t        Row, Column;
        std::bitset<9> *RowOccurence = nullptr, *ColOccurence = nullptr, *CellOccurence = nullptr;

        TileMRV() = default;
        TileMRV(uint16_t row, uint16_t col, std::bitset<9>* row_occur, std::bitset<9>* col_occur, std::bitset<9>* cell_occur) : 
            Row(row), Column(col), Filled(false), RowOccurence(row_occur), ColOccurence(col_occur), CellOccurence(cell_occur) 
        {}
    };
    std::vector<TileMRV> TheMRVs;

public:
    constexpr BoardMRV() = default;
    constexpr BoardMRV(SudokuBoard& board, SudokuBoardLogic& board_logic);

    constexpr void 
    CreateMRV(SudokuBoard& board, SudokuBoardLogic& board_logic) noexcept;
    constexpr std::tuple<uint16_t, uint16_t, uint16_t>
    GetHighestTileMRV() noexcept;
    std::bitset<9>
    GetOccurences(size_t mrv_index) noexcept;
    void
    SetFilledStatus(size_t index, uint16_t number, bool filled);

};

namespace SudokuSolver
{

//----------------------------------------------------------------------------------------------------------------------------------------------
// Sudoku Helper Functions
// - Helps on solving a given sudoku board with one function
//----------------------------------------------------------------------------------------------------------------------------------------------

/*
* Solve a sudoku board.
* 
* @param board - The sudoku board. Returns the completed board
* @param board_logic - The logic of the board. Returns the completed board logic if not nullptr 
* @param method - The method to be used for solving the sudoku board. Default as Brute Force method 
* @param check_the_solutions - A flag for checking the number of solutions of the board. Default as false
* @returns true if successful, otherwise false
*/
bool
Solve(std::array<std::array<char, 9>, 9>& board, SudokuBoardLogic* board_logic = nullptr, SolveMethod method = SolveMethod_BruteForce, bool check_the_solutions = false) noexcept;
// Helper Function. Solve a sudoku board with an array of char input
bool
Solve2(std::array<char, 81>& board, SudokuBoardLogic* board_logic = nullptr, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with a vector of char as input
bool
Solve3(std::vector<char>& board, SudokuBoardLogic* board_logic = nullptr, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with an array of an array of int as input
bool
Solve4(std::array<std::array<int, 9>, 9>& board, SudokuBoardLogic* board_logic = nullptr, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with a vector of int as input
bool
Solve5(std::vector<int>& board, SudokuBoardLogic* board_logic = nullptr, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with an array of int as input
bool
Solve6(std::array<int, 81>& board, SudokuBoardLogic* board_logic = nullptr, SolveMethod method = SolveMethod_BruteForce) noexcept;

//----------------------------------------------------------------------------------------------------------------------------------------------
// Sudoku Solving Functions
// - Functions that makes use of sudoku solvers (i.e. filling a partially filled sudoku and counting a sudoku board's number of solutions)
//----------------------------------------------------------------------------------------------------------------------------------------------

// A faster sudoku solver function using Minimum Remaining Values. Used for solving external sudoku boards
bool
SolveMRV(SudokuBoard& board, SudokuBoardLogic* board_logic = nullptr) noexcept;
constexpr bool
SolveMRVEX(SudokuBoard& board, BoardMRV& board_mrvs) noexcept;
// This sudoku solver function is better used for difficulty finder due to its brute force method
bool
SolveBruteForce(SudokuBoard& board, SudokuBoardLogic* board_logic = nullptr) noexcept;
constexpr bool
SolveBruteForceEX(SudokuBoard& board, SudokuBoardLogic& board_logic, const uint16_t row_start, const uint16_t col_start) noexcept;

//----------------------------------------------------------------------------------------------------------------------------------------------
// Utility functions for the sudoku solvers
// - Functions that helps sudoku solvers and its other works
//----------------------------------------------------------------------------------------------------------------------------------------------

// A sudoku solver function but its job is to fill the remaining blanks to create a sudoku board
bool
FillSudoku(SudokuBoard& board, SudokuBoardLogic& board_logic, const std::array<uint16_t, 9>& random_numbers, uint16_t const row_start, uint16_t const col_start) noexcept;
// Checks if the board has a unique solution
bool
IsUniqueBoard(SudokuBoard& board, SudokuBoardLogic* board_logic = nullptr) noexcept;
// Count the number of solutions of the created puzzle
void
CountSolutions(SudokuBoard& board, SudokuBoardLogic& board_logic, size_t& number_of_solutions, uint16_t const row_start, uint16_t const col_start) noexcept;
// Create the board logic with the given board. Also serves as a checker for board validity
bool
CreateBoardLogic(SudokuBoard& board, SudokuBoardLogic& board_logic) noexcept;
// Clear the sudoku board (fills the array with '0's)
void
ClearSudokuBoard(SudokuBoard& board) noexcept;
// Check for the difficulty of the board
// NOTE: This function is not really a reliable difficulty checker for a sudoku board.
SudokuDifficulty
CheckSudokuDifficulty(const SudokuBoard& solution_board, const SudokuBoard& puzzle_board, const SudokuBoardLogic& puzzle_logic) noexcept;
}
