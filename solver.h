//
//  solver.h
//
//  Created by Saul Spatz on 9/17/16.
//  Copyright © 2016 Saul Spatz. All rights reserved.
//

#ifndef solver_h
#define solver_h

#include <iostream>
#include <algorithm> // for sort
#include "entry.h"

template <int N>
using SwissSquare = std::array<std::array<int, N>, N>;
template <int N>
using SwissSquares = std::vector<SwissSquare<N>>;

template <int N>
SwissSquare<N> holes2Square(const HoleSquare<N>& holes){
    SwissSquare<N> answer;
    const int DIM = (N-1)/2;
    for (int row = 0; row < DIM; ++ row)
    for (int col = 0; col < DIM; ++col) {
        auto hole = *holes[row][col];
        for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            answer[2*row+r][2*col+c] = hole.cells[r][c];
    }
    return answer;
}

template<int N>
std::ostream& operator <<(std::ostream& os, const SwissSquare<N>& square) {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++ col)
            os << square[row][col] << " ";
        os << std::endl;
    }
    return os;
}

template <int N>
struct Solver {
    // Solves Swiss Square puzzle of fixed dimension N
    
    static const int DIM = (N-1)/2;    // dimension of square of holes
    HoleList holes[8*N-6];             // maximum central sum is 8*N-7
    Givens<N> clues;
    std::array<Entry<N>,1+(N-1)*(N-1)/4> Stack;
    
    Solver();
    int makeHoles();
    Coords best(int level);
    void constrain(int level);
    SwissSquares<N> search(Givens<N>& hints);
};

template <int N>
Solver<N>::Solver() {
    makeHoles();
}

template <int N>
int Solver<N>::makeHoles() {
    int answer;        // number of holes found
    int cell[8];       // numbers assigned to the cells
    Set<N> S[9];       // possible values for each of the cells
    int level = 0;     // for backtracking
    S[0].set();
    S[0].reset(0);
    S[8].reset();     // sentinel
    while (level >= 0) {
        while (S[level].any()) {
            //advance
            int i = 0;
            while (S[level][i] == 0) ++i;
            cell[level] = i;
            S[level].reset(i);
            level += 1;
            if (level == 8) {
                answer += 1;
                auto hole = new Hole(cell);
                int s = hole->sum;
                holes[s].all.push_back(hole);
                holes[s].tops[hole->top].push_back(hole);
                holes[s].bottoms[hole->bottom].push_back(hole);
                holes[s].lefts[hole->left].push_back(hole);
                holes[s].rights[hole->right].push_back(hole);
                holes[s].count++;
            }
            else {
                // compute S[level]
                S[level].set();
                S[level].reset(0);
                switch (level) {
                    case 1:
                        S[1].reset(cell[0]);
                        break;
                    case 2:
                        S[2].reset(cell[0]);
                        S[2].reset(cell[1]);
                        break;
                    case 3:
                        S[3].reset(cell[2]);
                        break;
                    case 4:
                        S[4].reset(cell[2]);
                        S[4].reset(cell[3]);
                        break;
                    case 5:
                        S[5].reset(cell[1]);
                        S[5].reset(cell[4]);
                        break;
                    case 6:
                        S[6].reset(cell[0]);
                        S[6].reset(cell[4]);
                        S[6].reset(cell[5]);
                        break;
                    case 7:
                        S[7].reset(cell[0]);
                        S[7].reset(cell[3]);
                        S[7].reset(cell[6]);
                        break;
                }
            }
        }
        level -= 1;   //backtrack
    }
    return answer;
}

template <int N>
SwissSquares<N>
Solver<N>::search(Givens<N>& hints){
    clues = hints;
    SwissSquares<N> answer;
    int level = 1;
    int min = INFINITY;
    int sum =  0;
    for (int r = 0; r < DIM; ++r) {
        for (int c = 0; c < DIM; ++c) {
            sum = clues[r][c];
            if (holes[sum].count < min) {
                min = holes[sum].count;
                Stack[1].current.row = r;
                Stack[1].current.col = c;
            }
        }
    }
    Stack[1].current.candidates = holes[sum].all.begin();
    Stack[1].current.stop = holes[sum].all.end();
    while (level > 0) {
        auto & current = Stack[level].current;
        while (current.candidates < current.stop) {
            auto c = current.candidates;
            while (c < current.stop and not Stack[level].suitable(*c) )
                c++;
            if (c == current.stop ) break;   //backtrack
            current.choice = *c;
            current.candidates = c+1;
            if (level == DIM*DIM) {
                auto soln = Stack[level].inherit.filled;
                soln[current.row][current.col] = *c;
                answer.push_back(holes2Square<N>(soln));
                if (answer.size() == 2)
                    return answer;
            }
            else {
                level += 1;
                Stack[level] = Stack[level-1];
                auto & inherit = Stack[level].inherit;
                auto & old = Stack[level-1].current;
                auto last = old.choice;
                inherit.filled[old.row][old.col] = last;
                for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c) {
                    int value = last->cells[r][c];
                    if (value != 0){
                        inherit.usedRows[2*old.row+r].set(value);
                        inherit.usedCols[2*old.col+c].set(value);
                    }
                }
                constrain(level);
            }
        }
        level = level-1;
    }
    return answer;
}

