//
//  clueGenerator.h
//  SwissSquares
//
//  Created by Saul Spatz on 9/24/16.
//  Copyright © 2016 Saul Spatz. All rights reserved.
//

#ifndef clueGenerator_h
#define clueGenerator_h
#include <random>
#include "alias.h"

template<int N>
struct ClueGenerator {
    std::random_device rd;
    std::mt19937 engine;
    std::uniform_int_distribution<int> dist;
    
    ClueGenerator() :engine(rd()), dist(15, 8*N-7) {}
    Givens<N> next() ;
};

template<int N>
Givens<N> ClueGenerator<N>::next() {
    Givens<N> answer;
    int DIM = (N-1)/2;
    for (int i = 0; i < DIM; ++i)
    for (int j = 0; j < DIM; ++j)
        answer[i][j] = dist(engine);
    return answer;
}


#endif /* clueGenerator_h */
