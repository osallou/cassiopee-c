// A simple program that computes the square root of a number
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#include <libgen.h>
#else
#define strdup _strdup
#include "win/getopt.h"
#endif
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>

#ifndef _WIN32
#include "CassiopeeConfig.h"
#else
#define Cassiopee_VERSION_MAJOR 1
#define Cassiopee_VERSION_MINOR 0
#define use_openmp 0
#define SUFFIX_CHUNK_SIZE 10

#endif


#include "Cassiopee.h"


using namespace std;

void showUsage() {
	 fprintf(stdout,"Usage:\n");
	 fprintf(stdout,"\t-s: sequence to convert\n");
	 fprintf(stdout,"\t-o: output file\n");
	 fprintf(stdout,"\t-v: show version\n");
	 fprintf(stdout,"\t-h: show this message\n");
}

void showVersion() {
	 fprintf(stdout,"%s Version %d.%d\n",
	            "Cassiopee",
	            Cassiopee_VERSION_MAJOR,
	            Cassiopee_VERSION_MINOR);
}

int main (int argc, char *argv[])
{
  if (argc == 1) {
	  showVersion();
  	  showUsage();
	  return 1;
  }


  int c;
  string sequence="";
  string out="";

  while ((c = getopt (argc, argv, "s:o:vh")) != -1)
      switch (c)
      {
         case 's':
           sequence = string(optarg);
           break;
         case 'o':
           out = string(optarg);
           break;
         case 'h':
        	 showUsage();
        	 return 0;
         case 'v':
        	 showVersion();
           return 0;
         default:
           abort ();
      }

  if(sequence.empty()||out.empty()) {
      fprintf (stderr,
               "Sequence file or output file not specified in command line.\n");
	  return 1;
  }



  CassiopeeUtils::transform_fasta(sequence, out);


  return 0;
}

