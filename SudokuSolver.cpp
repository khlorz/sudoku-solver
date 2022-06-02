#include "SudokuSolver.h"

// Functions for querying the sudoku board rows/columns/cells
namespace sdq
{

constexpr uint16_t GetNextRow(uint16_t row, uint16_t col) noexcept
{
    return row + (col + 1) / 9;
}

constexpr uint16_t GetNextCol(uint16_t col) noexcept
{
    return (col + 1) % 9;
}

constexpr uint16_t GetCellBlock(uint16_t row, uint16_t col) noexcept
{
    return (row / 3) * 3 + col / 3;
}

constexpr std::pair<uint16_t, uint16_t> NextEmptyPosition(const SudokuBoard& board, uint16_t row, uint16_t col) noexcept
{
    while (row != 9) {
        if (board[row][col] == '0') {
            return { row, col };
        }
        row = sdq::GetNextRow(row, col);
        col = sdq::GetNextCol(col);
    }

    return { 9, 0 };
}

}



//--------------------------------------------------------------------------------------------------------------------------------
// SudokuBoardLogic CLASS
//--------------------------------------------------------------------------------------------------------------------------------

SudokuBoardLogic::SudokuBoardLogic() :
    RowOccurences({ 0, 0, 0, 0, 0, 0, 0, 0, 0 }),
    ColOccurences({ 0, 0, 0, 0, 0, 0, 0, 0, 0 }),
    CellOccurences({ 0, 0, 0, 0, 0, 0, 0, 0, 0 })
{}

constexpr bool SudokuBoardLogic::IsEmpty() noexcept
{
    size_t total_count = 0;
    for (size_t idx = 0; idx < 9; ++idx) {
        total_count += RowOccurences[idx].count() + ColOccurences[idx].count() + CellOccurences[idx].count();
    }

    return total_count == 0;
}

void SudokuBoardLogic::ResetAll()
{
    RowOccurences = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ColOccurences = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CellOccurences = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
}

void SudokuBoardLogic::SetCellNumber(uint16_t row, uint16_t col, uint16_t number) noexcept
{
    RowOccurences[row].set(number);
    ColOccurences[col].set(number);
    const auto& cellblock = sdq::GetCellBlock(row, col);
    CellOccurences[cellblock].set(number);
}

std::bitset<9>& SudokuBoardLogic::GetRowOccurences(uint16_t row) noexcept
{
    return RowOccurences[row];
}

std::bitset<9>& SudokuBoardLogic::GetColumnOccurences(uint16_t col) noexcept
{
    return ColOccurences[col];
}

std::bitset<9>& SudokuBoardLogic::GetCellOccurences(uint16_t cell) noexcept
{
    return CellOccurences[cell];
}

std::bitset<9> SudokuBoardLogic::GetTileOccurences(uint16_t row, uint16_t col) const noexcept
{
    const auto& cell = sdq::GetCellBlock(row, col);
    return RowOccurences[row] | ColOccurences[col] | CellOccurences[cell];
}

void SudokuBoardLogic::ResetCellNumber(uint16_t row, uint16_t col, uint16_t number) noexcept
{
    RowOccurences[row].reset(number);
    ColOccurences[col].reset(number);
    const auto& cell = sdq::GetCellBlock(row, col);
    CellOccurences[cell].reset(number);
}



//--------------------------------------------------------------------------------------------------------------------------------
// VecMRV CLASS
//--------------------------------------------------------------------------------------------------------------------------------

constexpr BoardMRV::BoardMRV(SudokuBoard& board, SudokuBoardLogic& board_logic)
{
    TheMRVs.reserve(64);
    CreateMRV(board, board_logic);
}

constexpr void BoardMRV::CreateMRV(SudokuBoard& board, SudokuBoardLogic& board_logic) noexcept
{
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            if (board[row][col] != '0') {
                continue;
            }
            
            auto* row_occur  = &board_logic.GetRowOccurences(row);
            auto* col_occur  = &board_logic.GetColumnOccurences(col);
            auto* cell_occur = &board_logic.GetCellOccurences(sdq::GetCellBlock(row, col));
            TheMRVs.push_back(TileMRV(row, col, row_occur, col_occur, cell_occur));
        }
    }
}

