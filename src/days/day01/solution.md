# Day 1: Secret Entrance

## Part A

### Abridged Problem Statement

Given a series of inputs of the form 'L/R x', where L indicates a left rotation and R indicates a right rotation, all by $x$ amount, the goal is to determine how many times the dial (having positions 0 to 99) points at 0.

### Solution

We can simply use modulo arithmetic to keep the dial within 0 to 99. We add one when the dial points at 0.

## Part B

### Abriged Problem Statement 

Given a series of inputs of the form 'L/R x', where L indicates a left rotation and R indicates a right rotation, all by $x$ amount, the goal is to determine how many times the dial (having positions 0 to 99) crosses the 0 position.

### Solution

There are two cases:

Let our initial position be $S$ and the final position be $E$. 

- Moving right

We are looking for multiples of 100 in the range $(S, E]$, that is, integers $k$ satisfying:
$$S < 100k \leq E$$

Thus, the count is given by
$$C = \left\lfloor \frac{E}{100} \right\rfloor - \left\lfloor \frac{S}{100} \right\rfloor.$$

Since $E > S$.

- Moving Left

When moving left, we have $E < S$. So, we need to find multiples of 100 in the range $[E, S)$, that is, integers $k$ satisfying:
$$E \leq 100k < S$$

Thus, the count is given by (think of it as counting integers strictly less than of $S$ and $E$) 
$$C = \left\lfloor \frac{S-1}{100} \right\rfloor - \left\lfloor \frac{E-1}{100} \right\rfloor.$$

Resolving both cases, we get the solution.