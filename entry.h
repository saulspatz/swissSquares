//
//  entry.h
//
//  Created by Saul Spatz on 9/17/16.
//  Copyright © 2016 Saul Spatz. All rights reserved.
//

#ifndef entry_h
#define entry_h
#include "hole.h"

/*
Entry represents a bookkeepping entry for backtrack search.
The data members are of two kinds, "inherited" and "current."
The inherited members are aggregated from the Entry at the
previous level, and the current members are those determined
at the present level.
*/

template <int N>
using HoleSquare = std::array<std::array<Hole*, (N-1)/2>, (N-1)/2>;
template<int N>
using CellSquare = std::array<std::array<int, N>,N>;

template<int N>
struct Entry {
    static const int DIM = (N-1)/2;
    
    // These members are copied from preceding level, then updated
    // Note that filled will reflect the choice made at this level,
    // cells only reflects choices at prior levels.
    
    //These memebrs are copied from the previous level and updated
    
    HoleSquare<N> filled {};      // holes that have been filled in
    CellSquare<N> cells {};       // cells that have been filled in
    
    // These members are initiated at every level
    int row;
    int col;
    
    std::vector<Constraint> overlaps;
    std::vector<Hole*>::iterator candidates;
    std::vector<Hole*>::iterator stop;

    Entry() = default;
    bool suitable (Hole* hole);
    void updateCells(const Entry& previous);
};

template<int N>
bool Entry<N>::suitable (Hole* hole) {
    // Is hole suitable at this level?
    // Must satify all constraints and not violate "Latinity"
    for (auto constraint : overlaps)
        if (not constraint.match(hole))
            return false;
    // To test Latinity, simulate filling in the square
    Set<N> used;
    CellSquare<N> test(cells);
    int rowBase = 2*row;
    int colBase = 2*col;
    for (int r = 0; r < 3; ++r)
    for (int c = 0; c< 3; ++c) {
        auto value = hole->cells[r][c];
        test[rowBase+r][colBase+c] = value;
    }
    for (int r = 0; r < 3; ++r) {
        used.reset();
        for (int c = 0; c < N; ++c){
            int value = test[rowBase+r][c];
            if (value == 0) continue;
            if (used[value]) return false;
            used.set(value);
        }
    }
    for (int c = 0; c < 3; ++c) {
        used.reset();
        for (int r = 0; r < N; ++r){
            int value = test[r][colBase+c];
            if (value == 0) continue;
            if (used[value]) return false;
            used.set(value);
        }
    }
    return true;
}

template<int N>
void Entry<N>::updateCells(const Entry& previous) {
    int baseRow = 2*previous.row;
    int baseCol = 2*previous.col;
    Hole& hole = *filled[row][col];
    for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 3; ++c){
        int value = hole.cells[r][c];
        if (value != 0)
            cells[baseRow+r][baseCol+c] = value;
    }
}

#endif

