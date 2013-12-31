cassiopee-c
===========

Cassiopee index and search library C implementation

Dependencies
===========

Google log (glog)

tree.hh

Compilation
===========

cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_BUILD_TYPE=Debug .
make

BUGS
====
Adding sibling nodes fails though with no error
Only adding child nodes seems to work

TODO
====

* Add exact search
* Add tree reduction
* Add max depth of branches
* Add search with errors (subst/indel)
* Manage dna/rna/protein specific alphabets (skip N, ambiguity....)
