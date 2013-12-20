// A simple program that computes the square root of a number
#include <stdio.h>


#include "Cassiopee.h"
#include "CassiopeeConfig.h"

#include "tree/tree.hh"

using namespace std;

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
        cerr << "Error: " << path << " could not be opened for reading!" << endl;
        exit(1);
    }

    this->seqstream.seekg (0, this->seqstream.end);
    this->seq_length = this->seqstream.tellg();
    this->seqstream.seekg (0, this->seqstream.beg);
}

void CassieIndexer::index() {
	cout << "I will update this file:" << this->filename << endl;
	for (int i=0; i<this->seq_length; i++) {
		cout << "Index suffix " << i << endl;
	}
}

string CassieIndexer::getSuffix(int pos) {
	string suffix = NULL;

    this->seqstream.seekg(pos);
    // Get the rest of the line and print it
    getline(this->seqstream, suffix);
	return suffix;
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
  char* sequence = argv[1];
  CassieIndexer* indexer = new CassieIndexer(sequence);
  indexer->index();
  return 0;
}
