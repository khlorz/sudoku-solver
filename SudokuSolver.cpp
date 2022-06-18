#include "SudokuSolver.h"
#include <functional>

// Functions for querying the sudoku board rows/columns/cells
namespace SudokuQuery
{

constexpr int GetNextRow(int row, int col) noexcept
{
    return row + (col + 1) / 9;
}

constexpr int GetNextCol(int col) noexcept
{
    return (col + 1) % 9;
}

constexpr int GetCellBlock(int row, int col) noexcept
{
    return (row / 3) * 3 + col / 3;
}

constexpr std::tuple<int, int, int, int> GetMinMaxRowColumnFromCell(int const cell) noexcept
{
    int min_row, max_row;
    int min_col, max_col;

    switch (cell)
    {
    case 0:
        min_row = 0; max_row = 3;
        min_col = 0; max_col = 3;
        break;
    case 1:
        min_row = 0; max_row = 3;
        min_col = 3; max_col = 6;
        break;
    case 2:
        min_row = 0; max_row = 3;
        min_col = 6; max_col = 9;
        break;
    case 3:
        min_row = 3; max_row = 6;
        min_col = 0; max_col = 3;
        break;
    case 4:
        min_row = 3; max_row = 6;
        min_col = 3; max_col = 6;
        break;
    case 5:
        min_row = 3; max_row = 6;
        min_col = 6; max_col = 9;
        break;
    case 6:
        min_row = 6; max_row = 9;
        min_col = 0; max_col = 3;
        break;
    case 7:
        min_row = 6; max_row = 9;
        min_col = 3; max_col = 6;
        break;
    case 8:
        min_row = 6; max_row = 9;
        min_col = 6; max_col = 9;
        break;
    default:
        min_row = 0; max_row = 0;
        min_col = 0; max_col = 0;
        break;
    }

    return { min_row, max_row, min_col, max_col };
}

}



//--------------------------------------------------------------------------------------------------------------------------------
// SudokuOccurences CLASS
//--------------------------------------------------------------------------------------------------------------------------------

SudokuOccurences::SudokuOccurences() :
    RowOccurences({ 0, 0, 0, 0, 0, 0, 0, 0, 0 }),
    ColOccurences({ 0, 0, 0, 0, 0, 0, 0, 0, 0 }),
    CellOccurences({ 0, 0, 0, 0, 0, 0, 0, 0, 0 })
{}

constexpr bool SudokuOccurences::IsEmpty() noexcept
{
    size_t total_count = 0;
    for (size_t idx = 0; idx < 9; ++idx) {
        total_count += RowOccurences[idx].count() + ColOccurences[idx].count() + CellOccurences[idx].count();
    }

    return total_count == 0;
}

void SudokuOccurences::ResetAll()
{
    RowOccurences = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ColOccurences = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CellOccurences = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
}

void SudokuOccurences::SetCellNumber(int row, int col, int number) noexcept
{
    RowOccurences[row].set(number);
    ColOccurences[col].set(number);
    const auto& cellblock = SudokuQuery::GetCellBlock(row, col);
    CellOccurences[cellblock].set(number);
}

std::bitset<9>& SudokuOccurences::GetRowOccurences(int row) noexcept
{
    return RowOccurences[row];
}

std::bitset<9>& SudokuOccurences::GetColumnOccurences(int col) noexcept
{
    return ColOccurences[col];
}

std::bitset<9>& SudokuOccurences::GetCellOccurences(int cell) noexcept
{
    return CellOccurences[cell];
}

std::bitset<9> SudokuOccurences::GetTileOccurences(int row, int col) const noexcept
{
    const auto& cell = SudokuQuery::GetCellBlock(row, col);
    return RowOccurences[row] | ColOccurences[col] | CellOccurences[cell];
}

void SudokuOccurences::ResetCellNumber(int row, int col, int number) noexcept
{
    RowOccurences[row].reset(number);
    ColOccurences[col].reset(number);
    const auto& cell = SudokuQuery::GetCellBlock(row, col);
    CellOccurences[cell].reset(number);
}

std::bitset<9>& SudokuOccurences::GetCellOccurences(int row, int col) noexcept
{
    const auto& cell = SudokuQuery::GetCellBlock(row, col);
    return CellOccurences[cell];
}

//--------------------------------------------------------------------------------------------------------------------------------
// SudokuBoard Structure
//--------------------------------------------------------------------------------------------------------------------------------

//----------------------------------
// SudokuBoard Constructors
//----------------------------------

SudokuBoard::SudokuBoard() : BoardInitialized(false)
{
    PuzzleTiles.reserve(64);
    BoardOccurences.ResetAll();
    constexpr std::array<std::array<char, 9>, 9> empty_board = { { {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'},
                                                                   {'0', '0', '0', '0', '0', '0', '0', '0', '0'} } };

    for(int row = 0; row < 9; ++row){
        for(int col = 0; col < 9; ++col){
            BoardTiles[row][col].CreateSudokuTile(empty_board, 
                                                  BoardOccurences, 
                                                  row, 
                                                  col, 
                                                  SudokuQuery::GetCellBlock(row, col));
        }
    }
}

SudokuBoard::SudokuBoard(const SudokuBoard& other) noexcept
{
    PuzzleTiles.reserve(64);
    *this = other;
}

//----------------------------------
// SudokuBoard Operators
//----------------------------------

SudokuBoard& SudokuBoard::operator = (const SudokuBoard& other) noexcept
{
    this->BoardInitialized = other.BoardInitialized;
    this->BoardOccurences = other.BoardOccurences;

    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            this->BoardTiles[row][col].TileNumber    = other.BoardTiles[row][col].TileNumber;
            this->BoardTiles[row][col].Cell          = other.BoardTiles[row][col].Cell;
            this->BoardTiles[row][col].Row           = other.BoardTiles[row][col].Row;
            this->BoardTiles[row][col].Column        = other.BoardTiles[row][col].Column;
            this->BoardTiles[row][col].Pencilmarks   = other.BoardTiles[row][col].Pencilmarks;
            this->BoardTiles[row][col].InitializeTileOccurences(this->BoardOccurences);
        }
    }

    // Do not create new puzzle tiles if the former does not have one!
    if (other.PuzzleTiles.size() == 0) {
        return *this;
    }

    // Create new puzzle tiles because the pointers will be pointing to a different objects!
    this->CreatePuzzleTiles();
    return *this;
}

//----------------------------------
// SudokuBoard Queries
//----------------------------------

bool SudokuBoard::IsBoardCompleted() const noexcept
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (BoardTiles[row][col].TileNumber == '0')
                return false;
        }
    }

    return true;
}

//----------------------------------
// SudokuBoard Basic Functions
//----------------------------------

bool SudokuBoard::CreateSudokuBoard(const std::array<std::array<char, 9>, 9>& sudoku_board) noexcept
{
    if(!this->CreateBoardOccurences(sudoku_board)){
        BoardInitialized = false;
        return false;
    }

    for(int row = 0; row < 9; ++row){
        for(int col = 0; col < 9; ++col){
            BoardTiles[row][col].CreateSudokuTile(sudoku_board, 
                                                  BoardOccurences, 
                                                  row, 
                                                  col, 
                                                  SudokuQuery::GetCellBlock(row, col));
        }
    }

    BoardInitialized = true;
    return CreatePuzzleTiles();
}

bool SudokuBoard::CreateBoardOccurences(const std::array<std::array<char, 9>, 9>& board) noexcept
{
    BoardOccurences.ResetAll();

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (board[row][col] < '0' || board[row][col] > '9') {
                return false;
            }
            char digit;
            if ((digit = board[row][col]) != '0') {
                int digit_num = digit - '1';
                if (BoardOccurences.GetTileOccurences(row, col)[digit_num]) {
                    return false;
                }
                BoardOccurences.SetCellNumber(row, col, digit_num);
            }
        }
    }

    return true;
}

void SudokuBoard::ClearSudokuBoard() noexcept
{
    for (auto& row_tiles : BoardTiles) {
        for (auto& tile : row_tiles) {
            tile.ClearTile();
        }
    }

    BoardInitialized = false;
    BoardOccurences.ResetAll();
    PuzzleTiles.clear();
}

bool SudokuBoard::CreatePuzzleTiles() noexcept
{
    if (!BoardInitialized) {
        return false;
    }

    PuzzleTiles.clear();
    for (auto& row_tiles : BoardTiles) {
        for (auto& tile : row_tiles) {
            if (tile.TileNumber == '0') {
                PuzzleTiles.push_back(&tile);
            }
        }
    }

    return true;
}

//------------------------------------------------------
// SudokuBoard Backtracking Sudoku Solver Queries
//------------------------------------------------------

SudokuTile* SudokuBoard::FindNextEmptyPosition(int row, int col) noexcept
{
    while (row != 9) {
        if (BoardTiles[row][col].TileNumber == '0') {
            return &BoardTiles[row][col];
        }
        row = SudokuQuery::GetNextRow(row, col);
        col = SudokuQuery::GetNextCol(col);
    }

    return nullptr;
}

SudokuTile* SudokuBoard::FindNextEmptyPosition() noexcept
{
    for (auto& puzzle_tile : PuzzleTiles) {
        if (puzzle_tile->TileNumber == '0') {
            return puzzle_tile;
        }
    }

    return nullptr;
}

