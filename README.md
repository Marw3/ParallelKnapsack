Dynamic Knapsack Algorithm with OMP
====================================================================

Here I will describe the algorithm that was created during the implementation challenge. The task was to implement an parallel solution to the 0-1 knapsack problem by using a dynamic programming approach. In dynamic programming the idea is to break a problem into smaller sub-problems. After solving the sub-problems one can reassamble the solutions using the relation between the stored solutions to the sub-problems and the original problem. Splitting the problem into sub-problems also allows processing the sub-problems in parallel and thus increases the speed even more.

Description of the Algortihm
------------

Given the instance of the problem with N items and a capacity of C. The goal was to implement an exact solution to the 0-1-knapsack problem. The algorithm splits the problem into smaller problems and solves the problem recursively. Starting from $`1`$ item to take up to $`N`$ item's. For each set of $`n`$ items in consideration the algorithm goes from capacity $`0`$ to $`C`$ and calculates the best solution for the problem. The algorithms uses an $`N`$ x $`(C+1)`$ matrix where $`f_n(\hat{c})`$ is refering to the entry at position $`(n,\hat{c})`$. This entry is calculating the solution considering $`(n+1)`$ items ans $`\hat{c}`$ capacity. The solution is calculated recursively by using the following relation between the previous solutions for $`(n-1)`$ items:

$`f_n(\hat{c}) = max(p_n + f_n(\hat{c} - w_n), f_{n-1}(\hat{c}))`$

Where for the current item $`n`$ (indexed at $`0`$) with weight $`w_n`$ and profit $`p_n`$.

After solving all sub-problems i.e. filling the whole matrix, the last entry at position $`(N, (C+1))`$ is contianing the solution for the problem.

To get the items which where choosen for the solution the algorithm backtracks through the matrix and checks if an item was choosen or not. The

Implementation
------------

## 1. For
    FOR each item n
        w_n = items(n).weight
        p_n = items(n).profit

        FOR \hat{c} in 0 to C


        IF $`w_n >= \hat{c}`$
        ENDIF

        IF is_new_author(component, commit.author) THEN
          component.authors_twr += twr
        ENDIF
    ENDFOR

First the algorithm allocates the memory of for an $`N`$ x ($`C+1`$) matrix. This matrix is used to store the solutios to the sub-problems. The algorithm consideres in each step one more item. I.e. in the $`i`$'th step the algorithm consideres the solution for the first $`i`$ items. In every step the algorithm goes from $`0`$ to $`C`$ and calculates in each step the best solution for the current $`i`$ items with the current capacity $`\hat{c}`$.
