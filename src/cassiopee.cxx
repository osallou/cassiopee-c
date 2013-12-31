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

CassieIndexer::CassieIndexer(char* path): filename(path), seqstream(path, ios_base::in | ios_base::binary)
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


	LOG(WARNING) << "I will update this file:" << this->filename;

	LOG(INFO) << "Indexing " << this->filename ;


	for (int i=0; i<this->seq_length-1; i++) {
	//for (int i=0; i<2; i++) {

		int suffix_len = this->seq_length - i -1;

		char* suffix = new char [suffix_len];

		LOG(INFO) << "Index suffix " << i;

		this->seqstream.seekg(i, this->seqstream.beg);
		//suffix=NULL;

		// Extract a suffix
		this->seqstream.read(suffix, suffix_len);

		LOG(INFO) << "Suffix: "  << suffix << ", length: " << suffix_len << ":" << strlen(suffix);

		this->filltree(suffix, suffix_len, i);

		delete[] suffix;

	}


}

string CassieIndexer::getSuffix(int pos) {
	char* suffix;

    this->seqstream.seekg(pos);
    // Get the rest of the line and print it
    this->seqstream.getline(suffix,this->seq_length-1);
	return suffix;
}

void CassieIndexer::fillTreeWithSuffix(tree<TreeNode>::iterator sib, const char* suffix, int suffix_pos, int suffix_len, int pos) {

	tree<TreeNode>::iterator top_node = sib;

	for(int i=suffix_pos;i<suffix_len-1;i++) {
		char node_char = suffix[i];
		TreeNode* node = new TreeNode(node_char);
		if(i==suffix_len-2) {
			// If last node, append position of suffix
			LOG(INFO) << "add position to node";
			std::list<long>::iterator end = node->positions.end();
			node->positions.insert(end, pos);
		}
		//LOG(INFO) << "fill tree with new node " << node_char;
		if(top_node==NULL) {
			this->tr.insert(tr.end(),*node);
			top_node = this->tr.begin();
			//LOG(INFO) << "insert " << node_char << " at level " << this->tr.depth(top_node) ;
		}
		else {
			top_node = this->tr.append_child(top_node,*node);
			//LOG(INFO) << "insert " << node_char << " at level " << this->tr.depth(top_node) ;
		}
	}
}

tree<TreeNode> CassieIndexer::getTree() {
	return this->tr;
}

void CassieIndexer::filltree(const char* suffix, int suffix_len, int pos) {

	tree<TreeNode>::iterator sib;


	sib = this->tr.begin();

	bool match = false;

	int counter = 0;

	while(!match && sib!= this->tr.end() && sib.node!=0) {
		LOG(INFO) << "compare " << suffix[counter] << " with " << sib->c << " at " << this->tr.depth(sib);

		if(sib->c == suffix[counter]) {
			int nb_childs = sib.number_of_children();
			LOG(INFO) << "found match, check below, " << "node children " << nb_childs;
            LOG(INFO) << "counter: " << counter << ", suffix: " << strlen(suffix);
			if(counter==suffix_len-1) {
				LOG(INFO) << "no more suffix";
				match = true;
				std::list<long>::iterator end = sib->positions.end();
				sib->positions.insert(end, counter);
			}
			else if(nb_childs >0) {
				LOG(INFO) << "check children";
				// Continue parsing
				sib = tr.begin(sib);
				counter++;
			}
			else {
				LOG(INFO) << "no more child, fill with suffix";
				match = true;
				// Last matching node, fill the rest of the node with current suffix
				this->fillTreeWithSuffix(sib, suffix, counter+1, suffix_len, pos);
			}

		}
		else {
			// Switch to next node
			LOG(INFO) << "no match, compare sibling " << this->tr.depth(sib);
			sib = tr.next_sibling(sib);
		}
	}

	if(!match) {
		char node_char = suffix[counter];
		LOG(INFO) << "No match found, add new node " << node_char ;
		this->fillTreeWithSuffix(NULL, suffix, counter, suffix_len, pos);
	}

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
