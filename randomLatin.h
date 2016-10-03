//
//  randomLatin.h
//
//  Created by Saul Spatz on 10/2/16.
//
/*
 Implementation of technique for generating almost uniformly-distributed random
 Latin squares from "Generating uniformly distributed random Latin squares,"
 by Jacobson and Matthews, Journal of Combinatorial Designs, 4(6) pp. 405–437 (1996).
 
 Suppose that we wish to replace the symbol at row r0, column c0, of an nxn Latin
 square, by the symbol s0, which is different from the symbol s1 that is currently
 there.  Now s0 must occur at some column c1 in row r0 and at some row r1 in
 column c0.  If s1 occurs at (r1,c1), then we have a 2x2 Latin subsquare, and we
 can simply exchange s0 and s1 in this subsquare, and get a new Latin square.
 
 If s1 does not occur at (r1,c1) we can constuct an "improper" Latin having one cell
 with two symbols, by replacing s1 with s0 at (r0,c0), replacing s0 with s1 at
 (r0,c1) and (r1,c0), and adding s0 as an additional symbol at (r1,c1).  Since s1
 has been added to both row r1 and column c1, and it wasn't removed from either
 of them, it occurs twice in each of these lines.  In an improper square, there is
 one cell with two different symbols, say a and b, and there is a third symbol say c,
 that occurs twice in the row and column containing the anomalous cell.
 
 To try to convert the improprer square to a proper one, we first remove one of the
 symbols say b, at the anomalous cell (r0,c0), and choose one of the cells containing
 a c in each line to be (r0,c1) and (r1,c0).  Now if the symbol at (r1,c1) is b, we
 can replace the c's at these cells by b, and replace the b at (r1,c1) by c.  If
 b is not the symbol at (r1,c1) then we add c as an additional symbol at (r1,c1),
 and the lines containing this cell have two b's.  Again we have an improper square.
 
 The paper shows that if in a proper square we choose r0, c0, s0 uniformly at random,
 and in an improper square we choose the symbol to remove from the anamolous square
 and which of the two possible cells in each line to place it at uniformly at random,
 then we get an irreducible Markov chain on the proper and improper nxn Latin squares,
 and that the limiting distibution of the subchain consisting of the proper squares
 is uniformly distributed on the set of all nxn Latin squares.
 
 For efficiency, the Latin square is redundantly represented by three squares, a
 square of symbols indexed by (r,c), a square of columns indexed by (r,s), and
 a square of rows, indexed by (c,s); each allows for two entries in a cell.
 
 */
#include <iostream>
#include <random>
#include <array>
#include <bitset>   // for audit
#include "alias.h"

template <int N>
class LatinGenerator {
    using Square = std::array<std::array<std::array<int, 2>, N+1>, N+1>;
    const int MIN_ITER = 2*(N-1)*(N-1)*(N-1);
    const int DIM = (N-1)/2;
private:
    std::random_device rd;
    std::mt19937 engine;
    std::uniform_int_distribution<int> dist;
    std::uniform_int_distribution<int> coin;
    Square RC;  // symbol square -- coords are (row,column)
    Square CS;  // row square    -- coords are (column,symbol)
    Square RS;  // column square -- coords are (row,symbol)
    bool isProper {true};
    int r0, r1;
    int c0, c1;
    int s0, s1;
    
    void perturbProper();
    void perturbImproper();
    inline void update();
    
public:
    LatinGenerator();
    std::array<std::array<int, N>, N> next();
    LatinGenerator(const LatinGenerator&) = delete;
    LatinGenerator& operator=(const LatinGenerator&) = delete;
};

template <int N>
LatinGenerator<N>::LatinGenerator() :engine(rd()),dist(1, N),coin(0, 1){
    // start with a cyclic latin square
    for (int row = 1; row < N+1; ++row) {
        int sym = row;
        for (int col = 1; col < N+1; ++col) {
            RC[row][col][0] = sym;
            CS[col][sym][0] = row;
            RS[row][sym][0] = col;
            sym += 1;
            if (sym == N+1) sym = 1;
        }
    }
}

template <int N>
void LatinGenerator<N>::perturbProper() {
    // Set r0, r1, c0, c1, s0, s1
    do {
        r0 = dist(engine);
        c0 = dist(engine);
        s0 = dist(engine);
    } while (RC[r0][c0][0] == s0);
    s1 = RC[r0][c0][0];
    r1 = CS[c0][s0][0];
    c1 = RS[r0][s0][0];
    
    RC[r0][c0][0] = s0;
    CS[c0][s0][0] = r0;
    RS[r0][s0][0] = c0;
    
    update();
}

template <int N>
void LatinGenerator<N>::perturbImproper() {
    // r0, c0, s0 are already set
    s1 = RC[r0][c0][coin(engine)];
    r1 = CS[c0][s0][coin(engine)];
    c1 = RS[r0][s0][coin(engine)];
    
    if (s1 == RC[r0][c0][0])
        RC[r0][c0][0] = RC[r0][c0][1];
    if (r1 == CS[c0][s0][0])
        CS[c0][s0][0] = CS[c0][s0][1];
    if (c1 == RS[r0][s0][0])
        RS[r0][s0][0] = RS[r0][s0][1];
    
    update();
}

template <int N>
void LatinGenerator<N>::update() {
    // This part is common to perturbProper and perturbImproper
    
    isProper = RC[r1][c1][0] == s1;
    bool improper = not isProper;
    // For improper squares, use the second index
    
    RC[r0][c1][0] = s1;
    RS[r0][s1][0] = c1;
    CS[c1][s1][improper] = r0;
    
    RC[r1][c0][0] = s1;
    CS[c0][s1][0] = r1;
    RS[r1][s1][improper] = c0;
    
    CS[c1][s0][0] = r1;
    RS[r1][s0][0] = c1;
    RC[r1][c1][improper] = s0;
    
    if (improper) {
        r0 = r1;
        c0 = c1;
        s0 = s1;
    }
}

template <int N>
std::array<std::array<int, N>, N> LatinGenerator<N>::next(){
    std::array<std::array<int, N>, N> answer;
    int iters = 0;
    while (iters < MIN_ITER or not isProper ) {
        if (isProper)
            perturbProper();
        else
            perturbImproper();
        iters += 1;
    }
    for (int r = 0; r < N; ++r)
    for (int c = 0; c < N; ++ c)
        answer[r][c] = RC[r+1][c+1][0];
    return answer;
}
