//
//  swiss.cpp
//  swissCells
//
//  Created by Saul Spatz on 9/6/16.
//  Copyright © 2016 Saul Spatz. All rights reserved.
//

// Compute 3-by-3 holes with whole in the middle (like Swiss cheese) so
// that the numbers in each row and column are distinct.  Numbers go from
// 1 to MAX.

// Inspired by https://www.brainbashers.com/showpuzzles.asp?puzzle=ZIWB

#include <iostream>
#include <fstream>
#include "solver.h"

using std::cout;
using std::endl;
using std::array;
using std::vector;

int main() {
    Givens<7> clues {26,35,37,31,36,39,38,26,24};
    Solver<7> solver;
    auto answer = solver.search(clues);
    switch(answer.size()) {
        case 0:
            cout << "No solution" << endl;
            break;
        case 1:
            cout << "Unique solution" << endl;
            break;
        default:
            cout << "Multiple solutions" << endl;
            break;
    }
    for (auto & soln:answer) {
        for (auto & row:soln) {
            for (auto col:row)
                cout << col << " ";
            cout << endl;
        }
        cout << endl;
    }
    return 0;
}
