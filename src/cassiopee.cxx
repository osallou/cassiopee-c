// A simple program that computes the square root of a number
#include <stdio.h>
#include <glog/logging.h>

#include "Cassiopee.h"
#include "CassiopeeConfig.h"




using namespace std;




InDel::InDel(): in(0),del(0), max_in(0), max_del(0) {

}

bool InDel::maxReached() {
	return in >= max_in || del >= max_del;
}

TreeNode::TreeNode(char nc): c(nc), next(0), positions() {

}

TreeNode::TreeNode(char nc, int pos): c(nc), next(pos), positions() {

}

TreeNode::TreeNode(): c(), next(0), positions() {

}


CassieIndexer::~CassieIndexer() {
	if(this->seqstream) {
		this->seqstream.close();
	}
}

CassieIndexer::CassieIndexer(char* path): filename(path), seqstream(path)
{

    // If we couldn't open the input file stream for reading
    if (!this->seqstream)
    {
        // Print an error and exit
    	LOG(FATAL) << "Error: " << path << " could not be opened for reading!" ;
        exit(1);
    }

    this->seqstream.seekg (0, this->seqstream.end);
    this->seq_length = this->seqstream.tellg();
    this->seqstream.seekg (0, this->seqstream.beg);
}

void CassieIndexer::index() {

	tree<TreeNode> tr;

	tree<TreeNode>::iterator top;

	top = tr.begin();

	cout << "I will update this file:" << this->filename << endl;

	LOG(INFO) << "Indexing " << this->filename ;

	for (int i=0; i<this->seq_length; i++) {

		char* suffix = new char [this->seq_length-i];

		cout << "Index suffix " << i << endl;

		this->seqstream.seekg(i, this->seqstream.beg);
		//suffix=NULL;

		// Extract a suffix
		this->seqstream.read(suffix, this->seq_length-1-i);

		// << "suffix " << this->seq_length-1-i << ", " << suffix;

		tr = this->filltree(tr, top, suffix);

	}


}

string CassieIndexer::getSuffix(int pos) {
	char* suffix;

    this->seqstream.seekg(pos);
    // Get the rest of the line and print it
    this->seqstream.getline(suffix,this->seq_length-1);
	return suffix;
}

tree<TreeNode> CassieIndexer::filltree(tree<TreeNode> tr, tree<TreeNode>::iterator top, const char* suffix) {

	tree<TreeNode>::iterator sib;

	sib = tr.begin();

	bool match = false;
	int counter = 0;

	while(!match && sib!=tr.end()) {
		LOG(INFO) << "compare " << suffix[counter] << "with " << sib->c ;

		if(sib->c == suffix[counter]) {
			// match
			match = true;
			std::list<long>::iterator end = sib->positions.end();
			sib->positions.insert(end, counter);
		}
		else {
			// Switch to next node
			++sib;
		}
	}

	if(!match) {
		char node_char = suffix[counter];
		LOG(INFO) << "No match found, add new node " << node_char ;
		TreeNode* node = new TreeNode(node_char, counter);
		tr.insert(sib, *node);
	}
	return tr;

}


int main (int argc, char *argv[])
{
  if (argc < 2)
    {
    fprintf(stdout,"%s Version %d.%d\n",
            argv[0],
            Cassiopee_VERSION_MAJOR,
            Cassiopee_VERSION_MINOR);
    fprintf(stdout,"Usage: %s file\n",argv[0]);
    return 1;
    }

  const char logfile[] = "cassiopee.log";
  google::InitGoogleLogging(logfile);
  FLAGS_logtostderr = 1;


  char* sequence = argv[1];
  CassieIndexer* indexer = new CassieIndexer(sequence);
  indexer->index();
  return 0;
}
