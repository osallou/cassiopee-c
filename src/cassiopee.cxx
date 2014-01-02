// A simple program that computes the square root of a number
#include <stdio.h>
#include <glog/logging.h>

#include "Cassiopee.h"
#include "CassiopeeConfig.h"




using namespace std;




InDel::InDel(): in(0),del(0), subst(0), max_indel(0), max_subst(0) {

}

bool InDel::maxReached() {
	return (max_indel > 0 && in+del >= max_indel) || (max_subst > 0 && subst >= max_subst);
}

TreeNode::TreeNode(char nc): c(nc), next(0) {

}

TreeNode::TreeNode(char nc, long pos): c(nc), next(pos) {

}

TreeNode::TreeNode(): c(), next(0) {

}


CassieIndexer::~CassieIndexer() {
	if(this->seqstream) {
		this->seqstream.close();
	}
}

CassieIndexer::CassieIndexer(char* path): filename(path), seqstream(path, ios_base::in | ios_base::binary), matches()
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

void CassieIndexer::getMatchesFromNode(tree<TreeNode>::iterator sib) {

	std::list<long> positions = sib->positions;
	for (std::list<long>::iterator it = positions.begin(); it != positions.end(); it++) {
		  this->matches.insert(this->matches.end(), *it);
	}

	if(sib.number_of_children() > 0) {
	  tree<TreeNode>::iterator leaf_iterator = sib.begin();
	  while(leaf_iterator!= sib.end()) {

			  std::list<long> positions = leaf_iterator->positions;
			  for (std::list<long>::iterator it = positions.begin(); it != positions.end(); it++) {
				  //LOG(INFO) << "match at " << *it;
				  this->matches.insert(this->matches.end(), *it);
			  }

	  ++leaf_iterator;
	  }
	}
}

list<long> CassieIndexer::search(string suffix) {
	this->matches.clear();

	tree<TreeNode>::iterator sib;
	sib = this->tr.begin();

	tree<TreeNode>::iterator last_sibling;
	last_sibling = this->tr.end();

	long counter = 0;

	LOG(INFO) << "Search " << suffix;


	while(sib != last_sibling && sib.node!=0) {
			//LOG(INFO) << "compare " << suffix[counter] << " with " << sib->c << " at " << this->tr.depth(sib);

			if(sib->c == suffix[counter]) {

				int nb_childs = sib.number_of_children();
				//LOG(INFO) << "partial match, check below - " << nb_childs;
				//LOG(INFO) << "filled? " << counter << ":" << suffix.length()-1;
				if(counter == suffix.length()-1) {
					// Exact match, no more char to parse
					// Search leafs
					this->getMatchesFromNode(sib);
					break;
				}


				if(nb_childs > 0) {
					last_sibling = tr.end(sib);
					sib = tr.begin(sib);
					counter++;
				}
				else {
					// No match
					break;
				}
			}
			else {
				//LOG(INFO) << "No match, test sibling";
				sib = tr.next_sibling(sib);
			}
	}

	return this->matches;
}

void searchExact(string suffix, tree<TreeNode>::iterator sib) {
	LOG(FATAL) << "not yet implemented";
}

void searchWithError(string suffix, InDel errors, tree<TreeNode>::iterator sib){
	LOG(FATAL) << "not yet implemented";
}


void CassieIndexer::index() {


	LOG(WARNING) << "I will update this file:" << this->filename;

	LOG(INFO) << "Indexing " << this->filename ;


	for (long i=0; i<this->seq_length-1; i++) {

		long suffix_len = this->seq_length - i - 1;

		char* suffix = new char[suffix_len+1]();

		//LOG(INFO) << "Index suffix " << i;

		this->seqstream.seekg(i, this->seqstream.beg);

		// Extract a suffix
		this->seqstream.read(suffix, suffix_len);

		//LOG(INFO) << "Suffix: "  << suffix << ", length: " << suffix_len;

		this->filltree(suffix, suffix_len, i);

		delete[] suffix;

	}


}

string CassieIndexer::getSuffix(long pos) {
	char* suffix;

    this->seqstream.seekg(pos);
    // Get the rest of the line and print it
    this->seqstream.getline(suffix,this->seq_length-1);
	return suffix;
}

void CassieIndexer::fillTreeWithSuffix(const char* suffix, long suffix_pos, long suffix_len, long pos) {
	tree<TreeNode>::iterator sib;

	char node_char = suffix[0];
	TreeNode* node = new TreeNode(node_char);

	if(suffix_len==1) {
		std::list<long>::iterator end = node->positions.end();
		node->positions.insert(end, pos);
	}

	if(this->tr.begin().number_of_children()==0) {
		sib = this->tr.insert(this->tr.begin(), *node);
		//LOG(INFO) << "add first head element";
	}
	else {
		//LOG(INFO) << "insert new  head element";
		sib = this->tr.insert_after(this->tr.begin(),*node);

	}

	if(suffix_len==1) {
		return;
	}

	this->fillTreeWithSuffix(sib, suffix, suffix_pos+1, suffix_len, pos);
}