SudokuTile* SudokuBoard::FindLowestMRV() noexcept
{
    int lowest_mrv = -1;
    int highest_mrv_index = 0;
    for (int idx = 0; idx < PuzzleTiles.size(); ++idx) {
        if (PuzzleTiles[idx]->IsTileFilled()) {
            continue;
        }
        
        const auto& current_mrv_sum = PuzzleTiles[idx]->GetTileOccurences();
        const int& count = current_mrv_sum.count();
        if (lowest_mrv < count) {
            lowest_mrv  = count;
            highest_mrv_index = idx;
        }
    }

    if (lowest_mrv == -1) {
        return nullptr;
    }

    return PuzzleTiles[highest_mrv_index];
}

//------------------------------------------------
// SudokuBoard Pencilmark Functions
//------------------------------------------------

void SudokuBoard::UpdateAllPencilMarks() noexcept
{
    for (auto& row_tiles : BoardTiles) {
        for (auto& tile : row_tiles) {
            if (tile.TileNumber == '0') {
                tile.UpdatePencilMarks();
            }
        }
    }
}

bool SudokuBoard::UpdateRowPencilMarks(int row, int bit_number, const std::vector<int>& exempted_cells) noexcept
{
    int count = 0;

    for (int col = 0; col < 9; ++col) {
        if (BoardTiles[row][col].IsTileFilled() || BoardTiles[row][col].Pencilmarks[bit_number] || std::find_if(exempted_cells.begin(), exempted_cells.end(), [&](const int& cell_num) { return cell_num == BoardTiles[row][col].Cell; }) != exempted_cells.end()) {
            continue;
        }

        BoardTiles[row][col].Pencilmarks.set(bit_number);
        ++count;
    }

    return count != 0;
}

bool SudokuBoard::UpdateRowPencilMarks(int row, int bit_number, const std::vector<SudokuTile*>& exempted_tiles) noexcept
{
    int count = 0;

    for (int col = 0; col < 9; ++col) {
        if (BoardTiles[row][col].IsTileFilled() || BoardTiles[row][col].Pencilmarks[bit_number] || std::find_if(exempted_tiles.begin(), exempted_tiles.end(), [&](const SudokuTile* tile) { return *tile == BoardTiles[row][col]; }) != exempted_tiles.end()) {
            continue;
        }

        BoardTiles[row][col].Pencilmarks.set(bit_number);
        ++count;
    }

    return count != 0;
}

bool SudokuBoard::UpdateColumnPencilMarks(int col, int bit_number, const std::vector<int>& exempted_cells) noexcept
{
    int count = 0;

    for (int row = 0; row < 9; ++row) {
        if (BoardTiles[row][col].IsTileFilled() || BoardTiles[row][col].Pencilmarks[bit_number] || std::find_if(exempted_cells.begin(), exempted_cells.end(), [&](const int& cell_num) { return cell_num == BoardTiles[row][col].Cell; }) != exempted_cells.end()) {
            continue;
        }

        BoardTiles[row][col].Pencilmarks.set(bit_number);
        ++count;
    }

    return count != 0;
}

bool SudokuBoard::UpdateColumnPencilMarks(int col, int bit_number, const std::vector<SudokuTile*>& exempted_tiles) noexcept
{
    int count = 0;

    for (int row = 0; row < 9; ++row) {
        if (BoardTiles[row][col].IsTileFilled() || BoardTiles[row][col].Pencilmarks[bit_number] || std::find_if(exempted_tiles.begin(), exempted_tiles.end(), [&](const SudokuTile* tile) { return *tile == BoardTiles[row][col]; }) != exempted_tiles.end()) {
            continue;
        }

        BoardTiles[row][col].Pencilmarks.set(bit_number);
        ++count;
    }

    return count != 0;
}

bool SudokuBoard::UpdateCellPencilMarks(int cell, int bit_number, const std::vector<SudokuTile*>& exempted_tiles) noexcept
{
    size_t count = 0;
    const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);

    for (int row = min_row; row < max_row; ++row) {
        for (int col = min_col; col < max_col; ++col) {
            if (BoardTiles[row][col].IsTileFilled() || BoardTiles[row][col].Pencilmarks[bit_number] || std::find_if(exempted_tiles.begin(), exempted_tiles.end(), [&, this](const SudokuTile* tile) {return *tile == BoardTiles[row][col]; }) != exempted_tiles.end()) {
                continue;
            }

            BoardTiles[row][col].Pencilmarks.set(bit_number);
            ++count;
        }
    }

    return count != 0;
}

bool SudokuBoard::UpdateCellPencilMarks(int cell, int bit_number, int exempted_line_idx, int row_or_column) noexcept
{
    size_t count = 0;
    const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);

    for (int row = min_row; row < max_row; ++row) {
        for (int col = min_col; col < max_col; ++col) {
            if (BoardTiles[row][col].IsTileFilled() || BoardTiles[row][col].Pencilmarks[bit_number] || (row_or_column ? row == exempted_line_idx : col == exempted_line_idx)) {
                continue;
            }

            BoardTiles[row][col].Pencilmarks.set(bit_number);
            ++count;
        }
    }

    return count != 0;
}

bool SudokuBoard::UpdateCellLinePencilMarks(int cell, int bit_number, int line_idx, int row_or_column) noexcept
{
    size_t count = 0;
    const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);

    if (row_or_column == RowOrColumn_Row) {
        for (int col = min_col; col < max_col; ++col) {
            if (BoardTiles[line_idx][col].IsTileFilled() || BoardTiles[line_idx][col].Pencilmarks[bit_number]) {
                continue;
            }

            BoardTiles[line_idx][col].Pencilmarks.set(bit_number);
            ++count;
        }
    }
    else {
        for (int row = min_row; row < max_row; ++row) {
            if (BoardTiles[row][line_idx].IsTileFilled() || BoardTiles[row][line_idx].Pencilmarks[bit_number]) {
                continue;
            }

            BoardTiles[row][line_idx].Pencilmarks.set(bit_number);
            ++count;
        }
    }

    return count != 0;
}

bool SudokuBoard::UpdateTilePencilMarks(const std::vector<SudokuTile*>& sudoku_tiles, const std::vector<int> exempted_numbers) noexcept
{
    size_t count = 0;

    for (auto& tile : sudoku_tiles) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            if (tile->Pencilmarks[bit_num] || std::find_if(exempted_numbers.begin(), exempted_numbers.end(), [&](int num){ return num == bit_num; }) != exempted_numbers.end()) {
                continue;
            }

            ++count;
            tile->Pencilmarks.set(bit_num);
        }
    }

    return count != 0;
}

bool SudokuBoard::IsPencilMarkPresentInTheSameCell(int cell, int bit_number, const std::vector<SudokuTile*> exempted_tiles) noexcept
{
    const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);

    for (int row = min_row; row < max_row; ++row) {
        for (int col = min_col; col < max_col; ++col) {
            if (BoardTiles[row][col].IsTileFilled() || std::find_if(exempted_tiles.begin(), exempted_tiles.end(), [&](const SudokuTile* tile) { return *tile == BoardTiles[row][col]; }) != exempted_tiles.end()) {
                continue;
            }

            if (!BoardTiles[row][col].Pencilmarks[bit_number]) {
                return true;
            }
        }
    }
    
    return false;
}

bool SudokuBoard::IsPencilMarkPresentInOtherLines(int line_index, int bit_number, int row_or_column, const std::vector<SudokuTile*> exempted_tiles) noexcept
{
    if (row_or_column == RowOrColumn_Row) {
        for (int col = 0; col < 9; ++col) {
            if (BoardTiles[line_index][col].IsTileFilled() || std::find_if(exempted_tiles.begin(), exempted_tiles.end(), [&](const SudokuTile* tile) { return *tile == BoardTiles[line_index][col]; }) != exempted_tiles.end()) {
                continue;
            }

            if (!BoardTiles[line_index][col].Pencilmarks[bit_number]) {
                return true;
            }
        }
    }
    else {
        for (int row = 0; row < 9; ++row) {
            if (BoardTiles[row][line_index].IsTileFilled() || std::find_if(exempted_tiles.begin(), exempted_tiles.end(), [&](const SudokuTile* tile) { return *tile == BoardTiles[row][line_index]; }) != exempted_tiles.end()) {
                continue;
            }

            if (!BoardTiles[row][line_index].Pencilmarks[bit_number]) {
                return true;
            }
        }
    }

    return false;
}

//--------------------------------------------------------------------------------------------------------------------------------
// SudokuSolver Namespace
//--------------------------------------------------------------------------------------------------------------------------------

namespace SudokuSolver
{

//----------------------------------------------------------------------------------------------------------------------------------------------
// Sudoku Helper Functions
// - Helps on solving a given sudoku board with one function
//----------------------------------------------------------------------------------------------------------------------------------------------

bool Solve(SudokuBoard& sudoku_board, SolveMethod method) noexcept
{
    switch (method)
    {
    case SolveMethod_BruteForce:
        return SolveBruteForce(sudoku_board);
    case SolveMethod_MRV:
        return SolveMRV(sudoku_board);
    case SolveMethod_Humanely:
        return SolveHumanely(sudoku_board);
    default:
        return false;
    }
}

bool Solve2(std::array<std::array<char, 9>, 9>& board, SolveMethod method) noexcept
{
    SudokuBoard sudoku_board;
    if(!sudoku_board.CreateSudokuBoard(board)){
        return false;
    }

    Solve(sudoku_board, method);

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            board[row][col] = sudoku_board.BoardTiles[row][col].TileNumber;
        }
    }

    return sudoku_board.IsBoardCompleted();
}