constexpr std::tuple<uint16_t, uint16_t, uint16_t> BoardMRV::GetHighestTileMRV() noexcept
{
    size_t highest_mrv = 0;
    uint16_t highest_mrv_index = 0;
    for (uint16_t idx = 0; idx < TheMRVs.size(); ++idx) {
        if (TheMRVs[idx].Filled) {
            continue;
        }
        
        const auto& current_mrv_sum = TheMRVs[idx].CellOccurence->count() + TheMRVs[idx].ColOccurence->count() + TheMRVs[idx].RowOccurence->count();
        if (highest_mrv < current_mrv_sum) {
            highest_mrv  = current_mrv_sum;
            highest_mrv_index = idx;
        }
    }

    if (highest_mrv == 0) {
        return { 9, 0, 0 };
    }

    return { TheMRVs[highest_mrv_index].Row, TheMRVs[highest_mrv_index].Column, highest_mrv_index };
}

std::bitset<9> BoardMRV::GetOccurences(size_t mrv_index) noexcept
{
    return *TheMRVs[mrv_index].RowOccurence | *TheMRVs[mrv_index].ColOccurence | *TheMRVs[mrv_index].CellOccurence;
}

void BoardMRV::SetFilledStatus(size_t mrv_index, uint16_t number, bool filled)
{
    TheMRVs[mrv_index].Filled = filled;

    if (filled) { TheMRVs[mrv_index].RowOccurence->set(number);   TheMRVs[mrv_index].ColOccurence->set(number);   TheMRVs[mrv_index].CellOccurence->set(number);   }
    else        { TheMRVs[mrv_index].RowOccurence->reset(number); TheMRVs[mrv_index].ColOccurence->reset(number); TheMRVs[mrv_index].CellOccurence->reset(number); }
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

bool Solve(std::array<std::array<char, 9>, 9>& board, SudokuBoardLogic* board_logic, SolveMethod method, bool check_the_solutions) noexcept
{
    // For when there is no need for the board_logic output
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        return Solve(board, &new_board_logic, method, check_the_solutions);
    }

    // For when the board_logic is empty
    if (board_logic->IsEmpty()) {
        if (!CreateBoardLogic(board, *board_logic)) {
            return false;
        }
    }

    if (check_the_solutions) {
        size_t number_of_solutions = 0;
        CountSolutions(board, *board_logic, number_of_solutions, 0, 0);
        if (number_of_solutions > 1) {
            return false;
        }
    }

    switch (method)
    {
    case SolveMethod_BruteForce:
        return SolveBruteForce(board, board_logic);
    case SolveMethod_MRV:
        return SolveMRV(board, board_logic);
    default:
        return false;
    }
}

bool Solve2(std::array<char, 81>& board, SudokuBoardLogic* board_logic, SolveMethod method) noexcept
{
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        return Solve2(board, &new_board_logic, method);
    }

    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num];
            tile_num++;
        }
    }

    if (!Solve(converted_board, board_logic, method)) {
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

bool Solve3(std::vector<char>& board, SudokuBoardLogic* board_logic, SolveMethod method) noexcept
{
    if (board.size() != 81) {
        return false;
    }

    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        return Solve3(board, &new_board_logic, method);
    }

    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num];
            tile_num++;
        }
    }

    if (!Solve(converted_board, board_logic, method)) {
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

bool Solve4(std::array<std::array<int, 9>, 9>& board, SudokuBoardLogic* board_logic, SolveMethod method) noexcept
{
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        return Solve4(board, &new_board_logic, method);
    }

    std::array<std::array<char, 9>, 9> converted_board;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[row][col] + '0';
        }
    }

    if (!Solve(converted_board, board_logic, method)) {
        return false;
    }

    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            board[row][col] = converted_board[row][col] - '0';
        }
    }

    return true;
}

