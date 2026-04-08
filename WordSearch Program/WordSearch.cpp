#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution
{
public:
    bool dfs(vector<vector<char>>& board, string& word, int i, int j, int index)
    {
        // all characters matched
        if(index == word.size()) return true;

        // bounds + mismatch check
        if(i < 0 || i >= board.size() || j < 0 || j >= board[0].size() || board[i][j] != word[index])
            return false;

        // mark visited
        char temp = board[i][j];
        board[i][j] = '#';

        // 8 possible directions (horizontal, vertical, diagonal), the 8 directions are: 
        // down, up, right, left, diagonal down-right, diagonal down-left, diagonal up-right, diagonal up-left
        int dirs[8][2] = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},   // down, up, right, left
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // diagonals
        };

        bool found = false;

        // All 8 directions
        for(int d = 0; d < 8; d++)
        {
            int newRow = i + dirs[d][0];
            int newCol = j + dirs[d][1];

            // Recursively search in the chosen direction
            if(dfs(board, word, newRow, newCol, index + 1))
            {
                found = true;
                break; // stop early if found
            }
        }

        // backtrack
        board[i][j] = temp;

        return found;
    }

    bool exist(vector<vector<char>>& board, string word)
    {
        // Check each cell as a potential starting point
        for(int i = 0; i < board.size(); i++)
        {
            // Check each column in the current row
            for(int j = 0; j < board[0].size(); j++)
            {
                // Check if the current cell matches the first character of the word
                if(dfs(board, word, i, j, 0))
                {
                    return true;
                }
            }
        }
        return false;
    }
};

int main()
{
    vector<vector<char>> board = {
        {'A','B','C','E'},
        {'S','F','E','S'},
        {'A','D','E','E'}
    };

    string word = "BEE";    // Test word

    Solution sol;
    bool result = sol.exist(board, word);

    cout << "Word to search: " << word << endl;
    cout << "Does the word exist in the board? " 
         << (result ? "Yes" : "No") << endl;
    return 0;
}