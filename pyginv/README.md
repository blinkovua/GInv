# PyGInv: Investigation and Solution of Polynomial and Differential Systems

**Yu. A. Blinkov**  
E-mail: blinkovua@info.sgu.ru

**Abstract:**  
Computations in the PyGInv package for Gröbner bases in polynomial and differential systems and the construction of numerical schemes are presented. The results allow one to find the dimension of the solution space, numerically find all solutions in the polynomial case for zero-dimensional ideals, compute symmetries of differential equations without using special formulas, and perform transformations with difference schemes, for example, in the case of compact difference schemes. The PyGInv package is a lightweight version of GInv, written in pure Python and integrated with the SymPy computer algebra system.

**Keywords:** computer algebra, Gröbner bases, polynomial systems, differential equations

## Gröbner Bases

Historically, works aimed at creating the theory of Gröbner bases were focused on constructively solving the ideal membership problem [1-4].

An alternative approach to the Gröbner basis method was formed during the investigation of differential equations [5-7]. It was oriented towards reducing the system to a form from which the dimension of the solution space could be determined.

The difference between Gröbner bases and their extension, the involutive basis, is best demonstrated in Figure 1. In the left figure, the cones of divisible monomials are shown in different colors. As a result of fixing the monomial ordering in the general polynomial case, at the intersection points of the cones, it is necessary to compute S-polynomials relative to the current basis and add them to the basis. Upon completion of this process, a Gröbner basis in the chosen order will be obtained. Now, to determine membership, it is sufficient to compute the normal form of the polynomial relative to the current basis. In this case, the sequence of reductions is unimportant because all S-polynomials already belong to the basis. In this specific case, it is easy to calculate the Hilbert polynomial HP₈ = 3s + 10, which consists of three one-dimensional cones x^s, y^s, xy^s and a number of monomials that do not fall into these cones: 1_ℳ, x, y, x², xy, ... Consequently, the formula HP₈ = 3s + 10 is chosen such that it gives the exact number of monomials up to total degree s ≥ 8 that have no divisor among the leading monomials of the basis.

![Figure 1](divfull.png)

![Figure 2](divjanet.png)

![Figure 3](divjanetfull.png)

In the general case, such a calculation is difficult to perform. If the cones did not intersect, the computation of the Hilbert polynomial could be represented as the difference between the cone of the unit monomial 1_ℳ and the sum over all cones formed by the leading monomials of the basis.

The Janet involutive partition of variables prohibits division by variables called non-multiplicative. As a result, the cones in Figure 2 do not intersect, but there are also unfilled cones. Figure 3 shows a basis brought to involution by complementing the current basis with prolongations with respect to non-multiplicative variables. The involutive approach to constructing Gröbner bases was developed in the works [8-10].

## PyGinv