bool Solve5(std::vector<int>& board, SudokuBoardLogic* board_logic, SolveMethod method) noexcept
{
    if (board.size() != 81) {
        return false;
    }

    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        return Solve5(board, &new_board_logic, method);
    }

    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num] + '0';
            tile_num++;
        }
    }

    if (!Solve(converted_board, board_logic, method)) {
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

bool Solve6(std::array<int, 81>& board, SudokuBoardLogic* board_logic, SolveMethod method) noexcept
{
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        return Solve6(board, &new_board_logic, method);
    }

    std::array<std::array<char, 9>, 9> converted_board;
    size_t tile_num = 0;
    for (size_t row = 0; row < 9; ++row) {
        for (size_t col = 0; col < 9; ++col) {
            converted_board[row][col] = board[tile_num] + '0';
            ++tile_num;
        }
    }

    if (!Solve(converted_board, board_logic, method)) {
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

constexpr bool SolveBruteForceEX(SudokuBoard& board, SudokuBoardLogic& board_logic, const uint16_t row_start, const uint16_t col_start) noexcept
{
    const auto& [row, col] = sdq::NextEmptyPosition(board, row_start, col_start);

    // end of board 
    if (row == 9) {
        return true;
    }

    const std::bitset<9>& occurences = board_logic.GetTileOccurences(row, col);
    if (occurences.all()) {
        return false;
    }

    for (uint16_t digit_idx = 0; digit_idx < 9; ++digit_idx) {
        if (occurences[digit_idx]) {
            continue;
        }

        board[row][col] = static_cast<char>(digit_idx + '1');
        board_logic.SetCellNumber(row, col, digit_idx);
        if (SolveBruteForceEX(board, board_logic, row, col)) {
            return true;
        }
        board_logic.ResetCellNumber(row, col, digit_idx);
    }

    board[row][col] = '0';
    return false;
}

bool SolveBruteForce(SudokuBoard& board, SudokuBoardLogic* board_logic) noexcept
{
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        if (!CreateBoardLogic(board, new_board_logic)) {
            return false;
        }

        return SolveBruteForceEX(board, new_board_logic, 0, 0);
    }

    return SolveBruteForceEX(board, *board_logic, 0, 0);
}

constexpr bool SolveMRVEX(SudokuBoard& board, BoardMRV& board_mrvs) noexcept
{
    auto [row, col, mrv_index] = board_mrvs.GetHighestTileMRV();

    if (row == 9) {
        return true;
    }

    const std::bitset<9>& occurences = board_mrvs.GetOccurences(mrv_index);
    if (occurences.all()) {
        return false;
    }

    for (uint16_t digit_idx = 0; digit_idx < 9; ++digit_idx) {
        if (occurences[digit_idx]) {
            continue;
        }

        board[row][col] = static_cast<char>(digit_idx + '1');
        board_mrvs.SetFilledStatus(mrv_index, digit_idx, true);
        if (SolveMRVEX(board, board_mrvs)) {
            return true;
        }
        board_mrvs.SetFilledStatus(mrv_index, digit_idx, false);
    }

    board[row][col] = '0';
    return false;
}

bool SolveMRV(SudokuBoard& board, SudokuBoardLogic* board_logic) noexcept
{
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        if (!CreateBoardLogic(board, new_board_logic)) {
            return false;
        }

        return SolveMRV(board, &new_board_logic);
    }

    BoardMRV board_mrv(board, *board_logic);
    return SolveMRVEX(board, board_mrv);
}

//----------------------------------------------------------------------------------------------------------------------------------------------
// Utility functions for the sudoku solvers
// - Functions that helps sudoku solvers and its other works
//----------------------------------------------------------------------------------------------------------------------------------------------

bool FillSudoku(SudokuBoard& board, SudokuBoardLogic& board_logic, const std::array<uint16_t, 9>& random_numbers, uint16_t const row_start, uint16_t const col_start) noexcept
{
    const auto& [row, col] = sdq::NextEmptyPosition(board, row_start, col_start);

    if (row == 9) {
        return true;
    }

    const std::bitset<9>& occurences = board_logic.GetTileOccurences(row, col);
    if (occurences.all()) {
        return false;
    }

    for (uint16_t idx = 0; idx < 9; ++idx) {
        auto digit = random_numbers[idx];
        if (occurences[digit]) {
            continue;
        }

        board[row][col] = static_cast<char>(digit + '1');
        board_logic.SetCellNumber(row, col, digit);
        if (FillSudoku(board, board_logic, random_numbers, row, col)) {
            return true;
        }
        board_logic.ResetCellNumber(row, col, digit);
    }

    board[row][col] = '0';
    return false;
}

