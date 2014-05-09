TdZdd
===========================================================================

### A top-down/breadth-first decision diagram manipulation framework

TdZdd is an efficient C++ library for manipulating ordered decision diagrams
(DDs).
Unlike the ordinary DD libraries that are optimized for efficient operations
between multiple DDs, TdZdd focuses on direct construction of a non-trivial
DD structure from scratch.
It has three basic functions:

* Top-down/breadth-first DD construction
* Reduction as BDDs/ZDDs
* Bottom-up/breadth-first DD evaluation

The DD construction function takes user's class object as an argument,
which is a specification of the DD structure to be constructed.
An argument of the DD evaluation function represents return data type and
the procedure to be executed at each DD node.
The construction and evaluation functions can also be used to implement
import and export functions of DD structures from/to other DD libraries.

Features of TdZdd include:

* Support of *N*-ary (binary, ternary, quaternary, ...) DDs
* Parallel processing with OpenMP
* Distributed with sample applications
* Header-only C++ library; no need for installation

This software is released under the MIT License, see [LICENSE](LICENSE).

Documents
---------------------------------------------------------------------------

* [User's Guide](doc/users-guide.md)
* [Reference Manual](doc/html/index.html)

See also
---------------------------------------------------------------------------

* [Graphillion](http://graphillion.org): a Python software package using
  TdZdd as one of its core components.
