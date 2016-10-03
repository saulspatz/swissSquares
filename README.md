Generate puzzles like the one at  
https://www.brainbashers.com/showpuzzles.asp?puzzle=ZIWB
only with unique solutions.

For the solver, I generate all 3x3 "holes" first, and then backtrack by fitting
together holes with the appropriate sums and common border squares.  This works
well on 7x7 squares, but it's far too slow on 9x9 squares.

The best way I've come up with to find puzzles with unique solutions is to generate
random Latin squares (using a Markov Chain) and test if they lead to puzzles with
unique solutions.  For the 7X7, I get about 4 success in 10,000 trials, based on
limited experiments.  

Here are some of the puzzles i've found

43 23 21 
30 28 42 
23 32 40 

7 6 5 4 3 2 1 
6 0 4 0 1 0 2 
5 7 3 1 2 4 6 
1 0 6 0 7 0 5 
4 3 1 2 6 5 7 
3 0 2 0 5 0 4 
2 1 7 5 4 6 3 

31 34 29 
33 44 32 
30 27 42 

1 5 4 6 2 7 3 
7 0 3 0 1 0 6 
2 4 5 7 6 3 1 
5 0 7 0 4 0 2 
3 1 6 2 7 5 4 
6 0 1 0 5 0 7 
4 7 2 1 3 6 5 

38 31 21 
35 30 26 
20 40 41 

4 3 7 5 6 1 2 
5 0 1 0 2 0 4 
6 7 5 4 1 2 3 
7 0 4 0 3 0 1 
3 1 2 6 5 4 7 
2 0 3 0 7 0 6 
1 2 6 7 4 3 5 

37 31 26 
24 28 39 
20 36 39 

7 4 6 5 2 1 3 
4 0 7 0 3 0 1 
6 2 1 3 4 7 5 
5 0 4 0 1 0 7 
3 1 2 7 6 5 4 
1 0 3 0 5 0 2 
2 3 5 1 7 4 6 

41 26 23 
30 30 33 
18 34 39 

6 4 7 1 2 3 5 
4 0 5 0 3 0 1 
7 5 3 4 1 2 6 
5 0 4 0 6 0 2 
3 2 1 6 5 4 7 
1 0 2 0 4 0 3 
2 1 6 3 7 5 4 


19 31 40 
31 28 27 
44 34 22 

1 2 4 3 7 5 6 
2 0 3 0 1 0 5 
4 1 2 5 6 3 7 
5 0 1 0 3 0 2 
6 7 5 4 2 1 3 
3 0 7 0 4 0 1 
7 3 6 1 5 2 4 


23 32 41 
32 39 40 
39 29 20 

2 5 1 6 4 3 7 
3 0 2 0 6 0 5 
1 3 6 2 5 7 4 
4 0 5 0 7 0 6 
5 1 7 4 3 6 2 
6 0 3 0 2 0 1 
7 6 4 5 1 2 3 


  
 
