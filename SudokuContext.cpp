#include "SudokuContext.h"

//--------------------------------------------------------------------------------------------------------------------------------
// Sudoku CLASS
//--------------------------------------------------------------------------------------------------------------------------------

SudokuContext::SudokuContext() :
    GameDifficulty(2),
    RandomDifficulty(0)
{}

bool SudokuContext::InitializeSudoku(std::array<std::array<char, 9>, 9>& board) noexcept
{
    if (!SolutionBoard.CreateSudokuBoard(board)) {
        return false;
    }

    PuzzleBoard = SolutionBoard;

    if (!SudokuSolver::Solve(SolutionBoard, SolveMethod_MRV)) {
        return false;
    }

    RandomDifficulty = SudokuUtilities::CheckPuzzleDifficulty(PuzzleBoard);

    return true;
}

bool SudokuContext::InitializeSudoku(SudokuDifficulty game_difficulty) noexcept
{
    this->InitializedGameSeed();                      // Initialize game seed
    this->InitializeGameParameters(game_difficulty);  // Initialize important game parameters for creating a sudoku puzzle
    return this->CreateSudoku();                      // Create sudoku puzzle
}

void SudokuContext::InitializedGameSeed() noexcept
{
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    GameRNG.seed(seed);
}

void SudokuContext::InitializeGameParameters(SudokuDifficulty game_difficulty) noexcept
{
    this->GameDifficulty = game_difficulty;
    switch (game_difficulty)
    {
    case SudokuDifficulty_Random: {
        std::uniform_int_distribution<> max_remove_range(54, 64);
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
        MaxRemovedTiles   = 52;
        break;
    }
}

void SudokuContext::ClearAllBoards() noexcept 
{
    SolutionBoard.ClearSudokuBoard();
}

bool SudokuContext::CreateSudoku() noexcept 
{
    // Fill the grid with 27 numbers (three cells worth of numbers)
    // This is essentially the making of the solution board
    if (!this->CreateCompleteBoard()) {
        return false;
    }

    // Start making the puzzle by removing certain numbers
    // This is the making of the puzzle board
    if (!this->GeneratePuzzle()) {
        return false;
    }

    return true;
}

bool SudokuContext::CreateCompleteBoard() noexcept
{
    SolutionBoard.ClearSudokuBoard();

    static std::array<int, 9> random_numbers  = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    auto fill_diagonal_cells = [this](int start_row, int end_row, int start_col, int end_col) {
        std::shuffle(random_numbers.begin(), random_numbers.end(), GameRNG);
        int num_idx = 0;
        for (int row = start_row; row < end_row; ++row) {
            for (int col = start_col; col < end_col; ++col) {
                SolutionBoard.BoardTiles[row][col].SetTileNumber(random_numbers[num_idx]);
                num_idx++;
            }
        }
    };

    fill_diagonal_cells(0, 3, 0, 3); // We will fill the three      x o o
    fill_diagonal_cells(3, 6, 3, 6); // non-connecting diagonal     o x o
    fill_diagonal_cells(6, 9, 6, 9); // cells of the sudoku board   o o x

    std::shuffle(random_numbers.begin(), random_numbers.end(), GameRNG);
    if (!SudokuUtilities::FillSudoku(SolutionBoard, random_numbers)) {
        return false;
    }

    SolutionBoard.BoardInitialized = true;
    return true;
}

bool SudokuContext::GeneratePuzzle() noexcept
{
    PuzzleBoard = SolutionBoard;

    constexpr size_t max_number_of_tiles = 81;
    static std::array<std::pair<int, int>, max_number_of_tiles> tiles_to_be_removed = { { {0, 0},{0, 1},{0, 2},{0, 3},{0, 4},{0, 5},{0, 6},{0, 7},{0, 8},{1, 0},{1, 1},{1, 2},{1, 3},{1, 4},{1, 5},{1, 6},{1, 7},{1, 8},{2, 0},{2, 1},{2, 2},{2, 3},{2, 4},{2, 5},{2, 6},{2, 7},{2, 8},{3, 0},{3, 1},{3, 2},{3, 3},{3, 4},{3, 5},{3, 6},{3, 7},{3, 8},{4, 0},{4, 1},{4, 2},{4, 3},{4, 4},{4, 5},{4, 6},{4, 7},{4, 8},{5, 0},{5, 1},{5, 2},{5, 3},{5, 4},{5, 5},{5, 6},{5, 7},{5, 8},{6, 0},{6, 1},{6, 2},{6, 3},{6, 4},{6, 5},{6, 6},{6, 7},{6, 8},{7, 0},{7, 1},{7, 2},{7, 3},{7, 4},{7, 5},{7, 6},{7, 7},{7, 8},{8, 0},{8, 1},{8, 2},{8, 3},{8, 4},{8, 5},{8, 6},{8, 7},{8, 8} } };
    // Shuffle the array so that it would not just remove tiles from the top left to bottom right
    // because there is also a stop flag when a certain number of removed tiles is reached
    std::shuffle(tiles_to_be_removed.begin(), tiles_to_be_removed.end(), GameRNG);

    int removed_tiles = 0;
    for (int i = 0; i < max_number_of_tiles && removed_tiles < MaxRemovedTiles; ++i) {
        const auto& row = tiles_to_be_removed[i].first;
        const auto& col = tiles_to_be_removed[i].second;
        auto tile_num = PuzzleBoard.BoardTiles[row][col].TileNumber;
        if (tile_num != '0') {
            // Removes the tile
            PuzzleBoard.BoardTiles[row][col].ResetTileNumber(tile_num - '1');

            // Put back the removed tile if the board does not have a unique solution
            if (!SudokuUtilities::IsUniqueBoard(PuzzleBoard)) {
                PuzzleBoard.BoardTiles[row][col].SetTileNumber(tile_num - '1');
                continue;
            }
            removed_tiles++;
        }
    }

    // Create the neccesary puzzle tiles. Needed for solving the puzzle if someone wanted to, although there is already a solution
    PuzzleBoard.CreatePuzzleTiles();
    // Create the neccesary pencil marks of each tiles. Needed especially for most sudoku players
    PuzzleBoard.UpdateAllPencilMarks();

    if (GameDifficulty == SudokuDifficulty_Random) {
        RandomDifficulty = SudokuUtilities::CheckPuzzleDifficulty(PuzzleBoard);
    }
    else if (SudokuUtilities::CheckPuzzleDifficulty(PuzzleBoard) != GameDifficulty) {
        return CreateSudoku();
    }

    return true;
}

//----------------------------------------------------------------------
// Sudoku GETTERS
//----------------------------------------------------------------------

SudokuBoard* SudokuContext::GetPuzzleBoard() noexcept
{
    return &PuzzleBoard;
}

const SudokuDifficulty& SudokuContext::GetBoardDifficulty() const noexcept
{
    return GameDifficulty == SudokuDifficulty_Random ? RandomDifficulty : GameDifficulty;
}

const SudokuBoard* SudokuContext::GetSolutionBoard() const noexcept 
{
    return &SolutionBoard;
}

//----------------------------------------------------------------------
// Sudoku SETTERS
//----------------------------------------------------------------------

//bool SudokuContext::SetTile(int row, int col, int number)
//{
//    // In case the input 
//    if (number == (PuzzleBoard.BoardTiles[row][col].TileNumber - '0')) {
//        return true;
//    }
//
//    // Checks if the input number is valid. If not, still register but return false;
//    if (number > 0) {
//        const auto& is_used = PuzzleBoard.BoardTiles[row][col].GetTileOccurences();
//        if (is_used[number - 1]) {
//            PuzzleBoard[row][col] = static_cast<char>(number + '0');
//            return false;
//        }
//    }
//
//    const auto& tile_prev_number = PuzzleBoard[row][col];
//    PuzzleBoard[row][col] = static_cast<char>(number + '0');
//    number == 0 ? PuzzleOccurences.ResetCellNumber(row, col, tile_prev_number - '1') : PuzzleOccurences.SetCellNumber(row, col, number - 1);
//    return true;
//}

bool SudokuContext::CheckPuzzleState() const noexcept
{
    if (!SolutionBoard.IsBoardCompleted()) {
        return false;
    }

    // ADD A FUNCTION FOR CHECKING IF THE PUZZLE IS CORRECT DESPITE BEING COMPLETE
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// DEBUG FUNCTIONS
//--------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
void SudokuContext::FillPuzzleBoard()
{
    PuzzleBoard = SolutionBoard;
}
#endif



























