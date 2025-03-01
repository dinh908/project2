#include <iostream>
#include <cstdint>
#include <stack>
using namespace std;

const int BOARD_SIZE = 16;
const int BLOCK_SIZE = 4;

// Bit manipulation constants
const uint32_t IS_FILLED_IN = 0x80000000;
const uint32_t VALUE_MASK   = 0x0000000F;
const uint32_t IS_0_VALID   = 0x00000010;
const uint32_t VALID_BITS   = 0x000FFFF0;

uint32_t board[BOARD_SIZE][BOARD_SIZE];
stack<pair<int, int>> cellStack;

//Function read board from input
void readBoard() {
    cout << "[DEBUG] Reading board from input...\n";
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            char ch;
            cin >> ch;
            if (ch == '.')
                board[i][j] = VALID_BITS;
            else
                board[i][j] = ((isdigit(ch) ? ch - '0' : ch - 'a' + 10) & VALUE_MASK) | IS_FILLED_IN;
        }
    }
}

// DisplayDisplay board
void displayBoard() {
    cout << "[DEBUG] Current Board State:\n";
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] & IS_FILLED_IN) {
                int val = board[i][j] & VALUE_MASK;
                cout << (val < 10 ? char('0' + val) : char('a' + (val - 10)));
            } else {
                cout << ".";
            }
            if ((j + 1) % BLOCK_SIZE == 0 && j != BOARD_SIZE - 1)
                cout << " ";
        }
        cout << endl;
        if ((i + 1) % BLOCK_SIZE == 0 && i != BOARD_SIZE - 1)
            cout << endl;
    }
}

// Update valid address
void recomputeCandidates() {
    cout << "[DEBUG] Recomputing candidates...\n";

    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (!(board[i][j] & IS_FILLED_IN))
                board[i][j] = VALID_BITS;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] & IS_FILLED_IN) {
                int val = board[i][j] & VALUE_MASK;
                uint32_t mask = ~(IS_0_VALID << val);

                for (int k = 0; k < BOARD_SIZE; k++) {
                    if (!(board[i][k] & IS_FILLED_IN)) board[i][k] &= mask;
                    if (!(board[k][j] & IS_FILLED_IN)) board[k][j] &= mask;
                }

                int boxRow = (i / BLOCK_SIZE) * BLOCK_SIZE;
                int boxCol = (j / BLOCK_SIZE) * BLOCK_SIZE;
                for (int r = 0; r < BLOCK_SIZE; r++) {
                    for (int c = 0; c < BLOCK_SIZE; c++) {
                        if (!(board[boxRow + r][boxCol + c] & IS_FILLED_IN))
                            board[boxRow + r][boxCol + c] &= mask;
                    }
                }
            }
        }
    }
}

// Find best empty cell
bool findBestEmptyCell(int &bestRow, int &bestCol) {
    int minChoices = BOARD_SIZE + 1;
    bool found = false;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!(board[i][j] & IS_FILLED_IN)) {
                int choices = __builtin_popcount(board[i][j] & VALID_BITS);
                if (choices < minChoices && choices > 0) {
                    minChoices = choices;
                    bestRow = i;
                    bestCol = j;
                    found = true;
                }
            }
        }
    }
    
    if (found) {
        cout << "[DEBUG] Best empty cell: (" << bestRow << "," << bestCol << ") with " << minChoices << " choices.\n";
    }
    
    return found;
}

// Solver
bool solveSudoku() {
    int bestRow, bestCol;
    if (!findBestEmptyCell(bestRow, bestCol)) return true;

    cellStack.push({bestRow, bestCol});

    while (!cellStack.empty()) {
        auto [row, col] = cellStack.top();
        cellStack.pop();

        if (!(board[row][col] & IS_FILLED_IN)) {
            cout << "[DEBUG] Trying values for cell: (" << row << "," << col << ")\n";

            bool placed = false;
            
            for (int num = 0; num < BOARD_SIZE; num++) {
                if (board[row][col] & (IS_0_VALID << num)) {
                    board[row][col] = (num & VALUE_MASK) | IS_FILLED_IN;
                    recomputeCandidates();
                    if (findBestEmptyCell(bestRow, bestCol)) {
                        cellStack.push({row, col});
                        cellStack.push({bestRow, bestCol});
                        placed = true;
                        break;
                    } else {
                        cout << "[DEBUG] Solution found!\n";
                        return true;
                    }
                }
            }

            if (!placed) {
                cout << "[DEBUG] Backtracking on cell: (" << row << "," << col << ") - Resetting choices\n";
                board[row][col] = VALID_BITS;
                recomputeCandidates();
            }
        }
    }

    return false;
}

int main() {
    readBoard();
    displayBoard();

    if (solveSudoku()) {
        cout << "\n[SUCCESS] Solved Sudoku:\n";
        displayBoard();
    } else {
        cout << "\n[FAIL] No solution found\n";
    }

    return 0;
}

