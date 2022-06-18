#include "Sudoku.h"

//--------------------------------------------------------------------------------------------------------------------------------
// Sudoku CLASS
//--------------------------------------------------------------------------------------------------------------------------------

SudokuContext::SudokuContext() :
    GameDifficulty(2),
    RandomDifficulty(0)
{}

bool SudokuContext::InitializeGame(SudokuBoard& board) noexcept
{
    SolutionBoard = std::move(board);
    if (!SudokuSolver::CreateBoardLogic(SolutionBoard, SolutionLogic)) {
        return false;
    }

    PuzzleBoard = SolutionBoard;
    PuzzleLogic = SolutionLogic;

    if (!SudokuSolver::Solve(SolutionBoard, &SolutionLogic, SolveMethod_MRV)) {
        return false;
    }

    RandomDifficulty = SudokuSolver::CheckSudokuDifficulty(SolutionBoard, PuzzleBoard, PuzzleLogic);

    return true;
}

bool SudokuContext::InitializeGame(SudokuDifficulty game_difficulty) noexcept
{
    this->InitializeGameParameters(game_difficulty);  // Initialize important game parameters for creating a sudoku puzzle
    this->InitializedGameSeed();                      // Initialize game seed
    return this->CreateSudoku();                      // Create sudoku puzzle
}

void SudokuContext::InitializeGameParameters(SudokuDifficulty game_difficulty) noexcept
{
    this->GameDifficulty = game_difficulty;
    switch (game_difficulty)
    {
    case SudokuDifficulty_Random: {
        std::uniform_int_distribution<> max_remove_range(44, 64);
        MaxRemovedTiles   = max_remove_range(GameRNG);
        break;
    }
    case SudokuDifficulty_Easy:
        MaxRemovedTiles   = 42;
        break;
    case SudokuDifficulty_Normal:
        MaxRemovedTiles   = 48;
        break;
    case SudokuDifficulty_Hard:
        MaxRemovedTiles   = 54;
        break;
    case SudokuDifficulty_ChadBrain:
        MaxRemovedTiles   = 64;
        break;
    default:
        MaxRemovedTiles   = 51;
        break;
    }
}

void SudokuContext::InitializedGameSeed() noexcept
{
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    GameRNG.seed(seed);
}

void SudokuContext::ClearAllBoards() noexcept 
{
    SolutionLogic.ResetAll();
    SudokuSolver::ClearSudokuBoard(SolutionBoard);
}

bool SudokuContext::CreateSudoku() noexcept 
{
    // Fill the grid with 27 numbers (three cells worth of numbers)
    if (!this->FillFullBoard()) {
        return false;
    }

    // Start making the puzzle by removing certain numbers
    return this->GeneratePuzzle();
}

bool SudokuContext::FillFullBoard() noexcept
{
    ClearAllBoards();
    static std::array<short, 9> random_numbers  = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

    auto fill_diagonal_cells = [&](int start_row, int end_row, int start_col, int end_col) {
        std::shuffle(random_numbers.begin(), random_numbers.end(), GameRNG);
        int num_idx = 0;
        for (int row = start_row; row < end_row; ++row) {
            for (int col = start_col; col < end_col; ++col) {
                SolutionBoard[row][col] = static_cast<char>(random_numbers[num_idx] + '1');
                SolutionLogic.SetCellNumber(row, col, random_numbers[num_idx]);
                num_idx++;
            }
        }
    };

    fill_diagonal_cells(0, 3, 0, 3); // We will fill the three      x o o
    fill_diagonal_cells(3, 6, 3, 6); // non-connecting diagonal     o x o
    fill_diagonal_cells(6, 9, 6, 9); // cells of the sudoku board   o o x

    std::shuffle(random_numbers.begin(), random_numbers.end(), GameRNG);
    return SudokuSolver::FillSudoku(SolutionBoard, SolutionLogic, random_numbers, 0, 0);
}