template<int N>
Coords Solver<N>::best(int level) {
    /* Returns the coordinates of the next hole to fill at this level.
     This is the heart of the matter.
     Uses the following heuristic:
     1. Return the coordinates of a hole touching the greatest
     number of filled holes, provided it touches at least two.
     2. Return the cordinates of an empty hole in a maximally-
     filled row or column, provided there is more than one
     filled hole in that line.  The empty hole must touch
     a filled hole.
     3. Otherwise, among all empty holes adjacent to a filled
     hole, return the coordinates of the one where the
     combination of the clue and the constraint give the
     smallest number of possibilities.
     */
    Coords answer;
    
    auto & filled = Stack[level].inherit.filled;
    Givens<N> touches {};
    for (int r = 0; r< DIM; ++r)
        for (int c = 0; c < DIM; ++c) {
            if (not filled[r][c]) continue;
            if (r > 0 ) touches[r-1][c] +=1;
            if (r < DIM-1) touches[r+1][c] +=1;
            if (c > 0 ) touches[r][c-1] +=1;
            if (c < DIM-1) touches[r][c+1] +=1;
        }
    
    int max = 0;
    for (int r = 0; r < DIM; ++ r)
    for (int c = 0; c < DIM; c++) {
        if (not filled[r][c] and touches[r][c] > max) {
            max = touches[r][c];
            answer =  Coords(r,c);
        }
    }
    if ( max > 1) return answer;
    
    std::array<int, DIM> rowHoles {};
    std::array<int, DIM> colHoles {};
    
    for (int r = 0; r < DIM; ++r)
    for (int c = 0; c < DIM; ++c) {
        if (filled[r][c]) {
            rowHoles[r] += 1;
            colHoles[c] += 1;
        }
    }
    
    // Note: touches[r][c] = 0 for filled holes
    max = 0;
    for (int r = 0; r < DIM; ++r)
    for (int c = 0; c < DIM; ++ c) {
        if (not touches[r][c]) continue;
        if (rowHoles[r] > max) {
            max = rowHoles[r];
            answer = Coords(r,c);
        }
        if (colHoles[c] > max) {
            max = colHoles[c];
            answer = Coords(r,c);
        }
    }
    if (max > 1) return answer;
    
    size_t min {INFINITY};
    for (int r = 0; r < DIM; ++r)
    for (int c = 0; c < DIM; ++c) {
        if (not touches[r][c]) continue;
        int hint = clues[r][c];
        int top = r-1;

        int bottom = r+1;
        int left = c-1;
        int right = c+1;
        if (top >= 0 and filled[top][c]) {
            auto nbr = filled[top][c];
            size_t pop = holes[hint].tops[nbr->bottom].size();
            if (pop < min) {
                min = pop;
                answer = Coords(r,c);
            }
        }
        else if (bottom < DIM and filled[bottom][c]){
            auto nbr = filled[bottom][c];
            size_t pop = holes[hint].bottoms[nbr->top].size();
            if (pop < min) {
                min = pop;
                answer = Coords(r,c);
            }
        }
        else if (left >= 0 and filled[r][left]) {
            auto nbr = filled[r][left];
            size_t pop = holes[hint].lefts[nbr->right].size();
            if (pop < min) {
                min = pop;
                answer = Coords(r,c);
            }
        }
        else {
            auto nbr = filled[r][right];
            size_t pop = holes[hint].rights[nbr->left].size();
            if (pop < min) {
                min = pop;
                answer = Coords(r,c);
            }
        }
    }
    return answer;
}

template<int N>
void Solver<N>::constrain(int level){
    // Pre: inherit has been updated
    const int DIM = (N-1)/2;
    auto coords = best(level);
    auto & current = Stack[level].current;
    auto & filled = Stack[level].inherit.filled;
   
    int row = current.row = coords.first;
    int col = current.col = coords.second;
    int hint = clues[current.row][current.col];
    if (row > 0 and filled[row-1][col]) {
        // hole above is filled; its bottom is our top
        int v = filled[row-1][col]->bottom;
        auto c = holes[hint].tops[v].size();
        current.overlaps.push_back(Constraint('t',v,c));
    }
    if (row+1 < DIM and filled[row+1][col]) {
        // hole below is filled; its top is our bottom
        int v = filled[row+1][col]->top;
        auto c = holes[hint].bottoms[v].size();
        current.overlaps.push_back(Constraint('b', v, c));
    }
    if (col > 0 and filled[row][col-1]) {
        // hole to the left is filled; its right is our left
        int v = filled[row][col-1]->right;
        auto c = holes[hint].lefts[v].size();
        current.overlaps.push_back(Constraint('l',v,c));
    }
    if (col+1 < DIM and filled[row][col+1]) {
        // hole to the right is filled; its left is our right
        int v = filled[row][col+1]->left;
        auto c = holes[hint].rights[v].size();
        current.overlaps.push_back(Constraint('r',v,c));
    }
    std::sort(current.overlaps.begin(), current.overlaps.end(),
               [ ](Constraint& a, Constraint& b) {
                   return a.count > b.count;
               });
    auto least = current.overlaps.back();   // most restrictive constraint
    current.overlaps.pop_back();
    switch (least.side) {
        case 't':
            current.candidates = holes[hint].tops[least.value].begin();
            current.stop =       holes[hint].tops[least.value].end();
            break;
        case 'b':
            current.candidates = holes[hint].bottoms[least.value].begin();
            current.stop =       holes[hint].bottoms[least.value].end();
            break;
        case 'l':
            current.candidates = holes[hint].lefts[least.value].begin();
            current.stop =       holes[hint].lefts[least.value].end();
            break;
        case 'r':
            current.candidates = holes[hint].rights[least.value].begin();
            current.stop =       holes[hint].rights[least.value].end();
            break;
    }
    
}
#endif