PyGInv is a "lightweight" version of GIinv [11, 12] (short for Gröbner INVolutive). It is written in "pure" Python and uses the SymPy computer algebra system (https://www.sympy.org/) for working with parameters. Where possible, PyGInv allows import/export of data from/to SymPy. For building the Janet tree, the Graphviz package (https://graphviz.org/) is optionally used.

Currently, PyGInv allows building Gröbner bases for polynomials, modules, linear differential equations, and linear difference schemes. In many cases, as will be shown below, it is also possible to work with nonlinear differential equations and difference schemes. PyGInv contains additional functions for investigating and solving polynomial and differential systems.

The source code, presented in four files (`monomial.py`, `polynomial.py`, `janet.py`, `ginv.py`), takes up just under 2000 lines of code. All source files and example files, including those described below, are located at https://github.com/blinkovua/GInv/tree/master/pyginv.

Using the debugged PyGInv interface, it is planned to partially transfer it to the GIinv package written in C++11, which uses its own memory manager [13] for symbolic computations and "long" arithmetic.

All computations were performed on an Intel(R) Core(TM) i7-4770K CPU @ 3.50GHz processor, with 16 GB of RAM, in Debian 12.

## Polynomials and Modules

Let us consider the purely monomial basis illustrated in Figures 1-3.

Line 1 imports PyGInv as the main module `ginv.py`.

Line 3 specifies the type of admissible term ordering. Currently implemented are `POTlex`, `TOPlex`, `POTdeglex`, and `TOPdeglex`. In their names, `lex` stands for lexicographical, and `deglex` stands for degree lexicographical by total degree. Accordingly, `POT` means Position Over Term, and `TOP` means Term Over Position.

```python
from ginv import *

Monom.cmp = Monom.TOPdeglex
x, y = Poly.init('x, y'.split(', '))
mnm = Janet()
res = ginvBlockLow((x**7*y, x**5*y**2, \
   x**4*y**3, x**2*y**4), mnm, level=1)
mnm.saveImage('mnm.pdf')
```

Line 4 defines the variables `x`, `y` with the order $x \succ y$. Line 5 defines the type of involutive division; currently, only Janet division based on the Janet binary tree [1] shown in Figures 4 and 5 is implemented. Saving the result as a Janet tree happens on line 8. Lines 9-10 call the algorithm `ginvBlockLow` implementing the block strategy described in [2], which works only with the `TOPdeglex` ordering. The algorithm `ginvMin`, implementing the minimal strategy, works with all types of orderings.

![Figure](mnm.png)

Figures 4 and 5 show different levels of output information. The figure displays only the tree structure, with monomials belonging to the reduced Gröbner basis shown in bold, and monomials additionally belonging to the minimal involutive basis [3] shown in regular font.

Below, lines 1-17 demonstrate the computation of the Gröbner basis for the standard example of cyclic roots. In this case, the resulting ideal is zero-dimensional with $HP_{6}=4s+6$.

```python
Monom.cmp = Monom.TOPdeglex
x1, x2, x3, x4 = \
  Poly.init('x1, x2, x3, x4'.split(', '))
cyclic4 = Janet()
res = ginvBlockLow((
x1+x2+x3+x4,\
x1*x2+x2*x3+x3*x4+x4*x1,\
x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2,\
x1*x2*x3*x4-1), \
cyclic4, level=1)
print(f"    crit1: {res[1]}")
print(f"    crit2: {res[2]}")
print(f"     time: {res[0]:.2f} sec")
print(f"    count: {cyclic4.count()}")
print(f"reduction: {cyclic4.reduction()}")
print(f"       HP: {cyclic4.HP()}")
cyclic4.saveImage('cyclic4.pdf', level=3)
```

Lines 11-17 show the output of various information related to the computation. Their result is presented below in lines 18-23.

```python
    crit1: 6
    crit2: 0
     time: 0.02 sec
    count: 7
reduction: 54
       HP: 4s + 6
```

In Figure 5, compared to the monomial below, its non-multiplicative variables and the prolongations constructed from them are additionally displayed. At the very bottom, the "ancestor" is recorded; Buchberger's criteria [4] and some types of involutive divisions [5] (not yet implemented in PyGInv) operate based on it.

![Figure](cyclic4.png)

Lines 24-34 output the Gröbner basis. The wrapper class `Wrap` used contains a lot of technical information for efficiently organizing computations.

```python
for w in cyclic4.GB():
    print(w.poly)
x3**2*x4**4 + x2*x3 + x2*x4*-1 + x3*x4 + \
  x4**2*-2
x3**3*x4**2*-1 + x3**2*x4**3*-1 + x3 + x4
x2*x4**4*-1 + x4**5*-1 + x2 + x4
x2*x3*x4**2 + x3**2*x4**2 + x2*x4**3*-1 + \
  x3*x4**3 + x4**4*-1 + -1
x2*x3**2*-1 + x3**2*x4*-1 + x2*x4**2 + x4**3
x2**2*-1 + x2*x4*-2 + x4**2*-1
x1 + x2 + x3 + x4
```

Lines 1-12 show the organization of computations using the syzygy module for the fourth cyclic roots. For this, dependent variables are introduced on line 2, with the variable order $f \succ p_1 \succ  p_2 \succ  p_3 \succ  p_4$. In modules, they play the same role as functions in differential equations and difference schemes. Internally, this is organized using a positive number that determines the position. Naturally, they should only appear linearly in expressions. In this example, we have 6 different Janet trees according to the number of dependent variables. Therefore, line 6 uses the wrapper class `Forest`. It is assumed that it will work with other types of involutive divisions.

```python
var = 'x1, x2, x3, x4'.split(', ')
fun = 'f, p1, p2, p3, p4'.split(', ')
var, fun = Poly.init(var, fun)
x1, x2, x3, x4 = var
f, p1, p2, p3, p4 = fun
cyclic4 = Forest(Janet)
res = ginvMin((
f*(x1+x2+x3+x4)-p1,\
f*(x1*x2+x2*x3+x3*x4+x4*x1)-p2,\
f*(x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2)-p3,\
f*(x1*x2*x3*x4-1)-p4), \
cyclic4, level=1)
```

To construct syzygies, only expressions not containing the dependent variable `f` are needed, so the loop on line 14 starts from position 1. For this system, an elimination ordering was not required. Lines 15-19 show the conversion of the computations into a vector form representation using SymPy.

```python
syzygy = []
for i in range(1, 5):
  for w in cyclic4[i].GB():
    syzygy.append(tuple(sympy.Add(\
*tuple(eval(f"{m.lcm(fun[k].lm())}*{c}") \
for m, c in w.poly if m.position() == k),\
   evaluate=False) for k in range(1, 5)))
syzygy
```

$[ ( x_{3}^{2} x_{4}^{4} - x_{4}^{2},   x_{2} x_{3}^{2} x_{4}^{2} - x_{2} x_{4}^{4} - x_{3}^{2} x_{4}^{3} - x_{3} x_{4}^{4} + x_{3} + x_{4}, \\  - x_{2} x_{3}^{2} x_{4} + x_{2} x_{4}^{3} + x_{4}^{4} - 1,   x_{2} x_{3}^{2} - x_{2} x_{4}^{2} + x_{3}^{2} x_{4} - x_{4}^{3}),\\  ( x_{2} x_{3}^{2} + x_{3}^{2} x_{4},   - x_{2} x_{3} - x_{2} x_{4} - x_{3}^{2} - x_{3} x_{4}, \  x_{2} + x_{4},   0), \\  ( - x_{2} - x_{4},   - x_{2}^{2} x_{4}^{2} + 1,  x_{2}^{2} x_{4} + x_{2} x_{4}^{2},  - x_{2}^{2} - 2 x_{2} x_{4} - x_{4}^{2})]$

Lines 21-28 perform a verification of the correctness of the three constructed syzygies.

```python
P = (x1+x2+x3+x4,\
x1*x2+x2*x3+x3*x4+x4*x1,\
x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2,\
x1*x2*x3*x4-1)
for syz in syzygy:
  print(sum(s*p for s, p \
    in zip(syz, P)).expand(), end=" ")
0 0 0
```
