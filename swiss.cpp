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
#include <fstream>
#include "solver.h"
#include "randomLatin.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ostream;
using std::array;
using std::vector;
using std::exit;
using std::stoi;

template <int N>
void printClues(const Givens<N>& clues, ostream& out) {
    for ( auto & row: clues) {
        for ( auto & clue :row)
            out << clue << " ";
        out << endl;
    }
    out << endl;
}

template <int N>
void printSolution(const SwissSquare<N>& soln, ostream& out) {
    for (auto & row:soln) {
        for (auto col:row)
            out << col << " ";
        out << endl;
    }
    out << endl  << endl;
}

template<int N>
Givens<N> square2Clues(const SwissSquare<N>& soln) {
    Givens<N> answer {};
    const int DIM = (N-1)/2;
    
    for (int r = 0; r < DIM; ++r)
    for (int c = 0; c < DIM; ++c) {
        int rowCenter = 2*r+1;
        int colCenter = 2*c+1;
        for (int i=-1; i <=1; ++i)
        for (int j=-1; j <= 1; ++j)
            answer[r][c] += soln[rowCenter+i][colCenter+j];
        answer[r][c] -= soln[rowCenter][colCenter];
    }
    return answer;
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
    if ( argc != 3) {
        cerr << "Usage " << argv[0] << " trials outfile" << endl;
        exit(1);
    }
    const int N = 7;
    int trials = stoi(argv[1]);
    ofstream fout;
    
    fout.open(argv[2], std::fstream::app);

    if (not fout) {
        cerr << "Could not open " << argv[2] << " for output."  << endl;
        exit(1);
    }
    
    Solver<N> solver;
    LatinGenerator<N> gen;
    Givens<N> clues;
    int success = 0;
    int errors = 0;
    for (int trial = 0; trial < trials; ++trial) {
        cout << '\r' << trial+1 << std::flush;
        clues = square2Clues<N>(gen.next());
        auto answer = solver.search(clues);
        if (answer.size()!=1) continue;
        success += 1;
        printClues<N>(clues, fout);
        auto &soln(answer[0]);
        if (not audit<N>(soln, clues)) {
            fout << "INCORRECT SOLUTION FOLLOWS"<<endl;
            errors++;
        }
        printSolution<N>(soln, fout);
    }
    cout << endl;
    cout << success << " successes in " << trials << " trials." << endl;
    cout << errors << " errors encountered." << endl;
    return 0;
}
