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
struct Inherited {
    HoleSquare<N> filled;                  // holes that have been filled in
    std::array<Set<N>, N> usedRows;        // precluded values for rows
    std::array<Set<N>, N> usedCols;        // precluded values for columns
    Inherited() : filled{}, usedRows{}, usedCols{} {}
};

template<int N>
struct Current {
    int row; // row and column filled at this level
    int col;
    std::vector<Constraint> overlaps;
    std::vector<Hole*>::iterator candidates;
    std::vector<Hole*>::iterator stop;
    Hole* choice; // hole used at this level
    Current() : row{}, col{}, overlaps{}, candidates(), stop(),choice() {
        //std::cout << "Current ctor " << overlaps.size() << std::endl;
    }
};

template<int N>
struct Entry {
    static const int DIM = (N-1)/2;
    Inherited<N> inherit;
    Current<N> current;
    
    Entry() : inherit(), current() {}
    Entry(const Entry& e);
    bool suitable (Hole* hole);
};

template<int N>
bool Entry<N>::suitable (Hole* hole) {
    // Is hole suitable at this level?
    // Must satify all constraints and not violate "Latinity"
    for (auto constraint : current.overlaps)
        if (not constraint.match(hole))
            return false;
    int row = 2*current.row;
    int col = 2*current.col;
    auto & usedRows = inherit.usedRows;
    auto & usedCols = inherit.usedCols;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c< 3; ++c) {
            auto value = hole->cells[r][c];
            if (usedRows[row+r][value] or usedCols[col+c][value])
                return false;
        }
    }
    return true;
}

template<int N>
Entry<N>::Entry(const Entry& e) : inherit(e.inherit), current()  {
    const Current<N>& old = e.current;
    inherit.filled[old.row][old.col] = old.choice;
    for (int r = 0; r < 3; ++r)
    for (int c = 0; c < 3; ++c){
        int value = old.choice->cells[r][c];
        if (value != 0) {
            inherit.usedRows[2*old.row+r].set(value);
            inherit.usedCols[2*old.col+c].set(value);
        }
    }
}

#endif