bool SudokuContext::GeneratePuzzle() noexcept 
{
    PuzzleBoard = SolutionBoard;
    PuzzleLogic = SolutionLogic;

    constexpr size_t max_number_of_tiles = 81;
    static std::array<std::pair<short, short>, max_number_of_tiles> tiles_to_be_removed = { { {0, 0},{0, 1},{0, 2},{0, 3},{0, 4},{0, 5},{0, 6},{0, 7},{0, 8},{1, 0},{1, 1},{1, 2},{1, 3},{1, 4},{1, 5},{1, 6},{1, 7},{1, 8},{2, 0},{2, 1},{2, 2},{2, 3},{2, 4},{2, 5},{2, 6},{2, 7},{2, 8},{3, 0},{3, 1},{3, 2},{3, 3},{3, 4},{3, 5},{3, 6},{3, 7},{3, 8},{4, 0},{4, 1},{4, 2},{4, 3},{4, 4},{4, 5},{4, 6},{4, 7},{4, 8},{5, 0},{5, 1},{5, 2},{5, 3},{5, 4},{5, 5},{5, 6},{5, 7},{5, 8},{6, 0},{6, 1},{6, 2},{6, 3},{6, 4},{6, 5},{6, 6},{6, 7},{6, 8},{7, 0},{7, 1},{7, 2},{7, 3},{7, 4},{7, 5},{7, 6},{7, 7},{7, 8},{8, 0},{8, 1},{8, 2},{8, 3},{8, 4},{8, 5},{8, 6},{8, 7},{8, 8} } };
    // Shuffle the array so that it would not just remove tiles from the top left to bottom right
    // because there is also a stop flag when a certain number of removed tiles is reached
    std::shuffle(tiles_to_be_removed.begin(), tiles_to_be_removed.end(), GameRNG);

    int removed_tiles = 0;
    for (int i = 0; i < max_number_of_tiles && removed_tiles < MaxRemovedTiles; ++i) {
        auto row = tiles_to_be_removed[i].first;
        auto col = tiles_to_be_removed[i].second;
        auto tile_num = PuzzleBoard[row][col];
        if (tile_num != '0') {
            // Removes the tile
            this->PuzzleLogic.ResetCellNumber(row, col, tile_num - '1');
            this->PuzzleBoard[row][col] = '0';

            // Put back the removed tile if the number of solutions is greater than one
            if (!SudokuSolver::IsUniqueBoard(PuzzleBoard, &PuzzleLogic)) {
                PuzzleBoard[row][col] = tile_num;
                PuzzleLogic.SetCellNumber(row, col, tile_num - '1');
                continue;
            }
            removed_tiles++;
        }
    }
    
    if (GameDifficulty == SudokuDifficulty_Random) {
        RandomDifficulty = SudokuSolver::CheckSudokuDifficulty(SolutionBoard, PuzzleBoard, PuzzleLogic);
    }

    return true;
}

//----------------------------------------------------------------------
// Sudoku GETTERS
//----------------------------------------------------------------------

const SudokuBoard& SudokuContext::GetPuzzleBoard() const noexcept
{
    return PuzzleBoard;
}

const SudokuDifficulty& SudokuContext::GetBoardDifficulty() const noexcept
{
    return GameDifficulty == SudokuDifficulty_Random ? RandomDifficulty : GameDifficulty;
}

const SudokuBoard& SudokuContext::GetSolutionBoard() const noexcept 
{
    return SolutionBoard;
}

//----------------------------------------------------------------------
// Sudoku SETTERS
//----------------------------------------------------------------------

bool SudokuContext::SetTile(short row, short col, short number)
{
    // In case the input 
    if (number == (PuzzleBoard[row][col] - '0')) {
        return true;
    }

    // Checks if the input number is valid. If not, still register but return false;
    if (number > 0) {
        const auto& is_used = PuzzleLogic.GetTileOccurences(row, col);
        if (is_used[number - 1]) {
            PuzzleBoard[row][col] = static_cast<char>(number + '0');
            return false;
        }
    }

    const auto& tile_prev_number = PuzzleBoard[row][col];
    PuzzleBoard[row][col] = static_cast<char>(number + '0');
    number == 0 ? PuzzleLogic.ResetCellNumber(row, col, tile_prev_number - '1') : PuzzleLogic.SetCellNumber(row, col, number - 1);
    return true;
}

bool SudokuContext::CheckGameState() const noexcept
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (PuzzleBoard[row][col] != SolutionBoard[row][col]) {
                return false;
            }
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// DEBUG FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
void SudokuContext::FillPuzzleBoard()
{
    PuzzleBoard = SolutionBoard;
    PuzzleLogic = SolutionLogic;
}
#endif



























