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
     fprintf(stdout,"\t-f: file containing pattern to search\n");
     fprintf(stdout,"\t-r: apply tree reduction\n");
     fprintf(stdout,"\t-m: search mode: 0=DNA, 1=RNA, 2=Protein\n");
     fprintf(stdout,"\t-a: allow alphabet ambiguity search\n");
     fprintf(stdout,"\t-n: max consecutive N allowed matches in search\n");
     fprintf(stdout,"\t-e: max substitution allowed matches in search\n");
     fprintf(stdout,"\t-i: max indel allowed matches in search\n");
     fprintf(stdout,"\t-g: generates a dot file of the graph\n");
     fprintf(stdout,"\t-d: max depth of the graph\n");
     fprintf(stdout,"\t-o: output format: 0:tsv (default), 1:json\n");
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
  string pattern;
  bool is_pattern = false;
  char* pattern_file=NULL;
  opterr = 0;
  bool reduction = false;
  bool ambiguity = false;
  int nmax = 0;
  // DNA by default
  int mode = 0;

  int max_subst = 0;
  int max_indel = 0;

  bool graph = false;
  long max_graph = 0;

  int format = 0;

  while ((c = getopt (argc, argv, "d:ge:i:marhvs:p:n:o:f:")) != -1)
      switch (c)
      {
         case 's':
           sequence = strdup(optarg);
           break;
         case 'p':
           pattern = string(optarg);
           is_pattern = true;
           break;
         case 'f':
           pattern_file = strdup(optarg);
           is_pattern = true;
           break;
         case 'h':
        	 showUsage();
        	 return 0;
         case 'g':
        	 graph = true;
        	 break;
         case 'd':
        	 max_graph = atol(optarg);
        	 break;
         case 'r':
        	 reduction = true;
        	 break;
         case 'a':
        	 ambiguity = true;
        	 break;
         case 'n':
        	 nmax = atoi(optarg);
        	 break;
         case 'o':
           format = atoi(optarg);
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

  if(sequence==NULL|| ! is_pattern) {
      fprintf (stderr,
               "Sequence file or pattern not specified in command line.\n");
	  return 1;
  }

  const char logfile[] = "cassiopee.log";
  google::InitGoogleLogging(logfile);


  if(pattern_file!=NULL) {
    ifstream pfile(pattern_file);
    if(pfile.is_open()) {
      // Read only first line
      while( getline(pfile, pattern)) { 
         DLOG(INFO) << "Search pattern " << pattern;
         break;
      }
      pfile.close();
    }
  }

  //FLAGS_logtostderr = 1;

  CassieIndexer* indexer = new CassieIndexer(sequence);
  if(reduction) {
	  indexer->do_reduction = true;
  }
  indexer->max_depth = max_graph;
  indexer->index();
  if(graph) {
	  indexer->graph(max_graph);
  }
  DLOG(INFO) << "Tree size: " <<indexer->getTree()->size();

  CassieSearch* searcher = new CassieSearch(indexer);
  // Allow 1 substitution for test
  searcher->max_subst = max_subst;
  searcher->max_indel = max_indel;


  if(nmax > 0) {
	  ambiguity = true;
  }
  if(ambiguity) {
	  searcher->ambiguity = true;
	  if(nmax > 0) {
		  searcher->nmax = nmax;
	  }
  }

  searcher->search(pattern);
  searcher->sort();
  if(searcher->max_indel>0) {
	  searcher->removeDuplicates();
  }
  list<Match*> matches = searcher->matches;

  char* match_str;
  int p_length = 0;
  bool is_first = true;
  if(format==1) {
    cout << "[";
  }
  for (std::list<Match*>::iterator it = matches.begin(); it != matches.end(); it++) {
      p_length = pattern.length();
	  DLOG(INFO) << "Match at: " << (*it)->pos << ", errors: " << (*it)->subst << "," << (*it)->in << "," << (*it)->del;
	  // For debug
	  ifstream seqstream (sequence, ios_base::in | ios_base::binary);
	  seqstream.seekg((*it)->pos, seqstream.beg);
	  match_str = new char[p_length+1]();
	  if(((*it)->in - (*it)->del) != 0) {
		  p_length = pattern.length() + (*it)->in - (*it)->del;
	  }
	  seqstream.read(match_str, p_length + 1);
	  match_str[p_length] = '\0';
	  DLOG(INFO) << " => " << string(match_str);
    if(format == 0) {
        cout << (*it)->pos << "\t" << p_length << "\t" <<  (*it)->subst << "\t" << (*it)->in+(*it)->del << "\t" << (*it)->in << "\t" << (*it)->del << "\n";
    }
    else {
        if(is_first) {
          is_first = false;
        }
        else { cout << ","; }
        cout << "{\"position\": " << (*it)->pos << ",";
        cout << " \"length\": " << p_length << ",";
        cout << " \"substitution\": " <<  (*it)->subst << ",";
        cout << " \"indel\": " << (*it)->in+(*it)->del << ",";
        cout << " \"in\": " << (*it)->in << ",";
        cout << " \"del\": " << (*it)->del << "}";
    }
	  delete[] match_str;
	  seqstream.close();
  }

  if(format==1) { cout << "]\n"; }

  delete searcher;
  delete indexer;

  return 0;
}

