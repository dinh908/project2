#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

// Constants for bit manipulation
const uint32_t IS_FILLED_IN = 0x80000000;
const uint32_t VALUE_MASK  = 0x0000000F;
const uint32_t IS_0_VALID  = 0x00000010;
const uint32_t VALID_BITS  = 0x000FFFF0;

// Sudoku grid
uint32_t board[16][16];

// Stack for backtracking
vector<int> cellStack;

// Function to read the board
void readBoard() {
    for (int i = 0; i < 16; i++) {
        string row;
        cin >> row;
        for (int j = 0; j < 16; j++) {
            if (row[j] == '.') {
                board[i][j] = VALID_BITS; // All numbers initially valid
            } else {
                uint32_t val = (row[j] >= '0' && row[j] <= '9') ? row[j] - '0' : row[j] - 'a' + 10;
                board[i][j] = IS_FILLED_IN | val;
            }
        }
    }
}

// Function to print the solved board
void printBoard() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            uint32_t val = board[i][j] & VALUE_MASK;
            cout << (val < 10 ? char('0' + val) : char('a' + val - 10)) << " ";
        }
        cout << endl;
    }
}

// Function to update valid choices in row, column, and 4x4 block
void updateValidChoices(int row, int col, uint32_t val) {
    uint32_t mask = ~(IS_0_VALID << val);

    // Remove from row and column
    for (int i = 0; i < 16; i++) {
        if (!(board[row][i] & IS_FILLED_IN)) board[row][i] &= mask;
        if (!(board[i][col] & IS_FILLED_IN)) board[i][col] &= mask;
    }

    // Remove from 4x4 block
    int blockStartRow = row - row % 4;
    int blockStartCol = col - col % 4;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (!(board[blockStartRow + i][blockStartCol + j] & IS_FILLED_IN))
                board[blockStartRow + i][blockStartCol + j] &= mask;
}

// Function to find the best empty cell (fewest valid choices)
bool findBestEmptyCell(int &bestRow, int &bestCol) {
    int minChoices = 17;

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (board[i][j] & IS_FILLED_IN) continue;

            int count = 0;
            for (int k = 0; k < 16; k++)
                if (board[i][j] & (IS_0_VALID << k))
                    count++;

            if (count < minChoices) {
                minChoices = count;
                bestRow = i;
                bestCol = j;
            }
        }
    }

    if (minChoices == 17) return false; // No empty cells left
    board[bestRow][bestCol] |= IS_FILLED_IN;
    cellStack.push_back(bestRow * 16 + bestCol);
    return true;
}

// Function to solve the Sudoku using backtracking
void solveSudoku() {
    int row, col;

    while (true) {
        if (cellStack.empty()) {
            cout << "No solution found!\n";
            return;
        }

        int val = cellStack.back();
        row = val / 16;
        col = val % 16;

        // Try the next valid choice
        bool found = false;
        for (int choice = 0; choice < 16; choice++) {
            if (board[row][col] & (IS_0_VALID << choice)) {
                board[row][col] = (board[row][col] & ~VALUE_MASK) | choice;
                updateValidChoices(row, col, choice);
                found = true;
                break;
            }
        }

        if (!found) {
            // If no valid choice, backtrack
            board[row][col] &= ~IS_FILLED_IN;
            cellStack.pop_back();
        } else {
            // Find the next best cell
            if (!findBestEmptyCell(row, col)) break;
        }
    }
}

// Main function
int main() {
    readBoard();
    solveSudoku();
    printBoard();
    return 0;
}