bool Solve3(std::array<char, 81>& board, SolveMethod method) noexcept
{
    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num];
            tile_num++;
        }
    }

    if (!Solve2(converted_board, method)) {
        return false;
    }

    tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            board[tile_num] = converted_board[row][col];
            ++tile_num;
        }
    }

    return true;
}

bool Solve4(std::vector<char>& board, SolveMethod method) noexcept
{
    if (board.size() != 81) {
        return false;
    }

    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num];
            tile_num++;
        }
    }

    if (!Solve2(converted_board, method)) {
        return false;
    }

    tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            board[tile_num] = converted_board[row][col];
            ++tile_num;
        }
    }

    return true;
}

bool Solve5(std::array<std::array<int, 9>, 9>& board, SolveMethod method) noexcept
{
    std::array<std::array<char, 9>, 9> converted_board;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[row][col] + '0';
        }
    }

    if (!Solve2(converted_board, method)) {
        return false;
    }

    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            board[row][col] = converted_board[row][col] - '0';
        }
    }

    return true;
}

bool Solve6(std::vector<int>& board, SolveMethod method) noexcept
{
    if (board.size() != 81) {
        return false;
    }

    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num] + '0';
            tile_num++;
        }
    }

    if (!Solve2(converted_board, method)) {
        return false;
    }

    tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            board[tile_num] = converted_board[row][col] - '0';
            ++tile_num;
        }
    }

    return true;
}

bool Solve7(std::array<int, 81>& board, SolveMethod method) noexcept
{
    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num] + '0';
            ++tile_num;
        }
    }

    if (!Solve2(converted_board, method)) {
        return false;
    }

    tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            board[tile_num] = converted_board[row][col] - '0';
            ++tile_num;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------
// Sudoku Solving Functions
// - Functions that makes use of sudoku solvers (i.e. filling a partially filled sudoku and counting a sudoku board's number of solutions)
//----------------------------------------------------------------------------------------------------------------------------------------------

bool SolveBruteForceEX(SudokuBoard& sudoku_board) noexcept
{
    auto* puzzle_tile = sudoku_board.FindNextEmptyPosition();

    if (puzzle_tile == nullptr) {
        return true;
    }

    const std::bitset<9>& occurences = puzzle_tile->GetTileOccurences();
    if (occurences.all()) {
        return false;
    }

    for (int bit_number = 0; bit_number < 9; ++bit_number) {
        if (occurences[bit_number]) {
            continue;
        }

        puzzle_tile->SetTileNumber(bit_number);
        if (SolveBruteForceEX(sudoku_board)) {
            return true;
        }
        puzzle_tile->ResetTileNumber(bit_number);
    }

    return false;
}

bool SolveBruteForce(SudokuBoard& sudoku_board) noexcept
{
    if (!sudoku_board.BoardInitialized) {
        return false;
    }

    return SolveBruteForceEX(sudoku_board);
}

bool SolveMRVEX(SudokuBoard& sudoku_board) noexcept
{
    auto* puzzle_tile = sudoku_board.FindLowestMRV();

    if (puzzle_tile == nullptr) {
        return true;
    }

    const std::bitset<9>& occurences = puzzle_tile->GetTileOccurences();
    if (occurences.all()) {
        return false;
    }

    for (int digit_idx = 0; digit_idx < 9; ++digit_idx) {
        if (occurences[digit_idx]) {
            continue;
        }

        puzzle_tile->SetTileNumber(digit_idx);
        if (SolveMRVEX(sudoku_board)) {
            return true;
        }
        puzzle_tile->ResetTileNumber(digit_idx);
    }

    return false;
}

bool SolveMRV(SudokuBoard& sudoku_board) noexcept
{
    if (!sudoku_board.BoardInitialized) {
        return false;
    }

    return SolveMRVEX(sudoku_board);
}

