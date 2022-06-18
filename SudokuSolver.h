#pragma once

#include <vector>
#include <array>
#include <bitset>
#include <iostream>
#include <algorithm>

using SolveMethod         = int;
using SolveStartWith      = int;
using SudokuDifficulty    = int;
using UsedSudokuTechnique = int;

enum RowOrColumn_
{
    RowOrColumn_Column = 0,
    RowOrColumn_Row    = 1
};

enum SudokuDifficulty_
{
    SudokuDifficulty_Random    = 0,
    SudokuDifficulty_Easy      = 1,
    SudokuDifficulty_Normal    = 2,
    SudokuDifficulty_Hard      = 3,
    SudokuDifficulty_ChadBrain = 4
    //SudokuDifficulty_Custom    = 5     // Still work on progress
};

enum UsedSudokuTechnique_
{
    UsedSudokuTechnique_None           = 0,
    UsedSudokuTechnique_CandidateLines = 1 << 0,
    UsedSudokuTechnique_Intersections  = 1 << 1,
    UsedSudokuTechnique_NakedPair      = 1 << 2,
    UsedSudokuTechnique_NakedTriple    = 1 << 3,
    UsedSudokuTechnique_NakedQuad      = 1 << 4,
    UsedSudokuTechnique_HiddenPair     = 1 << 5,
    UsedSudokuTechnique_HiddenTriple   = 1 << 6,
    UsedSudokuTechnique_HiddenQuad     = 1 << 7,
    UsedSudokuTechnique_XWing          = 1 << 8,
    UsedSudokuTechnique_YWing          = 1 << 9,
    UsedSudokuTechnique_SwordFish      = 1 << 10
};

enum SolveMethod_
{
    SolveMethod_Humanely   = 1,    // Solve the sudoku like a human would with known sudoku techniques
    SolveMethod_BruteForce = 2,    // Typical sudoku solver method with simple backtracking method
    SolveMethod_MRV        = 3     // Sudoku solver method that uses minimum remaining values for faster solving
};

//struct SudokuVec2
//{
//    int x, y;
//    constexpr SudokuVec2()                                        { x = y = 0; }
//    constexpr SudokuVec2(int _x, int _y)                          { x = _x; y = _y; }
//    constexpr bool operator == (const SudokuVec2& other) noexcept { return this->x == other.x && this->y == other.y; }
//};

// This class is used for finding MRVs and for faster solving of the sudoku
class SudokuOccurences
{
private:
    std::array<std::bitset<9>, 9> RowOccurences;
    std::array<std::bitset<9>, 9> ColOccurences;
    std::array<std::bitset<9>, 9> CellOccurences;

public:
    SudokuOccurences();

    //Query
    constexpr bool IsEmpty() noexcept;

    // Getters
    std::bitset<9>& GetRowOccurences(int row)  noexcept;
    std::bitset<9>& GetColumnOccurences(int col)  noexcept;
    std::bitset<9>& GetCellOccurences(int cell) noexcept;
    std::bitset<9>& GetCellOccurences(int row, int col) noexcept;
    std::bitset<9>  GetTileOccurences(int row, int col) const noexcept;

    // Setters
    void ResetAll();
    void SetCellNumber(int row, int col, int number) noexcept;
    void ResetCellNumber(int row, int col, int number) noexcept;

};

// Structure holds the important parameters of a sudoku tile
struct SudokuTile
{
private:
    std::bitset<9>* RowOccurence = nullptr, * ColOccurence = nullptr, * CellOccurence = nullptr;

public:
    char            TileNumber;
    int             Row, Column, Cell;
    std::bitset<9>  Pencilmarks;        

    SudokuTile() : TileNumber('0'), Row(-1), Column(-1), Cell(-1), Pencilmarks(0) {}
    SudokuTile(std::array<std::array<char, 9>, 9>& board, SudokuOccurences& board_occurences, int row, int col, int cell) noexcept
    {
        this->CreateSudokuTile(board, board_occurences, row, col, cell);
    }

    void CreateSudokuTile(const std::array<std::array<char, 9>, 9>& board, SudokuOccurences& board_occurences, int row, int col, int cell) noexcept
    {
        Row           = row;
        Column        = col;
        Cell          = cell;
        TileNumber    = board[row][col];
        RowOccurence  = &board_occurences.GetRowOccurences(row);
        ColOccurence  = &board_occurences.GetColumnOccurences(col);
        CellOccurence = &board_occurences.GetCellOccurences(row, col);
        Pencilmarks   = *RowOccurence | *ColOccurence | *CellOccurence;
    }

