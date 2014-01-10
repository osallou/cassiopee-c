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

Tree graph generation
=====================

API provides the graph method to generate a cassiopee.dot file.
To generate the image from the file:

    dot -Tpng cassiopee.dot > cassiopee.png

Documentation
============

doxygen bin/Doxyfile

BUGS
====


TODO
====

* Add search with errors (subst/indel)
* Manage dna/rna/protein specific alphabets (skip N or allow max N consecutive)