bool SolveHumanelyEX(SudokuBoard& sudoku_board, size_t& difficulty_score, UsedSudokuTechnique used_techniques) noexcept
{
    if (sudoku_board.IsBoardCompleted()) {
        return true;
    }

    // These are the functions that fills out the board with the sure number
    // As such, if successful, the all board pencilmarks should be updated accordingly
    if (size_t count = SudokuTekniks::FindSingleCandidates(sudoku_board)) {
        difficulty_score += count * 100;
        sudoku_board.UpdateAllPencilMarks();
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    if (size_t count = SudokuTekniks::FindSinglePosition(sudoku_board)) {
        difficulty_score += count * 100;
        sudoku_board.UpdateAllPencilMarks();
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    // These are functions for removing pencilmarks to decrease the number of possible numbers in tiles
    // As such, updating the pencilmarks is not needed
    if (size_t count = SudokuTekniks::FindCandidateLines(sudoku_board)) {
        difficulty_score += (used_techniques & UsedSudokuTechnique_CandidateLines) ? count * 200 : 350 + ((count - 1) * 200);
        used_techniques |= UsedSudokuTechnique_CandidateLines;
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    if (size_t count = SudokuTekniks::FindIntersections(sudoku_board)) {
        difficulty_score += (used_techniques & UsedSudokuTechnique_Intersections) ? count * 500 : 800 + ((count - 1) * 500);
        used_techniques |= UsedSudokuTechnique_Intersections;
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    {
        const auto& [pair_count, triple_count, quad_count] = SudokuTekniks::FindNakedTuples(sudoku_board);
        bool found = false;
        if (pair_count > 0) {
            difficulty_score += (used_techniques & UsedSudokuTechnique_NakedPair) ? pair_count * 500 : 750 + ((pair_count - 1) * 500);
            used_techniques |= UsedSudokuTechnique_NakedPair;
            found = true;
        }
        if (triple_count > 0) {
            difficulty_score += (used_techniques & UsedSudokuTechnique_NakedTriple) ? triple_count * 1400 : 2000 + ((triple_count - 1) * 1400);
            used_techniques |= UsedSudokuTechnique_NakedTriple;
            found = true;
        }
        if (quad_count > 0) {
            difficulty_score += (used_techniques & UsedSudokuTechnique_NakedQuad) ? quad_count * 4000 : 5000 + ((quad_count - 1) * 4000);
            used_techniques |= UsedSudokuTechnique_NakedQuad;
            found = true;
        }
        if (found) {
            return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
        }
    }

    {
        const auto& [pair_count, triple_count, quad_count] = SudokuTekniks::FindHiddenTuples(sudoku_board);
        bool found = false;
        if (pair_count > 0) {
            difficulty_score += (used_techniques & UsedSudokuTechnique_HiddenPair) ? pair_count * 1200 : 1500 + ((pair_count - 1) * 1200);
            used_techniques |= UsedSudokuTechnique_HiddenPair;
            found = true;
        }
        if (triple_count > 0) {
            difficulty_score += (used_techniques & UsedSudokuTechnique_HiddenTriple) ? triple_count * 1600 : 2400 + ((triple_count - 1) * 1600);
            used_techniques |= UsedSudokuTechnique_HiddenTriple;
            found = true;
        }
        if (quad_count > 0) {
            difficulty_score += (used_techniques & UsedSudokuTechnique_HiddenQuad) ? quad_count * 5000 : 7000 + ((quad_count - 1) * 5000);
            used_techniques |= UsedSudokuTechnique_HiddenQuad;
            found = true;
        }
        if (found) {
            return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
        }
    }

    if (size_t count = SudokuTekniks::FindXWings(sudoku_board)) {
        difficulty_score += (used_techniques & UsedSudokuTechnique_XWing) ? count * 2000 : 3000 + ((count - 1) * 2000);
        used_techniques |= UsedSudokuTechnique_XWing;
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    if (size_t count = SudokuTekniks::FindYWings(sudoku_board)) {
        difficulty_score += (used_techniques & UsedSudokuTechnique_YWing) ? count * 2500 : 4000 + ((count - 1) * 2500);
        used_techniques |= UsedSudokuTechnique_YWing;
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    if (size_t count = SudokuTekniks::FindSwordFish(sudoku_board)) {
        difficulty_score += (used_techniques & UsedSudokuTechnique_SwordFish) ? count * 4000 : 5000 + ((count - 1) * 4000);
        used_techniques |= UsedSudokuTechnique_SwordFish;
        return SolveHumanelyEX(sudoku_board, difficulty_score, used_techniques);
    }

    return false;
}

bool SolveHumanely(SudokuBoard& sudoku_board, size_t* difficulty_score) noexcept
{
    if (!sudoku_board.BoardInitialized) {
        return false;
    }

    if (difficulty_score == nullptr) {
        size_t new_difficulty_score = 0;
        return SolveHumanelyEX(sudoku_board, new_difficulty_score);
    }

    return SolveHumanelyEX(sudoku_board, *difficulty_score);
}

}

namespace SudokuUtilities
{

bool FillSudokuEX(SudokuBoard& sudoku_board, const std::array<int, 9>& random_numbers, const int row_start, const int col_start) noexcept
{
    auto* puzzle_tile = sudoku_board.FindNextEmptyPosition(row_start, col_start);

    if (puzzle_tile == nullptr) {
        return true;
    }

    const std::bitset<9>& occurences = puzzle_tile->GetTileOccurences();
    if (occurences.all()) {
        return false;
    }

    for (int idx = 0; idx < 9; ++idx) {
        auto digit = random_numbers[idx];
        if (occurences[digit]) {
            continue;
        }

        puzzle_tile->SetTileNumber(digit);
        if (FillSudokuEX(sudoku_board, random_numbers, puzzle_tile->Row, puzzle_tile->Column)) {
            return true;
        }
        puzzle_tile->ResetTileNumber(digit);
    }

    return false;
}

bool FillSudoku(SudokuBoard& sudoku_board, const std::array<int, 9>& random_numbers) noexcept
{
    return FillSudokuEX(sudoku_board, random_numbers, 0, 0);
}

void CountSolutions(SudokuBoard& sudoku_board, size_t& number_of_solutions, int const row_start, int const col_start) noexcept
{
    auto* puzzle_tile = sudoku_board.FindNextEmptyPosition(row_start, col_start);

    // end of board 
    if (puzzle_tile == nullptr) {
        ++number_of_solutions;
        return;
    }

    std::bitset<9> const occurences = puzzle_tile->GetTileOccurences();
    if (occurences.all()) {
        return;
    }

    for (int digit_idx = 0; digit_idx < 9 && number_of_solutions < 2; ++digit_idx) {
        if (occurences[digit_idx]) {
            continue;
        }

        puzzle_tile->SetTileNumber(digit_idx);
        CountSolutions(sudoku_board, number_of_solutions, puzzle_tile->Row, puzzle_tile->Column);
        puzzle_tile->ResetTileNumber(digit_idx);
    }
}

bool IsUniqueBoard(SudokuBoard& sudoku_board) noexcept
{
    size_t number_of_solutions = 0;
    CountSolutions(sudoku_board, number_of_solutions, 0, 0);
    return number_of_solutions == 1;
}

void PrintPencilMarks(const SudokuBoard& sudoku_board) noexcept
{
    printf("-------------------------------------------------------\n");
    for (int row = 0; row < 27; ++row) {
        int actual_row = row / 3;
        int min_bit = row % 3 * 3;
        int max_bit = min_bit + 3;
        printf("| ");
        for (int col = 0; col < 9; ++col) {
            for (int bit_num = min_bit; bit_num < max_bit; ++bit_num) {
                sudoku_board.BoardTiles[actual_row][col].IsTileFilled() || sudoku_board.BoardTiles[actual_row][col].Pencilmarks[bit_num] ?
                    (std::cout << ' ') : (std::cout << bit_num + 1);
            }
            std::cout << " | ";
        }
        printf("\n");
        if (actual_row < (row + 1) / 3) {
            printf("-------------------------------------------------------\n");
        }
    }
}

SudokuDifficulty CheckPuzzleDifficulty(SudokuBoard& sudoku_board) noexcept
{
    size_t difficulty_score = 0;
    SudokuSolver::SolveHumanely(sudoku_board, &difficulty_score);

    if (difficulty_score < 4000 && sudoku_board.IsBoardCompleted()) {
        return SudokuDifficulty_Easy;
    }
    if (difficulty_score > 4000 && difficulty_score < 10000 && sudoku_board.IsBoardCompleted()) {
        return SudokuDifficulty_Normal;
    }
    if (difficulty_score > 10000 && difficulty_score < 18000 && sudoku_board.IsBoardCompleted()) {
        return SudokuDifficulty_Hard;
    }
    if (!sudoku_board.IsBoardCompleted()) {
        size_t blank_count = std::count_if(sudoku_board.PuzzleTiles.begin(), sudoku_board.PuzzleTiles.end(), [&](const SudokuTile* tile) { return !tile->IsTileFilled(); });
        if (blank_count < 5 && difficulty_score < 2000) {
            return SudokuDifficulty_Easy;
        }
        if (blank_count < 10 && difficulty_score < 5000 && difficulty_score > 2000) {
            return SudokuDifficulty_Normal;
        }
        if (blank_count < 20 && difficulty_score < 10000 && difficulty_score > 5000) {
            return SudokuDifficulty_Hard;
        }
    }
    return SudokuDifficulty_ChadBrain;
}

}

namespace SudokuTekniks
{
//
size_t FindSinglePosition(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    for (int cell = 0; cell < 9; ++cell) {
        const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            int bit_count = 0;
            SudokuTile* single_position_tile = nullptr;
            for (int row = min_row; row < max_row; ++row) {
                for (int col = min_col; col < max_col; ++col) {
                    if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                        continue;
                    }

                    if (!sudoku_board.BoardTiles[row][col].Pencilmarks[bit_num]) {
                        ++bit_count;
                        single_position_tile = &sudoku_board.BoardTiles[row][col];
                    }
                }
            }
            if (bit_count == 1) {
                single_position_tile->SetTileNumber(bit_num);
                ++count;
            }
        }
    }

    return count;
}
//
size_t FindSingleCandidates(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    for (auto& tile : sudoku_board.PuzzleTiles) {
        if (tile->IsTileFilled()) {
            continue;
        }
        const std::bitset<9>& occurences = ~tile->Pencilmarks;
        if (occurences.count() != 1) {
            continue;
        }

        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            if (!occurences[bit_num]) {
                continue;
            }

            tile->SetTileNumber(bit_num);
            ++count;
            break;
        }
    }

    return count;
}
//
size_t FindCandidateLines(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    auto candidate_lines_lambda = [&](std::array<std::bitset<9>, 3> total_line_bitset, int min_line_index, int cell_number, int row_or_column) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            int bit_count = 0;
            int line_idx = 0;
            for (int bitset_idx = 0; bitset_idx < 3; ++bitset_idx) {
                if (!total_line_bitset[bitset_idx][bit_num]) {
                    ++bit_count;
                    line_idx = bitset_idx + min_line_index;
                }
            }
            if (bit_count == 1) {
                if (row_or_column ? sudoku_board.UpdateRowPencilMarks(line_idx, bit_num, std::vector<int>({ cell_number }) ) : 
                                    sudoku_board.UpdateColumnPencilMarks(line_idx, bit_num, std::vector<int>({ cell_number }))) {
                    ++count;
                }
            }
        }
    };

    auto& board_tiles = sudoku_board.BoardTiles;
    for (int cell = 0; cell < 9; ++cell) {
        const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);
        {
            std::array<std::bitset<9>, 3> total_row_bitset = { 0, 0, 0 };
            for (int row = min_row; row < max_row; ++row) {
                std::bitset<9> row_bitset = 0;
                for (int col = min_col; col < max_col; ++col) {
                    if (board_tiles[row][col].TileNumber == '0') {
                        row_bitset |= ~board_tiles[row][col].Pencilmarks;
                    }
                }
                total_row_bitset[min_row == 0 ? row : row % min_row] = ~row_bitset;
            }
            candidate_lines_lambda(total_row_bitset, min_row, cell, RowOrColumn_Row);
        }
        {
            std::array<std::bitset<9>, 3> total_col_bitset = { 0, 0, 0 };
            for (int col = min_col; col < max_col; ++col) {
                std::bitset<9> col_bitset = 0;
                for (int row = min_row; row < max_row; ++row) {
                    if (board_tiles[row][col].TileNumber == '0') {
                        col_bitset |= ~board_tiles[row][col].Pencilmarks;
                    }
                }
                total_col_bitset[min_col ? col % min_col : col] = ~col_bitset;
            }
            candidate_lines_lambda(total_col_bitset, min_col, cell, RowOrColumn_Column);
        }
    }

    return count;
}
//
size_t FindIntersections(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    auto intersection_lambda = [&](int line_index, const int row_or_column, const std::array<std::bitset<9>, 3> line_bitsets) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            int bit_count = 0;
            int intersection_cell_idx = 0;
            for (int arr_idx = 0; arr_idx < line_bitsets.size(); ++arr_idx) {
                if (!line_bitsets[arr_idx][bit_num]) {
                    bit_count++;
                    intersection_cell_idx = row_or_column ? arr_idx + ((line_index / 3) * 3) : (arr_idx * 3) + (line_index / 3); // col 1 + ((row / 3) * 3)
                }
            }
            if (bit_count == 1) {
                if (sudoku_board.UpdateCellPencilMarks(intersection_cell_idx, bit_num, line_index, row_or_column)) {
                    ++count;
                }
            }
        }
    };

    std::array<std::bitset<9>, 3> row_bitsets;
    for (int row = 0; row < 9; ++row) {
        row_bitsets = { 0, 0, 0 };
        for (auto& bits : row_bitsets) { bits.flip(); }
        for (int col = 0; col < 9; ++col) {
            if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                continue;
            }

            int array_index = SudokuQuery::GetCellBlock(row, col) % 3;
            row_bitsets[array_index] &= sudoku_board.BoardTiles[row][col].Pencilmarks;
        }
        intersection_lambda(row, RowOrColumn_Row, row_bitsets);
    }

    std::array<std::bitset<9>, 3> col_bitsets;
    for (int col = 0; col < 9; ++col) {
        col_bitsets = { 0, 0, 0};
        for (auto& bits : col_bitsets) { bits.flip(); }
        for (int row = 0; row < 9; ++row) {
            if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                continue;
            }

            int array_index = SudokuQuery::GetCellBlock(row, col) / 3;
            col_bitsets[array_index] &= sudoku_board.BoardTiles[row][col].Pencilmarks;
        }
        intersection_lambda(col, RowOrColumn_Column, col_bitsets);
    }

    return count;
}
//
std::tuple<size_t, size_t, size_t> FindNakedTuples(SudokuBoard& sudoku_board) noexcept
{
    size_t pair_count   = 0;
    size_t triple_count = 0;
    size_t quad_count   = 0;

    auto find_naked_tuple_lambda = [&](const std::vector<SudokuTile*>& tuple_tiles) {
        if (tuple_tiles.size() == 2) {
            return tuple_tiles[0]->Pencilmarks == tuple_tiles[1]->Pencilmarks && ~(tuple_tiles[0]->Pencilmarks.count()) == 2 ? tuple_tiles[0]->Pencilmarks : ~std::bitset<9>(0);
        }

        int number_of_tuples = 0;
        for (int bit_number = 0; bit_number < 9; ++bit_number) {
            int bit_count = 0;
            for (auto& tile : tuple_tiles) {
                if (!tile->Pencilmarks[bit_number]) {
                    ++bit_count;
                }
            }
            if      (bit_count >= 2) { ++number_of_tuples; }
            else if (bit_count == 1) { return ~std::bitset<9>(0); }
        }

        if (number_of_tuples != tuple_tiles.size()) {
            return ~std::bitset<9>(0);
        }

        std::bitset<9> output_bitset = 0;
        for (auto& tile : tuple_tiles) {
            output_bitset |= ~tile->Pencilmarks;
        }
        return ~output_bitset;
    };

    {
        std::vector<SudokuTile*> naked_tuple_tiles;
        naked_tuple_tiles.reserve(4);
        for (int cell = 0; cell < 9; ++cell) {
            const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);
            auto naked_tuple_cell = [&](int tuple_number) {
                auto recursive_tuple_cell = [&](int row, int col, auto& recursive_function) {
                    do
                    {
                        col++;
                        if (col == max_col) {
                            col = min_col;
                            ++row;
                            if (row == max_row) {
                                return;
                            }
                        }
                    } while (sudoku_board.BoardTiles[row][col].IsTileFilled());

                    naked_tuple_tiles.push_back(&sudoku_board.BoardTiles[row][col]);
                    if (naked_tuple_tiles.size() == tuple_number) {
                        const std::bitset<9>& resultant_lambda = find_naked_tuple_lambda(naked_tuple_tiles);
                        if (!resultant_lambda.all()) {
                            for (int bit_num = 0; bit_num < 9; ++bit_num) {
                                if (resultant_lambda[bit_num]) {
                                    continue;
                                }

                                if (sudoku_board.UpdateCellPencilMarks(cell, bit_num, naked_tuple_tiles)) {
                                    switch (tuple_number)
                                    {
                                    case 2: pair_count++;   break;
                                    case 3: triple_count++; break;
                                    case 4: quad_count++;   break;
                                    default: break;
                                    }
                                    for (auto& tiles : naked_tuple_tiles) {

                                    }
                                    // Add another function to check if all the tiles are of the same row or column
                                }
                            }
                        }
                        naked_tuple_tiles.erase(naked_tuple_tiles.end() - 1);
                    }
                    recursive_function(row, col, recursive_function);
                };
                for (int row = min_row; row < max_row; ++row) {
                    for (int col = min_col; col < max_col; ++col) {
                        if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                            continue;
                        }
                        
                        naked_tuple_tiles.push_back(&sudoku_board.BoardTiles[row][col]);
                        recursive_tuple_cell(row, col, recursive_tuple_cell);
                        naked_tuple_tiles.clear();
                    }
                }
            };

            naked_tuple_cell(2);
            naked_tuple_cell(3);
            naked_tuple_cell(4);
        }
    }

    {
        std::vector<SudokuTile*> naked_tuple_tiles;
        naked_tuple_tiles.reserve(4);
        auto naked_tuple_lines = [&](int tuple_number, int line_idx, int row_or_column, const std::array<std::vector<SudokuTile*>, 3>& tuple_tiles) {
            auto recursive_tuple_line = [&](int arr_idx, int vec_idx, auto& recursive_function) {
                if (arr_idx == tuple_tiles.size()) {
                    return;
                }
                if (tuple_tiles[arr_idx].empty()) {
                    arr_idx++;
                    if (arr_idx == tuple_tiles.size()) {
                        return;
                    }
                    vec_idx = -1;
                }
                vec_idx++;
                if (vec_idx == tuple_tiles[arr_idx].size()) {
                    vec_idx = 0;
                    arr_idx++;
                    if (arr_idx == tuple_tiles.size()) {
                        return;
                    }
                    if (tuple_tiles[arr_idx].empty()) {
                        arr_idx++;
                        if (arr_idx == tuple_tiles.size()) {
                            return;
                        }
                        vec_idx = -1;
                    }
                }

                naked_tuple_tiles.push_back(tuple_tiles[arr_idx][vec_idx]);
                if (naked_tuple_tiles.size() == tuple_number) {
                    const std::bitset<9>& resultant_lambda = find_naked_tuple_lambda(naked_tuple_tiles);
                    if (!resultant_lambda.all()) {
                        for (int bit_num = 0; bit_num < 9; ++bit_num) {
                            if (resultant_lambda[bit_num]) {
                                continue;
                            }

                            if (row_or_column ? sudoku_board.UpdateRowPencilMarks(line_idx, bit_num, naked_tuple_tiles) : sudoku_board.UpdateColumnPencilMarks(line_idx, bit_num, naked_tuple_tiles)) {
                                switch (tuple_number)
                                {
                                case 2: pair_count++;   break;
                                case 3: triple_count++; break;
                                case 4: quad_count++;   break;
                                default: break;
                                }
                                // Add another function to check if all the tiles are of the same row or column
                            }
                        }
                    }
                    naked_tuple_tiles.erase(naked_tuple_tiles.end() - 1);
                }
                recursive_function(arr_idx, vec_idx, recursive_function);
            };
            for (int arr_idx = 0; arr_idx < tuple_tiles.size(); ++arr_idx) {
                for (int vec_idx = 0; vec_idx < tuple_tiles[arr_idx].size(); ++vec_idx) {
                    naked_tuple_tiles.push_back(tuple_tiles[arr_idx][vec_idx]);
                    arr_idx++;
                    recursive_tuple_line(arr_idx, -1, recursive_tuple_line);
                    arr_idx--;
                    naked_tuple_tiles.clear();
                }
            }
        };

        // Naked Tuples in rows
        std::array<std::vector<SudokuTile*>, 3> naked_tuples_row;
        for (int row = 0; row < 9; ++row) {
            for (auto& row_tiles : naked_tuples_row) {
                row_tiles.clear();
            }
            for (int col = 0; col < 9; ++col) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                    continue;
                }

                int array_index = SudokuQuery::GetCellBlock(row, col) % 3;
                naked_tuples_row[array_index].push_back(&sudoku_board.BoardTiles[row][col]);
            }
            naked_tuple_lines(2, row, RowOrColumn_Row, naked_tuples_row);
            naked_tuple_lines(3, row, RowOrColumn_Row, naked_tuples_row);
            naked_tuple_lines(4, row, RowOrColumn_Row, naked_tuples_row);
        }

        // Naked Tuples in columns
        std::array<std::vector<SudokuTile*>, 3> naked_tuples_col;
        for (int col = 0; col < 9; ++col) {
            for (auto& row_tiles : naked_tuples_col) {
                row_tiles.clear();
            }
            for (int row = 0; row < 9; ++row) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                    continue;
                }

                int array_index = SudokuQuery::GetCellBlock(row, col) / 3;
                naked_tuples_col[array_index].push_back(&sudoku_board.BoardTiles[row][col]);
            }
            naked_tuple_lines(2, col, RowOrColumn_Column, naked_tuples_col);
            naked_tuple_lines(3, col, RowOrColumn_Column, naked_tuples_col);
            naked_tuple_lines(4, col, RowOrColumn_Column, naked_tuples_col);
        }
    }

    return { pair_count, triple_count, quad_count };
}
//
std::tuple<size_t, size_t, size_t> FindHiddenTuples(SudokuBoard& sudoku_board) noexcept
{
    size_t pair_count   = 0;
    size_t triple_count = 0;
    size_t quad_count   = 0;

    auto find_hidden_tuple_lambda = [&](const std::vector<SudokuTile*>& hidden_tuples, int cell_or_line, int cell_or_line_idx, int row_or_column = RowOrColumn_Row) {
        std::vector<int> possible_hidden_tuple_numbers;
        possible_hidden_tuple_numbers.reserve(6);

        // This is to find the numbers that are more than appears more than twice
        for (int bit_number = 0; bit_number < 9; ++bit_number) {
            int bit_count = 0;
            for (auto& tile : hidden_tuples) {
                if (!tile->Pencilmarks[bit_number]) {
                    ++bit_count;
                }
            }
            if (bit_count >= 2) {
                possible_hidden_tuple_numbers.push_back(bit_number);
            }
        }

        // The size of hidden_tiles is the tuple_number i.e. size == 2 is hidden pair
        // The size of possibilities should be equal or higher than the tuple number
        if (possible_hidden_tuple_numbers.size() < hidden_tuples.size()) {
            return;
        }

        // Finding the actual hidden tuple numbers
        // They are the numbers that does not appear on the same cell or line(row or column)
        std::vector<int> actual_hidden_tuple_numbers;
        actual_hidden_tuple_numbers.reserve(hidden_tuples.size());
        for (auto& numbers : possible_hidden_tuple_numbers) {
            if (cell_or_line ? !sudoku_board.IsPencilMarkPresentInTheSameCell(cell_or_line_idx, numbers, hidden_tuples) : !sudoku_board.IsPencilMarkPresentInOtherLines(cell_or_line_idx, numbers, row_or_column, hidden_tuples)) {
                actual_hidden_tuple_numbers.push_back(numbers);
            }
        }

        // If the size of the actual numbers is the same as the tuple number, then it's a hidden tuples!
        if (actual_hidden_tuple_numbers.size() == hidden_tuples.size()) {
            if (sudoku_board.UpdateTilePencilMarks(hidden_tuples, actual_hidden_tuple_numbers)) {
                switch (hidden_tuples.size())
                {
                case 2:  pair_count++;
                case 3:  triple_count++;
                case 4:  quad_count++;
                default: break;
                }
            }
        }
    };

    {   // Find the hidden tuples in cells
        std::vector<SudokuTile*> hidden_tuple_tiles;
        hidden_tuple_tiles.reserve(4);
        for (int cell = 0; cell < 9; ++cell) {
            const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);
            // Main lambda for finding hidden tuples in cells
            auto find_hidden_tuple_cell = [&](int tuple_number) {
                // Recursive lambda for populating possible hidden tuple tiles
                auto recursive_find_hidden_tuple_cell = [&](int row, int col, auto& self_function) {
                    do {
                        col++;
                        if (col == max_col) {
                            col = min_col;
                            ++row;
                            if (row == max_row) {
                                return;
                            }
                        }
                    } while (sudoku_board.BoardTiles[row][col].IsTileFilled());

                    hidden_tuple_tiles.push_back(&sudoku_board.BoardTiles[row][col]);
                    if (hidden_tuple_tiles.size() == tuple_number) {
                        find_hidden_tuple_lambda(hidden_tuple_tiles, 1, cell);
                        hidden_tuple_tiles.erase(hidden_tuple_tiles.end() - 1);
                    }
                    self_function(row, col, self_function);
                };
                for (int row = min_row; row < max_row; ++row) {
                    for (int col = min_col; col < max_col; ++col) {
                        if (sudoku_board.BoardTiles[row][col].IsTileFilled()) { 
                            continue; 
                        }
                        hidden_tuple_tiles.push_back(&sudoku_board.BoardTiles[row][col]);
                        recursive_find_hidden_tuple_cell(row, col, recursive_find_hidden_tuple_cell);
                        hidden_tuple_tiles.clear();
                    }
                }
            };

            find_hidden_tuple_cell(2);
            find_hidden_tuple_cell(3);
            find_hidden_tuple_cell(4);
        }
    }

    {   // Find the hidden tuples in lines (row or column)
        std::vector<SudokuTile*> hidden_tuple_tiles;
        hidden_tuple_tiles.reserve(4);
        // Start of finding hidden tuple in lines lambda
        auto find_hidden_tuple_lines = [&](int tuple_number, int line_idx, int row_or_column, const std::array<std::vector<SudokuTile*>, 3>& tuple_tiles) {
            // Start of recursive tuple line lambda
            auto recursive_hidden_tuple_line = [&](int arr_idx, int vec_idx, auto& recursive_function) {
                if (arr_idx == tuple_tiles.size()) {
                    return;
                }
                while (tuple_tiles[arr_idx].empty()) {
                    arr_idx++;
                    if (arr_idx == tuple_tiles.size()) {
                        return;
                    }
                    vec_idx = -1;
                }
                vec_idx++;
                if (vec_idx == tuple_tiles[arr_idx].size()) {
                    vec_idx = 0;
                    arr_idx++;
                    if (arr_idx == tuple_tiles.size()) {
                        return;
                    }
                    while (tuple_tiles[arr_idx].empty()) {
                        arr_idx++;
                        if (arr_idx == tuple_tiles.size()) {
                            return;
                        }
                        vec_idx = -1;
                    }
                }

                hidden_tuple_tiles.push_back(tuple_tiles[arr_idx][vec_idx]);
                if (hidden_tuple_tiles.size() == tuple_number) {
                    find_hidden_tuple_lambda(hidden_tuple_tiles, 0, line_idx, row_or_column);
                    hidden_tuple_tiles.erase(hidden_tuple_tiles.end() - 1);
                }
                recursive_function(arr_idx, vec_idx, recursive_function);
            };
            // End of recursive tuple line lambda

            for (int arr_idx = 0; arr_idx < tuple_tiles.size(); ++arr_idx) {
                for (int vec_idx = 0; vec_idx < tuple_tiles[arr_idx].size(); ++vec_idx) {
                    hidden_tuple_tiles.push_back(tuple_tiles[arr_idx][vec_idx]);
                    arr_idx++;
                    recursive_hidden_tuple_line(arr_idx, -1, recursive_hidden_tuple_line);
                    arr_idx--;
                    hidden_tuple_tiles.clear();
                }
            }
        };
        // End of finding hidden tuple in lines lambda

        // Hidden Tuples in rows
        std::array<std::vector<SudokuTile*>, 3> hidden_tuples_row;
        for (int row = 0; row < 9; ++row) {
            for (auto& row_tiles : hidden_tuples_row) {
                row_tiles.clear();
            }
            for (int col = 0; col < 9; ++col) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                    continue;
                }

                int array_index = SudokuQuery::GetCellBlock(row, col) % 3;
                hidden_tuples_row[array_index].push_back(&sudoku_board.BoardTiles[row][col]);
            }
            find_hidden_tuple_lines(2, row, RowOrColumn_Row, hidden_tuples_row);
            find_hidden_tuple_lines(3, row, RowOrColumn_Row, hidden_tuples_row);
            find_hidden_tuple_lines(4, row, RowOrColumn_Row, hidden_tuples_row);
        }

        // Hidden Tuples in columns
        std::array<std::vector<SudokuTile*>, 3> hidden_tuples_col;
        for (int col = 0; col < 9; ++col) {
            for (auto& row_tiles : hidden_tuples_col) {
                row_tiles.clear();
            }
            for (int row = 0; row < 9; ++row) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled()) {
                    continue;
                }

                int array_index = SudokuQuery::GetCellBlock(row, col) / 3;
                hidden_tuples_col[array_index].push_back(&sudoku_board.BoardTiles[row][col]);
            }
            find_hidden_tuple_lines(2, col, RowOrColumn_Column, hidden_tuples_col);
            find_hidden_tuple_lines(3, col, RowOrColumn_Column, hidden_tuples_col);
            find_hidden_tuple_lines(4, col, RowOrColumn_Column, hidden_tuples_col);
        }
    }

    return { pair_count, triple_count, quad_count };
}
//
size_t FindXWings(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    int row = 0;
    std::vector<SudokuTile*> possible_x_wings;     possible_x_wings.reserve(5);
    std::vector<SudokuTile*> possible_x_wing_pair; possible_x_wing_pair.reserve(5);

    // Max number of rows to be search will only be 6(0 - 5)!
    // Because if we search x-wings beyond row 5, then there would only be x-wing within cells, which is already done with candidate lines and intersections!
    for (int row = 0; row < 6; ++row) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            possible_x_wings.clear();
            for (int col = 0; col < 9; ++col) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled() || sudoku_board.BoardTiles[row][col].Pencilmarks[bit_num]) {
                    continue;
                }

                possible_x_wings.push_back(&sudoku_board.BoardTiles[row][col]);
            }

            // If there are no two possible x-wing, then there's no x-wing for that number
            // If there are two possible x-wing, but they are of the same cell, then it's also not an x-wing
            if (possible_x_wings.size() != 2 || possible_x_wings[0]->Cell == possible_x_wings[1]->Cell) {
                continue;
            }

            auto find_x_wing_pair_row = [&sudoku_board, &possible_x_wings, &possible_x_wing_pair, &count, bit_num](int current_row, auto& self_function) {
                current_row++;
                if (current_row == 9) {
                    return;
                }

                // Check first if the number of the supposed x_wing number is two, otherwise, it's not possible, so move on to the next row
                possible_x_wing_pair.clear();
                for (int col = 0; col < 9; ++col) {
                    if (sudoku_board.BoardTiles[current_row][col].TileNumber == '0' && !sudoku_board.BoardTiles[current_row][col].Pencilmarks[bit_num]) {
                        possible_x_wing_pair.push_back(&sudoku_board.BoardTiles[current_row][col]);
                    }
                }
                if (possible_x_wing_pair.size() != 2) {
                    return self_function(current_row, self_function);
                }

                // Check first if the both of the corresponding tiles has the x_wing numbers otherwise, move on to the next row
                // Checks if the corresponding tile is blank
                if (sudoku_board.BoardTiles[current_row][possible_x_wings[0]->Column].IsTileFilled() || sudoku_board.BoardTiles[current_row][possible_x_wings[1]->Column].IsTileFilled()) {
                    return self_function(current_row, self_function);
                }
                // Checks if it is the corresponding tile
                if (possible_x_wing_pair[0]->Column != possible_x_wings[0]->Column || possible_x_wing_pair[1]->Column != possible_x_wings[1]->Column) {
                    return self_function(current_row, self_function);
                }
                // Checks if the corresponding tile has the pencilmark number
                if (sudoku_board.BoardTiles[current_row][possible_x_wings[0]->Column].Pencilmarks[bit_num] || sudoku_board.BoardTiles[current_row][possible_x_wings[1]->Column].Pencilmarks[bit_num]) {
                    return self_function(current_row, self_function);
                }

                // If all other flags failed, meaning, there is an x-wing!
                possible_x_wings.push_back(possible_x_wing_pair[0]);
                possible_x_wings.push_back(possible_x_wing_pair[1]);

                // Apply neccesary steps for removing pencilmarks
                bool success = sudoku_board.UpdateColumnPencilMarks(possible_x_wings[0]->Column, bit_num, possible_x_wings);
                success      = sudoku_board.UpdateColumnPencilMarks(possible_x_wings[1]->Column, bit_num, possible_x_wings);
                if (success) {
                    ++count;
                }
            };

            int starting_row = ((row / 3) * 3) + 3 - 1;
            find_x_wing_pair_row(starting_row, find_x_wing_pair_row);
        }
    }

    // The same reason as with rows
    for (int col = 0; col < 6; ++col) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            possible_x_wings.clear();
            for (int row = 0; row < 9; ++row) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled() || sudoku_board.BoardTiles[row][col].Pencilmarks[bit_num]) {
                    continue;
                }

                possible_x_wings.push_back(&sudoku_board.BoardTiles[row][col]);
            }

            // If there are no two possible x-wing, then there's no x-wing for that number
            // If there are two possible x-wing, but they are of the same cell, then it's also not an x-wing
            if (possible_x_wings.size() != 2 || possible_x_wings[0]->Cell == possible_x_wings[1]->Cell) {
                continue;
            }

            auto find_x_wing_pair_column = [&sudoku_board, &possible_x_wings, &possible_x_wing_pair, &count, bit_num](int current_column, auto& self_function) {
                current_column++;
                if (current_column == 9) {
                    return;
                }

                // Check first if the number of the supposed x_wing number is two, otherwise, it's not possible, so move on to the next row
                possible_x_wing_pair.clear();
                for (int row = 0; row < 9; ++row) {
                    if (sudoku_board.BoardTiles[row][current_column].TileNumber == '0' && !sudoku_board.BoardTiles[row][current_column].Pencilmarks[bit_num]) {
                        possible_x_wing_pair.push_back(&sudoku_board.BoardTiles[row][current_column]);
                    }
                }
                if (possible_x_wing_pair.size() != 2) {
                    return self_function(current_column, self_function);
                }

                // Check first if the both of the corresponding tiles has the x_wing numbers otherwise, move on to the next row
                if (sudoku_board.BoardTiles[possible_x_wings[0]->Row][current_column].IsTileFilled() || sudoku_board.BoardTiles[possible_x_wings[1]->Row][current_column].IsTileFilled()) {
                    return self_function(current_column, self_function);
                }
                if (possible_x_wing_pair[0]->Row != possible_x_wings[0]->Row || possible_x_wing_pair[1]->Row != possible_x_wings[1]->Row) {
                    return self_function(current_column, self_function);
                }
                if (sudoku_board.BoardTiles[possible_x_wings[0]->Row][current_column].Pencilmarks[bit_num] || sudoku_board.BoardTiles[possible_x_wings[0]->Row][current_column].Pencilmarks[bit_num]) {
                    return self_function(current_column, self_function);
                }

                // If all other flags failed, meaning, there is an x-wing!
                possible_x_wings.push_back(possible_x_wing_pair[0]);
                possible_x_wings.push_back(possible_x_wing_pair[1]);

                // Apply neccesary steps for removing pencilmarks
                bool success = sudoku_board.UpdateRowPencilMarks(possible_x_wings[0]->Row, bit_num, possible_x_wings);
                success      = sudoku_board.UpdateRowPencilMarks(possible_x_wings[1]->Row, bit_num, possible_x_wings);
                if (success) {
                    ++count;
                }
            };

            int starting_column = ((col / 3) * 3) + 3 - 1;
            find_x_wing_pair_column(starting_column, find_x_wing_pair_column);
        }
    }

    return count;
}
//
size_t FindYWings(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    // We first get the first possible pivot tile
    // A pivot tile should have 2 pencilmarks
    for (auto& pivot_tile_1 : sudoku_board.PuzzleTiles) {
        if (pivot_tile_1->IsTileFilled() || pivot_tile_1->Pencilmarks.count() != 7) {
            continue;
        }

        std::array<int, 2> pivot_numbers;
        int bit_count = 0;
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            if (!pivot_tile_1->Pencilmarks[bit_num]) {
                pivot_numbers[bit_count] = bit_num;
                bit_count++;
            }
        }

        auto find_y_wing_lambda = [&count, &sudoku_board, &pivot_tile_1, &pivot_numbers](int pincer_line_alignment, int line_idx, bool stop_finding_in_lines) {
            // Here we try to find another possible pivot tile that is aligned with the first pivot tile
            // If the pincer line alignment is in row, then we should traverse in column and vise versa
            // Finding the next possible pivot tile should start on the next cell!
            int starting_line_idx = (((pincer_line_alignment ? pivot_tile_1->Column : pivot_tile_1->Row) / 3) * 3) + 3;
            for (int opposing_line_idx = starting_line_idx; opposing_line_idx < 9; ++opposing_line_idx) {
                auto& pivot_tile_2 = pincer_line_alignment ? sudoku_board.BoardTiles[line_idx][opposing_line_idx] : sudoku_board.BoardTiles[opposing_line_idx][line_idx];
                // Same as with the first pivot tile, but the second pivot tile should not be equal to the first pivot tile
                if (pivot_tile_2.IsTileFilled() || pivot_tile_2.Pencilmarks.count() != 7 || pivot_tile_2.Pencilmarks == pivot_tile_1->Pencilmarks) {
                    continue;
                }

                // This assumes that the pivot_tile_1 is the real pivot tile and pivot_tile_2 is the first pincer tile
                // As for the opposite, then the pincer number would be the complementary_pivot_number
                int pincer_number;
                int complementary_pivot_number = -1;
                for (int bit_num = 0; bit_num < 9; ++bit_num) {
                    if (!pivot_tile_2.Pencilmarks[bit_num]) {
                        bit_num == pivot_numbers[0] ? complementary_pivot_number = pivot_numbers[1] :
                            bit_num == pivot_numbers[1] ? complementary_pivot_number = pivot_numbers[0] : pincer_number = bit_num;
                    }
                }

                if (complementary_pivot_number == -1) {
                    continue;
                }

                auto find_second_pincer_in_cell = [&count, &sudoku_board, &line_idx, &pincer_line_alignment](SudokuTile& pivot_tile, SudokuTile& pincer_tile_1, int pincer_number, int complementary_number) {
                    const int cell = SudokuQuery::GetCellBlock(pivot_tile.Row, pivot_tile.Column);
                    const auto& [min_row, max_row, min_col, max_col] = SudokuQuery::GetMinMaxRowColumnFromCell(cell);

                    for (int row = min_row; row < max_row; ++row) {
                        for (int col = min_col; col < max_col; ++col) {
                            if (pincer_line_alignment ? row == line_idx : col == line_idx) {
                                continue;
                            }

                            if (sudoku_board.BoardTiles[row][col].IsTileFilled() || sudoku_board.BoardTiles[row][col].Pencilmarks.count() != 7) {
                                continue;
                            }

                            if (sudoku_board.BoardTiles[row][col].Pencilmarks[complementary_number] || sudoku_board.BoardTiles[row][col].Pencilmarks[pincer_number]) {
                                continue;
                            }

                            if (sudoku_board.UpdateCellLinePencilMarks(pincer_tile_1.Cell, pincer_number, (pincer_line_alignment ? row : col), pincer_line_alignment)) {
                                ++count;
                            }
                        }
                    }
                };

                auto find_second_pincer_in_row = [&count, &sudoku_board](SudokuTile& pivot_tile, SudokuTile& pincer_tile_1, int pincer_number, int complementary_number) {
                    int pincer_row = pivot_tile.Row;
                    int starting_col = ((pivot_tile.Column / 3) * 3) + 3;
                    if (starting_col == 9) {
                        starting_col = 0;
                    }

                    for (int col = starting_col; sudoku_board.BoardTiles[pincer_row][col].Cell != pivot_tile.Cell; ++col) {
                        if (sudoku_board.BoardTiles[pincer_row][col].IsTileFilled() || sudoku_board.BoardTiles[pincer_row][col].Pencilmarks.count() != 7) {
                            if (col == 8) {
                                col = -1;
                            }
                            continue;
                        }

                        if (sudoku_board.BoardTiles[pincer_row][col].Pencilmarks[complementary_number] || sudoku_board.BoardTiles[pincer_row][col].Pencilmarks[pincer_number]) {
                            if (col == 8) {
                                col = -1;
                            }
                            continue;
                        }

                        if (!sudoku_board.BoardTiles[pincer_tile_1.Row][col].Pencilmarks[pincer_number]) {
                            sudoku_board.BoardTiles[pincer_tile_1.Row][col].Pencilmarks.set(pincer_number);
                            ++count;
                        }

                        if (col == 8) {
                            col = -1;
                        }
                    }
                };

                auto find_second_pincer_in_col = [&count, &sudoku_board](SudokuTile& pivot_tile, SudokuTile& pincer_tile_1, int pincer_number, int complementary_number) {
                    int pincer_col = pivot_tile.Column;
                    int starting_row = ((pivot_tile.Row / 3) * 3) + 3;
                    if (starting_row == 9) {
                        starting_row = 0;
                    }

                    for (int row = starting_row; sudoku_board.BoardTiles[row][pincer_col].Cell != pivot_tile.Cell; ++row) {
                        if (sudoku_board.BoardTiles[row][pincer_col].IsTileFilled() || sudoku_board.BoardTiles[row][pincer_col].Pencilmarks.count() != 7) {
                            if (row == 8) {
                                row = -1;
                            }
                            continue;
                        }

                        if (sudoku_board.BoardTiles[row][pincer_col].Pencilmarks[complementary_number] || sudoku_board.BoardTiles[row][pincer_col].Pencilmarks[pincer_number]) {
                            if (row == 8) {
                                row = -1;
                            }
                            continue;
                        }

                        if (!sudoku_board.BoardTiles[row][pincer_tile_1.Column].Pencilmarks[pincer_number]) {
                            sudoku_board.BoardTiles[row][pincer_tile_1.Column].Pencilmarks.set(pincer_number);
                            ++count;
                        }

                        if (row == 8) {
                            row = -1;
                        }
                    }
                };

                find_second_pincer_in_cell(*pivot_tile_1, pivot_tile_2, pincer_number, complementary_pivot_number);
                find_second_pincer_in_cell(pivot_tile_2, *pivot_tile_1, complementary_pivot_number, pincer_number);

                // This is a flag to not repeat the same operation after the first one (you usually find y-wings through rows then columns)
                if (!stop_finding_in_lines) {
                    pincer_line_alignment ? find_second_pincer_in_col(*pivot_tile_1, pivot_tile_2, pincer_number, complementary_pivot_number) : find_second_pincer_in_row(*pivot_tile_1, pivot_tile_2, pincer_number, complementary_pivot_number);
                    pincer_line_alignment ? find_second_pincer_in_col(pivot_tile_2, *pivot_tile_1, complementary_pivot_number, pincer_number) : find_second_pincer_in_row(pivot_tile_2, *pivot_tile_1, complementary_pivot_number, pincer_number);
                }
            }
        };

        find_y_wing_lambda(RowOrColumn_Row, pivot_tile_1->Row, true);
        find_y_wing_lambda(RowOrColumn_Column, pivot_tile_1->Column, false);
    }

    return count;
}
//
size_t FindSwordFish(SudokuBoard& sudoku_board) noexcept
{
    size_t count = 0;

    // Initialized before the loop so no multiple instances of recreating the vector over and over. 
    // Also preserves the capacity so resizing will be minimal or none at all.
    std::vector<SudokuTile*> first_swordfish_tiles; first_swordfish_tiles.reserve(4); // This contains the first swordfish tiles
    std::vector<SudokuTile*> other_swordfish_tiles; other_swordfish_tiles.reserve(4); // This contains the other swordfish tiles
    std::vector<SudokuTile*> final_swordfish_tiles; final_swordfish_tiles.reserve(9); // This contains the combination of first and other swordfish tiles

    auto final_swordfish_step = [&count, final_swordfish_tiles, &sudoku_board](int row_or_column, int bit_num) {
        // A flag for a stray row for early exit
        bool stray_row = false;
        std::vector<int> aligned_lines;
        if (row_or_column == RowOrColumn_Row) {
            for (int line_idx = 0; line_idx < 9; ++line_idx) {
                const size_t line_count = std::count_if(final_swordfish_tiles.begin(), final_swordfish_tiles.end(), [&](const SudokuTile* obj) { return obj->Column == line_idx; });
                if (line_count == 1) {
                    stray_row = true;
                    break;
                }
                if (line_count >= 2) {
                    aligned_lines.push_back(line_idx);
                }
            }
        }
        else {
            for (int line_idx = 0; line_idx < 9; ++line_idx) {
                const size_t line_count = std::count_if(final_swordfish_tiles.begin(), final_swordfish_tiles.end(), [&](const SudokuTile* obj) { return obj->Row == line_idx; });
                if (line_count == 1) {
                    stray_row = true;
                    break;
                }
                if (line_count >= 2) {
                    aligned_lines.push_back(line_idx);
                }
            }
        }

        if (stray_row || aligned_lines.size() != 3) {
            return;
        }

        bool success = false;
        for (auto& line_idx : aligned_lines) {
            success = (row_or_column ? sudoku_board.UpdateColumnPencilMarks(line_idx, bit_num, final_swordfish_tiles) : sudoku_board.UpdateRowPencilMarks(line_idx, bit_num, final_swordfish_tiles)) || success;
        }
        if (success) {
            ++count;
        }
    };

    for (int row = 0; row < 9; ++row) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            first_swordfish_tiles.clear();
            for (int col = 0; col < 9; ++col) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled() || sudoku_board.BoardTiles[row][col].Pencilmarks[bit_num]) {
                    continue;
                }

                first_swordfish_tiles.push_back(&sudoku_board.BoardTiles[row][col]);
            }

            if (first_swordfish_tiles.size() > 3 || first_swordfish_tiles.size() < 2) {
                continue;
            }

            auto find_other_swordfish_triples_row = [&sudoku_board, &final_swordfish_tiles, &other_swordfish_tiles, &bit_num](int& current_row, auto& self_function) {
                current_row++;
                if (current_row == 9) {
                    return false;
                }

                other_swordfish_tiles.clear();
                for (int col = 0; col < 9; ++col) {
                    if (!sudoku_board.BoardTiles[current_row][col].IsTileFilled() && !sudoku_board.BoardTiles[current_row][col].Pencilmarks[bit_num]) {
                        other_swordfish_tiles.push_back(&sudoku_board.BoardTiles[current_row][col]);
                    }
                }

                if (other_swordfish_tiles.size() > 3 || other_swordfish_tiles.size() < 2) {
                    return self_function(current_row, self_function);
                }

                final_swordfish_tiles.insert(final_swordfish_tiles.end(), other_swordfish_tiles.begin(), other_swordfish_tiles.end());
                return true;
            };

            for (int starting_row_1 = row; starting_row_1 < 9; ) {
                final_swordfish_tiles = first_swordfish_tiles;
                if (!find_other_swordfish_triples_row(starting_row_1, find_other_swordfish_triples_row)) {
                    break;
                }

                for (int starting_row_2 = starting_row_1; starting_row_2 < 9; ) {
                    if (!find_other_swordfish_triples_row(starting_row_2, find_other_swordfish_triples_row)) {
                        break;
                    }

                    final_swordfish_step(RowOrColumn_Row, bit_num);
                }
            }
        }
    }

    for (int col = 0; col < 9; ++col) {
        for (int bit_num = 0; bit_num < 9; ++bit_num) {
            first_swordfish_tiles.clear();
            for (int row = 0; row < 9; ++row) {
                if (sudoku_board.BoardTiles[row][col].IsTileFilled() || sudoku_board.BoardTiles[row][col].Pencilmarks[bit_num]) {
                    continue;
                }

                first_swordfish_tiles.push_back(&sudoku_board.BoardTiles[row][col]);
            }

            if (first_swordfish_tiles.size() > 3 || first_swordfish_tiles.size() < 2) {
                continue;
            }

            auto find_other_swordfish_triples_col = [&sudoku_board, &final_swordfish_tiles, &other_swordfish_tiles, &bit_num](int& current_col, auto& self_function) {
                current_col++;
                if (current_col == 9) {
                    return false;
                }

                other_swordfish_tiles.clear();
                for (int row = 0; row < 9; ++row) {
                    if (!sudoku_board.BoardTiles[row][current_col].IsTileFilled() && !sudoku_board.BoardTiles[row][current_col].Pencilmarks[bit_num]) {
                        other_swordfish_tiles.push_back(&sudoku_board.BoardTiles[row][current_col]);
                    }
                }

                if (other_swordfish_tiles.size() > 3 || other_swordfish_tiles.size() < 2) {
                    return self_function(current_col, self_function);
                }

                final_swordfish_tiles.insert(final_swordfish_tiles.end(), other_swordfish_tiles.begin(), other_swordfish_tiles.end());
                return true;
            };

            for (int starting_col_1 = col; starting_col_1 < 9; ) {
                final_swordfish_tiles = first_swordfish_tiles;
                if (!find_other_swordfish_triples_col(starting_col_1, find_other_swordfish_triples_col)) {
                    break;
                }

                for (int starting_col_2 = starting_col_1; starting_col_2 < 9; ) {
                    if (!find_other_swordfish_triples_col(starting_col_2, find_other_swordfish_triples_col)) {
                        break;
                    }

                    final_swordfish_step(RowOrColumn_Column, bit_num);
                }
            }
        }
    }

    return count;
}

}