    constexpr void ClearTile() noexcept
    {
        this->TileNumber  = '0';
        this->Pencilmarks = 0;
    }

    void SetTileNumber(int bit_number) noexcept
    {
        RowOccurence->set(bit_number);
        ColOccurence->set(bit_number);
        CellOccurence->set(bit_number);
        TileNumber = static_cast<char>(bit_number + '1');
    }

    void ResetTileNumber(int bit_number) noexcept
    {
        TileNumber = '0';
        RowOccurence->reset(bit_number);
        ColOccurence->reset(bit_number);
        CellOccurence->reset(bit_number);
        //UpdatePencilMarks();
    }

    std::bitset<9> GetTileOccurences() noexcept
    {
        return *RowOccurence | *ColOccurence | *CellOccurence;
    }

    void InitializeTileOccurences(SudokuOccurences& board_occurences) noexcept
    {
        RowOccurence = &board_occurences.GetRowOccurences(Row);
        ColOccurence = &board_occurences.GetColumnOccurences(Column);
        CellOccurence = &board_occurences.GetCellOccurences(Row, Column);
    }

    void UpdatePencilMarks()
    {
        Pencilmarks |= * RowOccurence | *ColOccurence | *CellOccurence;
    }

    constexpr bool IsTileFilled() const noexcept
    {
        return TileNumber != '0';
    }

    constexpr bool operator == (const SudokuTile& other) const noexcept
    {
        return this->Row == other.Row && this->Column == other.Column;
    }

};

// Contains the sudoku board object
struct SudokuBoard
{
    bool                                     BoardInitialized;
    std::array<std::array<SudokuTile, 9>, 9> BoardTiles;
    SudokuOccurences                         BoardOccurences;
    std::vector<SudokuTile*>                 PuzzleTiles;

    SudokuBoard();
    SudokuBoard(const SudokuBoard& other) noexcept;

    // Create a sudoku board from a pre-made sudoku board
    bool 
    CreateSudokuBoard(const std::array<std::array<char, 9>, 9>& sudoku_board) noexcept;
    void
    ClearSudokuBoard() noexcept;
    bool
    IsBoardCompleted() const noexcept;
    bool
    CreatePuzzleTiles() noexcept;
    SudokuTile*
    FindNextEmptyPosition(int row, int col) noexcept;
    SudokuTile* 
    FindNextEmptyPosition() noexcept;
    SudokuTile*
    FindLowestMRV() noexcept;
    void
    UpdateAllPencilMarks() noexcept;
    bool
    UpdateRowPencilMarks(int row, int bit_number, const std::vector<int>& exempted_cells) noexcept;
    bool
    UpdateRowPencilMarks(int row, int bit_number, const std::vector<SudokuTile*>& exempted_tiles) noexcept;
    bool
    UpdateColumnPencilMarks(int col, int bit_number, const std::vector<int>& exempted_cells) noexcept;
    bool
    UpdateColumnPencilMarks(int col, int bit_number, const std::vector<SudokuTile*>& exempted_tiles) noexcept;
    bool
    UpdateCellPencilMarks(int cell, int bit_number, const std::vector<SudokuTile*>& exempted_tiles) noexcept;
    bool
    UpdateCellPencilMarks(int cell, int bit_number, int exempted_line_idx, int row_or_column) noexcept;
    bool
    UpdateCellLinePencilMarks(int cell, int bit_number, int line_idx, int row_or_column) noexcept;
    bool
    UpdateTilePencilMarks(const std::vector<SudokuTile*>& sudoku_tiles, const std::vector<int> exempted_numbers) noexcept;
    bool
    IsPencilMarkPresentInTheSameCell(int cell, int bit_number, const std::vector<SudokuTile*> exempted_tiles) noexcept;
    bool
    IsPencilMarkPresentInOtherLines(int line_index, int bit_number, int row_or_column, const std::vector<SudokuTile*> exempted_tiles) noexcept;

    // Copy operator
    SudokuBoard&
    operator = (const SudokuBoard& other) noexcept;

private:
    bool CreateBoardOccurences(const std::array<std::array<char, 9>, 9>& board) noexcept;
};

