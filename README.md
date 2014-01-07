cassiopee-c
===========

Cassiopee index and search library C implementation

Dependencies
===========

cppunit

Google log (glog)

tree.hh

Compilation
===========

    cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_BUILD_TYPE=Debug .
    make

Valgrind
=======

    valgrind --leak-check=full bin/Cassiopee  -s test/sequence.txt -p ggc

Static analysis
==============

    CXX=/usr/share/clang/scan-build/c++-analyzer cmake .

Tests
=====

    bin/test_cassiopee

or 

    cd test; ctest -V

Documentation
============

doxygen bin/Doxyfile

BUGS
====


TODO
====

* Add tree reduction
* Add max depth of branches
* Add search with errors (subst/indel)
* Manage dna/rna/protein specific alphabets (skip N, ambiguity....)
