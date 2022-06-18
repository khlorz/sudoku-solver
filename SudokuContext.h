#pragma once

#include <random>
#include <chrono>
#include "SudokuSolver.h"

class SudokuContext
{
private:
    SudokuDifficulty   GameDifficulty;    // Stores the current game difficulty
    SudokuDifficulty   RandomDifficulty;  // Store the actual difficulty if the game difficulty is random or custom
    size_t             MaxRemovedTiles;   // Max removed tiles for certain difficulties. The lower it is, the easier the difficulty could be
    std::mt19937_64    GameRNG;           // Game's Random Number Generator for generating the puzzle
    SudokuBoard        SolutionBoard;     // Stores the solution of the sudoku board
    SudokuBoard        PuzzleBoard;       // Stores the puzzle of the sudoku board

public:
    SudokuContext();

    // Initialized the game with a pre-made sudoku board
    bool
    InitializeSudoku(std::array<std::array<char, 9>, 9>& board) noexcept;
    // Initialized the game with a random sudoku board
    bool
    InitializeSudoku(SudokuDifficulty game_difficulty) noexcept;
    // Checks if the current puzzle is already finished
    bool
    CheckPuzzleState() const noexcept;

    // Getters
    SudokuBoard*             GetPuzzleBoard    () noexcept;
    const SudokuBoard*       GetSolutionBoard  () const noexcept;
    const SudokuDifficulty&  GetBoardDifficulty() const noexcept;

    // Setters
    bool SetTile(int row, int col, int number);

private:
    // Create a sudoku board
    bool
    CreateSudoku() noexcept;
    // Clear the sudoku board
    void
    ClearAllBoards() noexcept;
    // Fill the board with random numbers (still abides the sudoku rules)
    bool
    CreateCompleteBoard() noexcept;
    // Make a puzzle out of the created sudoku board
    bool
    GeneratePuzzle() noexcept;
    // Initialize the seed for randomizing numbers
    void
    InitializedGameSeed() noexcept;
    // Initialize the important parameters for creating a sudoku board
    void
    InitializeGameParameters(SudokuDifficulty game_difficulty) noexcept;

// DEBUG FUNCTIONS
#ifdef _DEBUG
public:
    void FillPuzzleBoard();
#endif
};