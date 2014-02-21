cassiopee-c
===========

Cassiopee index and search library C implementation.
It is a complete rewrite of the ruby Cassiopee gem.
It scan an input genomic sequence (dna/rna/protein) and search for a
subsequence with exact match or allowing substitutions (Hamming distance)
and/or insertion/deletions.

This program provides both a binary (Cassiopee) and a shared library.

Index is based on a suffix tree with compression. It is possible to save the
indexed sequence for later use without the need to reindex the whole sequence
(for large data sets).

See Cassiopee -h for all options.

Expected input sequence is a one-line sequence with no header. CassiopeeKnife  (see later chapter) can be used to convert Fasta sequences in cassiopee input sequences.

Compilation dependencies
===========

* cppunit
* Google log (glog)
* tree.hh (included):
  - Author: kasper.peeters (at) phi-sci.com
  - http://tree.phi-sci.com/ under GNU GPL
* libboost-serialization-dev,libboost-iostreams-dev

Runtime dependencies
===================

* Google log (glog)

* libboost-serialization, libboost-iostreams

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


CassiopeeKnife
=============

Convert an input Fasta sequence in a cassiopee input format (one line sequence
with no header).


TODO
====
