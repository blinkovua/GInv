# GInv

## Abstract

The open source software GINV implements the Gröbner bases method for systems
of equations. GINV is a C++ module of Python designed for constructing Gröbner
bases of ideals and modules in polynomial, differential and difference rings.
Gröbner bases are constructed by involutive algorithms.

GINV is an open source software.

## Installation guide

Beforehand, install the following dependences libraries:

Debian/Ubuntu

    apt install g++ cmake libgmp-dev python3-dev libcppunit-dev python3-pygraphviz doxygen-gui libgraphviz-dev cython3 jupyter-notebook jupyter-nbconvert build-essential python3-pip

Fedora/Red Hat/CentOS

    yum install g++ cmake libgmp-devel python3-devel libcppunit-devel python3-pygraphviz doxygen-gui libgraphviz-devel cython3 jupyter-notebook jupyter-nbconvert build-essential python3-pip

To download the Ginv source clone the Github folder:

git clone https://github.com/blinkovua/GInv.git

The Ginv source has three parts.

1. First of them is util. To install it go to util folder and make it:

        cd GInv/build/util
        cmake ../../util/
        make

    To test the result

        cd test
        ./unittest
        ./graphtest

2. The second part is ginv. To install it go to ginv folder and make it:

        cd GInv/build/ginv
        cmake ../../ginv/
        make

    To test the result use the tests collected in test folder.

3. The third part generate the library *.co in GInv/build/cython/build.
To install it go to cython folder and use the setup file:

        cd GInv/build/cython
        cp -R ../../cython/* ./
        python3 ./setup.py build

    To test the result use the tests collected in test folder.

At the end copy the *.co in your work folder. Then open the terminal in work
folder and type

    jupyter notebook --ip 192.168.1.1 --port=8888

where 192.168.1.1 is ip address of your computer or local host and 8888
is an open to you port. Follow the link in browser and enjoy.

## Getting started

GInv work with polynomial and differential rings over ZZ. To describe such
a ring we need to indicate independent variables x1, ..., xn and functions
u1, ..., um of them. The product of independent variables and the partial
derivations of the  functions is called a monomial. The differential ring
is the set of linear combinations of monomials over ZZ. The number of
independent variables is called size, the number of functions
is called pos. If the ring is polynomial then pos=-1. In GInv monomials
x1^m1*...*xn^mn printed as

    [pos degree:m0 ... mn]

where degree is equal to sum m0+...+mn.

Ex. 1. Let's consider the ring ZZ[x0,x1,x2,x3]. First at all we need to define
monomials x0, x1, x2, x3. In GInv the monomial xi written as

    monom.variable(i, size, pos)

Thus the cycle

    size, pos = 4, -1
    vars = [monom.variable(i, size, pos) for i in range(size)]

give list vars, which elements is four our variables:

    vars
    [[-1 1:1 0 0 0], [-1 1:0 1 0 0], [-1 1:0 0 1 0], [-1 1:0 0 0 1]]

However, the elements of the vars have a type "ginv.monom". To transform they
in polynomials we use the cycle:

    x = [poly.to_monom("TOP", "lex", m) for m in vars]

Now the elements of x are polynomials x0,x1,x2,x3. Any polynomial of
ZZ[x0,x1,x2,x3] we can define using its symbolic expression. For ex.
we can define f=(3*x1*x3+2*x0)^3+15 by follow expression

    f=(x[1]*x[3]*3+x[0]*2)**3+15

It should be noted, that coefficients have to be to the right of the monomials.

GInv print only leading term of a polynomial and leading coefficient is printed
in octal form. List function can transform a polynomial in list of terms:

    f=(x[1]+x[2])**2
    list(f)

    [([-1 2:0 2 0 0], 0x1), ([-1 2:0 1 1 0], 0x2), ([-1 2:0 0 2 0], 0x1)]

Coefficients have the type "ginv.const_integer", the print function converts
them in the standard decimal form:

    L=list(f)
    L[0][1]
    0x1
    print(L[0][1])
    1

