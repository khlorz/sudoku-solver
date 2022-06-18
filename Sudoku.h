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
    SudokuBoard        PuzzleBoard;       // The puzzle board. Should never be modified outside the class except for SetTile function
    SudokuBoardLogic   PuzzleLogic;       // The logic of the puzzle board
    SudokuBoard        SolutionBoard;     // The puzzle's actual solution
    SudokuBoardLogic   SolutionLogic;     // The actual solutions logic

public:
    SudokuContext();

    // Initialized the game with a pre-made sudoku board
    bool
    InitializeGame(SudokuBoard& board) noexcept;
    // Initialized the game with a random sudoku board
    bool
    InitializeGame(SudokuDifficulty game_difficulty) noexcept;
    // Checks if the current puzzle is already finished
    bool
    CheckGameState() const noexcept;

    // Getters
    const SudokuBoard&       GetPuzzleBoard    () const noexcept;
    const SudokuBoard&       GetSolutionBoard  () const noexcept;
    const SudokuDifficulty&  GetBoardDifficulty() const noexcept;

    // Setters
    bool SetTile(short row, short col, short number);

private:
    // Create a sudoku board
    bool
    CreateSudoku() noexcept;
    // Clear the sudoku board
    void
    ClearAllBoards() noexcept;
    // Fill the board with random numbers (still abides the sudoku rules)
    bool
    FillFullBoard() noexcept;
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