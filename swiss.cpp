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
#include <string>
#include "solver.h"
#include "clueGenerator.h"

using std::cout;
using std::endl;
using std::array;
using std::vector;
using std::exit;
using std::stoi;

template <int N>
void printClues(const Givens<N>& clues) {
    for ( auto & row: clues) {
        for ( auto & clue :row)
            cout << clue << " ";
        cout << endl;
    }
    cout << endl;
}

template <int N>
void printSolution(const SwissSquare<N>& soln) {
    for (auto & row:soln) {
        for (auto col:row)
            cout << col << " ";
        cout << endl;
    }
    cout << endl  << endl;
}

template <int N>
bool audit(const SwissSquare<N>& soln, const Givens<N> & clues) {
    const int DIM = (N-1)/2;
    Set<N> used;
    for (int row = 0; row < N; ++row) {
        used.reset();
        for (int col = 0; col < N; ++col) {
            int value = soln[row][col];
            if (value==0) continue;
            if (used[value]) return false;
            used.set(value);
        }
    }
    for (int col = 0; col < N; ++col) {
        used.reset();
        for (int row = 0; row < N; ++row) {
            int value = soln[row][col];
            if (value==0) continue;
            if (used[value]) return false;
            used.set(value);
        }
    }
    for (int r = 0; r < DIM; ++r)
        for (int c = 0; c < DIM; ++c){
            int row = 2*r;
            int col = 2*c;
            int hint = clues[r][c];
            int sum = 0;
            for (int r2 = 0; r2 < 3; ++r2)
                for (int c2 = 0; c2 < 3; c2++)
                    sum += soln[row+r2][col+c2];
            if (sum != hint) return false;
        }
    return true;
}

int main(int argc, char **argv) {
    if ( argc != 2) {
        cout << "Usage " << argv[0] << " trials" << endl;
        exit(1);
    }
    const int N = 7;
    int trials = stoi(argv[1]);
    
    Solver<N> solver;
    ClueGenerator<N> gen;
    Givens<N> clues;
    int success = 0;
    int multiple = 0;
    int errors = 0;
    for (int trial = 0; trial < trials; ++trial) {
        cout << trial+1 << " ";
        clues = gen.next();
        auto answer = solver.search(clues);
        if (solver.count == 0) {
            printClues<N>(clues);
            exit(0);
        }
        switch(answer.size()) {
            case 0:
                cout << "No solution " << solver.count << " holes" << endl;
                continue;
            case 1:
                cout << "Unique solution " << solver.count << " holes" << endl;
                break;
            default:
                cout << "Multiple solutions " << solver.count << " holes" << endl;
                multiple += 1;
                continue;
        }
        success += 1;
        printClues<N>(clues);
        
        auto &soln(answer[0]);
        if (not audit<N>(soln, clues)) {
            cout << "INCORRECT SOLUTION FOLLOWS"<<endl;
            errors++;
        }
        printSolution<N>(soln);
    }
    cout << success << " successes in " << trials << " trials." << endl;
    cout << multiple << " multiple solutions." << endl;
    cout << errors << " errors encountered." << endl;
    return 0;
}