bool IsUniqueBoard(SudokuBoard& board, SudokuBoardLogic* board_logic) noexcept
{
    if (board_logic == nullptr) {
        SudokuBoardLogic new_board_logic;
        if (!CreateBoardLogic(board, new_board_logic)) {
            printf("INVALID BOARD!");
            return false;
        }

        return IsUniqueBoard(board, &new_board_logic);
    }

    size_t number_of_solutions = 0;
    CountSolutions(board, *board_logic, number_of_solutions, 0, 0);
    return number_of_solutions == 1;
}

void CountSolutions(SudokuBoard& board, SudokuBoardLogic& board_logic, size_t& number_of_solutions, uint16_t const row_start, uint16_t const col_start) noexcept
{
    const auto& [row, col] = sdq::NextEmptyPosition(board, row_start, col_start);

    // end of board 
    if (row == 9) {
        ++number_of_solutions;
        return;
    }

    std::bitset<9> const occurences = board_logic.GetTileOccurences(row, col);
    //if (occurences.all()) {
    //    return;
    //}

    for (uint16_t digit_idx = 0; digit_idx < 9 && number_of_solutions < 2; ++digit_idx) {
        if (occurences[digit_idx]) {
            continue;
        }

        board[row][col] = static_cast<char>(digit_idx + '1');
        board_logic.SetCellNumber(row, col, digit_idx);
        CountSolutions(board, board_logic, number_of_solutions, row, col);
        board_logic.ResetCellNumber(row, col, digit_idx);
    }

    board[row][col] = '0';
}

bool CreateBoardLogic(SudokuBoard& board, SudokuBoardLogic& board_logic) noexcept
{
    board_logic.ResetAll();

    for (uint16_t row = 0; row < 9; ++row) {
        for (uint16_t col = 0; col < 9; ++col) {
            if (board[row][col] < '0' || board[row][col] > '9') {
                return false;
            }
            char digit;
            if ((digit = board[row][col]) != '0') {
                uint16_t digit_num = digit - '1';
                if (board_logic.GetTileOccurences(row, col)[digit_num]) {
                    return false;
                }
                board_logic.SetCellNumber(row, col, digit_num);
            }
        }
    }

    return true;
}

void ClearSudokuBoard(SudokuBoard& board) noexcept
{
    board.fill(std::array<char, 9>({ '0', '0', '0', '0', '0', '0', '0', '0', '0' }));
}

SudokuDifficulty CheckSudokuDifficulty(const SudokuBoard& solution_board, const SudokuBoard& puzzle_board, const SudokuBoardLogic& puzzle_logic) noexcept
{
    size_t           difficulty_score      = 0;
    size_t           number_of_empty_tiles = 0;
    SudokuBoard      temp_board            = puzzle_board;
    SudokuBoardLogic temp_logic            = puzzle_logic;
    BoardMRV puzzle_mrv(temp_board, temp_logic);
    do
    {
        const auto [row, col, mrv_index] = puzzle_mrv.GetHighestTileMRV();
        if (row == 9) {
            break;
        }

        const auto& occurences = puzzle_mrv.GetOccurences(mrv_index);
        difficulty_score += (9 - occurences.count()) * (9 - occurences.count());
        temp_board[row][col] = solution_board[row][col];
        puzzle_mrv.SetFilledStatus(mrv_index, temp_board[row][col] - '1', true);
        ++number_of_empty_tiles;

    } while (true);

    difficulty_score += difficulty_score * number_of_empty_tiles;
    
    if (difficulty_score <= 3500)
        return SudokuDifficulty_Easy;

    if (difficulty_score > 3500 && difficulty_score <= 5500)
        return SudokuDifficulty_Normal;

    if (difficulty_score > 5500 && difficulty_score <= 7250)
        return SudokuDifficulty_Hard;
    
    return SudokuDifficulty_ChadBrain;
}

}










