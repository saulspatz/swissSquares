//
//  alias.h
//  Latin3
//
//  Created by Saul Spatz on 9/17/16.
//  Copyright © 2016 Saul Spatz. All rights reserved.
//

#include <bitset>
#include <utility>   //for pair
#include <array>

template <int N>
using Set = std::bitset<N+1>;
template <int N>
using Givens = std::array<std::array<int, (N-1)/2>, (N-1)/2>;
using Coords = std::pair<int,int>;

