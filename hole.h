//
//  hole.h
//  Latin3
//
//  Created by Saul Spatz on 9/7/16.
//  Copyright © 2016 Saul Spatz. All rights reserved.
//

#ifndef hole_h
#define hole_h

#include <vector>
#include <array>
#include "alias.h"

struct Hole{
    int top;    // 3-digit integer in base 10
    int bottom;
    int left;
    int right;
    int sum;
    int cells[3][3];
    Hole(int c[]) {
        top    = 100*c[0] + 10*c[1] + c[2];
        right  = 100*c[2] + 10*c[3] + c[4];
        left   = 100*c[0] + 10*c[7] + c[6];
        bottom = 100*c[6] + 10*c[5] + c[4];
        sum = 0;
        for ( int i = 0; i < 8; ++ i)
            sum += c[i];
        cells[0][0] = c[0];
        cells[0][1] = c[1];
        cells[0][2] = c[2];
        cells[1][0] = c[7];
        cells[1][1] = 0;
        cells[1][2] = c[3];
        cells[2][0] = c[6];
        cells[2][1] = c[5];
        cells[2][2] = c[4];
    }
};

struct HoleList {
    std::vector<Hole*> all;
    std::vector<Hole*> tops[1000];
    std::vector<Hole*> bottoms[1000];
    std::vector<Hole*> lefts[1000];
    std::vector<Hole*> rights[1000];
    int count;  // number of holes with given central sum
};

struct Constraint {
    char side;  // will be t, b, l, or r for top, etc.
    int value;  // required value for the side
    size_t count;  // how many holes match this constraint?
    Constraint(char s, int v, size_t c) : side(s), value(v), count(c) {}
    Constraint() = default;
    inline bool match(Hole* hole) {
        switch(side) {
            case('t'):
                return value == hole->top;
            case('b'):
                return value == hole->bottom;
            case('l'):
                return value == hole->left;
            case ('r'):
                return value == hole->right;
            default:  // will never get here
                return false;
        }
    }
};

#endif /* hole_h */


