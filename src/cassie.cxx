// A simple program that computes the square root of a number
#include <stdio.h>
#include <glog/logging.h>
#include <unistd.h>

#include "Cassiopee.h"

using namespace std;

void showUsage() {
	 fprintf(stdout,"Usage:\n");
	 fprintf(stdout,"\t-s: sequence to index\n");
	 fprintf(stdout,"\t-p: pattern to search\n");
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
  char* pattern=NULL;
  opterr = 0;
  while ((c = getopt (argc, argv, "hvs:p:")) != -1)
      switch (c)
      {
         case 's':
           sequence = strdup(optarg);
           break;
         case 'p':
           pattern = strdup(optarg);
           break;
         case 'h':
        	 showUsage();
        	 return 0;
         case 'v':
        	 showVersion();
           return 0;
         case '?':
           if (optopt == 's' || optopt == 'p')
             fprintf (stderr, "Option -%s requires an argument.\n", (char*)optopt);
           else if (isprint (optopt))
             fprintf (stderr, "Unknown option `-%s'.\n", (char*)optopt);
           else
             fprintf (stderr,
                      "Unknown option character `\\x%x'.\n",
                      optopt);
           return 1;
         default:
           abort ();
      }

  if(sequence==NULL||pattern==NULL) {
      fprintf (stderr,
               "Sequence file or pattern not specified in command line.\n");
	  return 1;
  }


  const char logfile[] = "cassiopee.log";
  google::InitGoogleLogging(logfile);
  FLAGS_logtostderr = 1;

  //char* sequence = argv[1];
  CassieIndexer* indexer = new CassieIndexer(sequence);
  indexer->index();



  //string suffix = "gggc";
  list<long> matches = indexer->search(string(pattern));
  matches.sort();
  for (std::list<long>::iterator it = matches.begin(); it != matches.end(); it++) {
	  LOG(INFO) << "Match at: " << *it;
  }




  return 0;
}