void CassieIndexer::fillTreeWithSuffix(tree<TreeNode>::iterator sib, const char* suffix, long suffix_pos, long suffix_len, long pos) {

	for(long i=suffix_pos;i<suffix_len;i++) {
		char node_char = suffix[i];
		TreeNode* node = new TreeNode(node_char);
		if(i==suffix_len-1) {
			// If last node, append position of suffix
			//LOG(INFO) << "add position to node " << pos;
			std::list<long>::iterator end = node->positions.end();
			node->positions.insert(end, pos);
		}
		sib = this->tr.append_child(sib,*node);
	}
}

tree<TreeNode> CassieIndexer::getTree() {
	return this->tr;
}

void CassieIndexer::filltree(const char* suffix, long suffix_len, long pos) {

	tree<TreeNode>::iterator sib;
	tree<TreeNode>::iterator last_sibling;
	tree<TreeNode>::iterator place_to_insert;



	sib = this->tr.begin();
	place_to_insert = this->tr.end();
	last_sibling = this->tr.end();


	bool match = false;
	bool nomore = false;
	bool head = true;

	long counter = 0;

	while(!match  && !nomore) {
		//LOG(INFO) << "compare " << suffix[counter] << " with " << sib->c << " at " << this->tr.depth(sib);

		if(sib->c == suffix[counter]) {
			head = false;
			int nb_childs = sib.number_of_children();
			//LOG(INFO) << "found match, check below, " << "node children " << nb_childs;
            //LOG(INFO) << "counter: " << counter << ", suffix: " << strlen(suffix);
			if(counter==suffix_len-1) {
				//LOG(INFO) << "no more suffix";
				match = true;
				std::list<long>::iterator end = sib->positions.end();
				//LOG(INFO) << "suffix " << suffix << " at " << pos;
				sib->positions.insert(end, pos);

			}
			else if(nb_childs >0) {
				//LOG(INFO) << "check children";
				// Continue parsing
				sib = tr.begin(sib);
				counter++;
			}
			else {
				//LOG(INFO) << "no more child, fill with suffix";
				match = true;
				// Last matching node, fill the rest of the node with current suffix
				this->fillTreeWithSuffix(sib, suffix, counter+1, suffix_len, pos);
				//LOG(INFO) << "suffix " << suffix << " at " << pos;
			}

		}
		else {


			// Switch to next node
			//LOG(INFO) << "no match, compare sibling " << this->tr.depth(sib);
			last_sibling = this->tr.end(sib);
			place_to_insert = sib;
			sib = tr.next_sibling(sib);
			if(sib.node==0) {
				nomore = true;
				//LOG(INFO) << "nomore";
			}

		}
	}

	if(!match) {
		char node_char = suffix[counter];
		if(head) {
			//LOG(INFO) << "No match found, add new node " << node_char << " at head";
			this->fillTreeWithSuffix(suffix, counter, suffix_len, pos);
		}
		else {
			//LOG(INFO) << "No match found, add new node " << node_char << " in tree at " << tr.depth(place_to_insert);

			TreeNode* node = new TreeNode(node_char);
			if(counter==suffix_len-1) {
				// If last node, append position of suffix
				//LOG(INFO) << "add position to node " << pos;
				std::list<long>::iterator end = node->positions.end();
				node->positions.insert(end, pos);
			}
			place_to_insert = this->tr.insert_after(place_to_insert, *node);
			this->fillTreeWithSuffix(place_to_insert, suffix, counter+1, suffix_len, pos);
		}
		//LOG(INFO) << "suffix " << suffix << " at " << pos;
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



  string suffix = "gggc";
  list<long> matches = indexer->search(suffix);
  matches.sort();
  for (std::list<long>::iterator it = matches.begin(); it != matches.end(); it++) {
	  LOG(INFO) << "Match at: " << *it;
  }




  return 0;
}


void testTree() {
	  tree<string> tr;
	  tree<string>::iterator sib;
	  sib = tr.insert(tr.begin(), "hello1");
	  sib = tr.insert_after(tr.begin(),"hello2");
	  sib = tr.insert_after(tr.begin(),"hello3");

	  tree<string>::iterator loop=tr.begin();
	  tree<string>::iterator end=tr.end();
	   while(loop!=end) {
	     cout << (*loop) << endl;
	     ++loop;
	     }

}