namespace SudokuSolver
{

//----------------------------------------------------------------------------------------------------------------------------------------------
// Sudoku Helper Functions
// - Helps on solving a given sudoku board with one function
//----------------------------------------------------------------------------------------------------------------------------------------------

bool
Solve(SudokuBoard& sudoku_board, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with an array of an array of char
bool
Solve2(std::array<std::array<char, 9>, 9>& board, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with an array of char input
bool
Solve3(std::array<char, 81>& board, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with a vector of char as input
bool
Solve4(std::vector<char>& board, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with an array of an array of int as input
bool
Solve5(std::array<std::array<int, 9>, 9>& board, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with a vector of int as input
bool
Solve6(std::vector<int>& board, SolveMethod method = SolveMethod_BruteForce) noexcept;
// Helper Function. Solve a sudoku board with an array of int as input
bool
Solve7(std::array<int, 81>& board, SolveMethod method = SolveMethod_BruteForce) noexcept;

//----------------------------------------------------------------------------------------------------------------------------------------------
// Sudoku Solving Functions
// - Functions that makes use of sudoku solvers (i.e. filling a partially filled sudoku and counting a sudoku board's number of solutions)
//----------------------------------------------------------------------------------------------------------------------------------------------

// A faster sudoku solver function using Minimum Remaining Values. Used for solving external sudoku boards
bool
SolveMRV(SudokuBoard& puzzle_board) noexcept;
bool
SolveMRVEX(SudokuBoard& sudoku_board) noexcept;
// This sudoku solver function is better used for difficulty finder due to its brute force method
bool
SolveBruteForce(SudokuBoard& puzzle_board) noexcept;
bool
SolveBruteForceEX(SudokuBoard& sudoku_board) noexcept;
// 
bool
SolveHumanely(SudokuBoard& sudoku_board, size_t* difficulty_score = nullptr) noexcept;
bool
SolveHumanelyEX(SudokuBoard& sudoku_board, size_t& difficulty_score, UsedSudokuTechnique used_techniques = 0) noexcept;

}

namespace SudokuUtilities
{
void
PrintPencilMarks(const SudokuBoard& sudoku_board) noexcept;
// A sudoku solver function but its job is to fill the remaining blanks to create a sudoku board
bool
FillSudoku(SudokuBoard& sudoku_board, const std::array<int, 9>& random_numbers) noexcept;
// Checks if the board has a unique solution
bool
IsUniqueBoard(SudokuBoard& sudoku_board) noexcept;
// Count the number of solutions of the created puzzle
void
CountSolutions(SudokuBoard& sudoku_board, size_t& number_of_solutions, int const row_start, int const col_start) noexcept;
//
SudokuDifficulty
CheckPuzzleDifficulty(SudokuBoard& sudoku_board) noexcept;

}

namespace SudokuTekniks
{
/* @returns The number of times single position is used */
size_t
FindSinglePosition(SudokuBoard& sudoku_board) noexcept;
/* @returns The number of times single candidates is used */
size_t 
FindSingleCandidates(SudokuBoard& sudoku_board) noexcept;
/* @returns The number of times candidate lines is used */
size_t
FindCandidateLines(SudokuBoard& sudoku_board) noexcept;
/* @returns The number of times intersections is used */
size_t
FindIntersections(SudokuBoard& sudoku_board) noexcept;
/* 
*  @returns 
*  first tuple  = hidden pair count;
*  second tuple = hidden triple count;
*  third tuple  = hidden quad count;
*/
std::tuple<size_t, size_t, size_t>
FindNakedTuples(SudokuBoard& sudoku_board) noexcept;
/* 
*  @returns 
*  first tuple  = hidden pair count; 
*  second tuple = hidden triple count; 
*  third tuple  = hidden quad count; 
*/
std::tuple<size_t, size_t, size_t>
FindHiddenTuples(SudokuBoard& sudoku_board) noexcept;
/* @returns The number of times x-wing is used */
size_t
FindXWings(SudokuBoard& sudoku_board) noexcept;
/* @returns The number of times x-wing is used */
size_t
FindYWings(SudokuBoard& sudoku_board) noexcept;
/* @returns The number of times swordfish is used */
size_t
FindSwordFish(SudokuBoard& sudoku_board) noexcept;

}






