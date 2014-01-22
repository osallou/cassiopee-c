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
     fprintf(stdout,"\t-r: apply tree reduction\n");
     fprintf(stdout,"\t-m: search mode: 0=DNA, 1=RNA, 2=Protein\n");
     fprintf(stdout,"\t-a: allow alphabet ambiguity search\n");
     fprintf(stdout,"\t-n: max consecutive N allowed matches in search\n");
     fprintf(stdout,"\t-e: max substitution allowed matches in search\n");
     fprintf(stdout,"\t-i: max indel allowed matches in search\n");
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
  bool reduction = false;
  bool ambiguity = false;
  int nmax = 0;
  // DNA by default
  int mode = 0;

  int max_subst = 0;
  int max_indel = 0;

  while ((c = getopt (argc, argv, "e:i:marhvs:p:n:")) != -1)
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
         case 'r':
        	 reduction = true;
        	 break;
         case 'a':
        	 ambiguity = true;
        	 break;
         case 'n':
        	 nmax = atoi(optarg);
        	 break;
         case 'e':
        	 max_subst = atoi(optarg);
        	 break;
         case 'i':
        	 max_indel = atoi(optarg);
        	 break;
         case 'm':
        	 mode = atoi(optarg);
        	 break;
         case 'v':
        	 showVersion();
           return 0;
         case '?':
           if (optopt == 's' || optopt == 'p')
             fprintf (stderr, "Option requires an argument.\n");
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

  CassieIndexer* indexer = new CassieIndexer(sequence);
  if(reduction) {
	  indexer->do_reduction = true;
  }
  indexer->index();
  indexer->graph();
  LOG(INFO) << "Tree size: " <<indexer->getTree()->size();

  CassieSearch* searcher = new CassieSearch(indexer);
  // Allow 1 substitution for test
  searcher->max_subst = max_subst;
  searcher->max_indel = max_indel;


  if(nmax > 0) {
	  searcher->ambiguity = true;
  }
  if(ambiguity) {
	  searcher->ambiguity = true;
	  if(nmax > 0) {
		  searcher->nmax = nmax;
	  }
  }

  searcher->search(string(pattern));
  searcher->sort();
  if(searcher->max_indel>0) {
	  searcher->removeDuplicates();
  }
  list<Match*> matches = searcher->matches;

  char* match_str;
  int p_length = string(pattern).length();


  for (std::list<Match*>::iterator it = matches.begin(); it != matches.end(); it++) {
	  LOG(INFO) << "Match at: " << (*it)->pos << ", errors: " << (*it)->subst << "," << (*it)->in << "," << (*it)->del;
	  // For debug
	  ifstream seqstream (sequence, ios_base::in | ios_base::binary);
	  seqstream.seekg((*it)->pos, seqstream.beg);
	  match_str = new char[p_length+1]();
	  if(((*it)->in - (*it)->del) != 0) {
		  p_length = string(pattern).length() + (*it)->in - (*it)->del;
	  }
	  seqstream.read(match_str, p_length + 1);
	  match_str[p_length] = '\0';
	  LOG(INFO) << " => " << string(match_str);
	  delete[] match_str;
	  seqstream.close();
  }


  delete searcher;
  delete indexer;

  return 0;
}

