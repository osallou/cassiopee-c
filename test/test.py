#!/usr/bin/python

import os
import sys
import subprocess

def searchPattern(pos, pattern):
    #print('Search, pos: '+str(i)+', '+pattern)
    subprocess.check_call(['../bin/cassiopee', '-p', pattern,
                     '-s', 'test2.fasta.cass',
                     '-o', 'test.out',
                     '-r'
                    ])
    results = None
    with open('test.out', 'r') as res:
        results = res.readlines()
    if not results:
        return False
    match = False
    for result in results:
        elts = result.split('\t')
        if str(pos) == elts[0]:
            match = True
            break
    if not match:
        return False
    return True
       


sequence = ''
with open('test2.fasta.cass', 'r') as seq:
    sequence = seq.read()

for i in range(0, len(sequence) - 1):
    for j in range(1, len(sequence) - i ):
        found = searchPattern(i, sequence[i:i+j])
        if not found:
            print('Not found: pos: '+str(i)+', '+sequence[i:i+j])
            sys.exit(1)
