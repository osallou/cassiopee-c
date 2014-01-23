// A simple program that computes the square root of a number
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>

#include "CassiopeeConfig.h"


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
  char* sequence=NULL;
  char* out=NULL;

  while ((c = getopt (argc, argv, "s:o:vh")) != -1)
      switch (c)
      {
         case 's':
           sequence = strdup(optarg);
           break;
         case 'o':
           out = strdup(optarg);
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

  if(sequence==NULL||out==NULL) {
      fprintf (stderr,
               "Sequence file or output file not specified in command line.\n");
	  return 1;
  }



  ifstream input( sequence );
  ofstream out_file ( out );
  bool first_seq = true;
  for( string line; getline( input, line ); )
  {
    // Skip lines starting with '>'
    if(line[0] == '>') { 
        if(first_seq) {
            continue;
        }
        else {
            break;
        }
    }
    first_seq = false;
    // write other lines
    transform(line.begin(), line.end(), line.begin(), ::tolower);
    out_file << line;
  }
  out_file.close();

  return 0;
}

