// A simple program that computes the square root of a number
#include <stdio.h>
#include <glog/logging.h>
#include <unistd.h>

#include "Cassiopee.h"

using namespace std;


InDel::InDel(): in(0),del(0), subst(0), max_indel(0), max_subst(0) {

}

bool InDel::maxReached() {
	return (max_indel > 0 && in+del >= max_indel) || (max_subst > 0 && subst >= max_subst);
}

TreeNode::TreeNode(char nc): c(nc), next_pos(-1), next_length(0) {

}

TreeNode::TreeNode(char nc, long pos): c(nc), next_pos(-1), next_length(0) {

}

TreeNode::TreeNode(): c(), next_pos(-1), next_length(0) {

}


CassieSearch::CassieSearch(CassieIndexer* index_ref): indexer(index_ref) {
}

void CassieSearch::getMatchesFromNode(tree<TreeNode>::iterator sib) {
     //LOG(INFO) << "getMatchesFromNode " << sib->c << "," << indexer->getTree()->depth(sib) << ":" << sib.number_of_children();
	std::list<long> positions = sib->positions;
#pragma omp critical(dataupdate)
	for (std::list<long>::iterator it = positions.begin(); it != positions.end(); it++) {
		this->matches.push_back(*it);
	}

	if(sib.number_of_children() > 0) {
	  tree<TreeNode>::iterator leaf_iterator = sib.begin();
	  while(leaf_iterator!= sib.end()) {

			  std::list<long> positions = leaf_iterator->positions;
#pragma omp critical(dataupdate)
			  for (std::list<long>::iterator it = positions.begin(); it != positions.end(); it++) {
				  //LOG(INFO) << "match at " << *it;
				  this->matches.push_back(*it);
			  }

	  ++leaf_iterator;
	  }
	}
}


list<long> CassieSearch::search(list<string> suffixes) {
	this->matches.clear();
#pragma omp parralel for if(use_openmp)
	for (std::list<string>::iterator it = suffixes.begin(); it != suffixes.end(); it++) {
		this->search(*it,false);
	}
	return this->matches;
}

list<long> CassieSearch::search(string suffix) {
	return this->search(suffix,true);
}

list<long> CassieSearch::search(string suffix, bool clear) {
	this->matches.clear();

	tree<TreeNode>* tr = this->indexer->getTree();

	tree<TreeNode>::iterator sib;
	sib = tr->begin();

	tree<TreeNode>::iterator last_sibling;
	last_sibling = tr->end();

	long counter = 0;

	LOG(INFO) << "Search " << suffix;

	char tree_char = sib->c;
	char suffix_char = suffix[counter];

	while(sib != last_sibling && sib.node!=0) {
			//LOG(INFO) << "compare " << suffix_char << " with " << tree_char ;

			if(tree_char == suffix_char) {

				int nb_childs = sib.number_of_children();
				//LOG(INFO) << "partial match, check below - " << nb_childs;
				//LOG(INFO) << "filled? " << counter << ":" << suffix.length()-1;
				if(counter == suffix.length()-1) {
					// Exact match, no more char to parse
					// Search leafs
					this->getMatchesFromNode(sib);
					break;
				}

				else if(sib->next_pos>=0){
					long tree_reducted_pos = -1;
					while(tree_reducted_pos < sib->next_length && suffix_char == tree_char) {
						counter++;
						suffix_char = suffix[counter];
						tree_reducted_pos++;
						tree_char = this->indexer->getCharAtSuffix(sib->next_pos+tree_reducted_pos);
						//LOG(INFO) << "match " << suffix_char << " with " << tree_char;
					}
				}

				else if(nb_childs > 0) {
					last_sibling = tr->end(sib);
					sib = tr->begin(sib);
					tree_char = sib->c;
					counter++;
					suffix_char = suffix[counter];
				}
				else {
					// No match
					break;
				}
			}
			else {
				//LOG(INFO) << "No match, test sibling";
				sib = tr->next_sibling(sib);
				if(sib.node != 0) {
				tree_char = sib->c;
				}
				else {
					tree_char = '\0';
				}
			}
	}

	return this->matches;
}


CassieIndexer::~CassieIndexer() {
	if(this->seqstream) {
		this->seqstream.close();
	}
	delete[] this->suffix;
}

CassieIndexer::CassieIndexer(char* path): do_reduction(false), filename(path), seqstream(path, ios_base::in | ios_base::binary), matches(), MAX_SUFFIX(SUFFIX_CHUNK_SIZE),suffix_position(-1), suffix(NULL)
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


char CassieIndexer::getCharAtSuffix(long pos) {

	assert(pos < this->seq_length && pos>=0);
    //LOG(INFO) << "getchar " << this->suffix_position << " < " << pos << " <= " << this->suffix_position + this->MAX_SUFFIX;
	if(this->suffix_position >= 0 && pos >= this->suffix_position && pos< this->suffix_position + this->MAX_SUFFIX ) {
		//LOG(INFO) << pos - this->suffix_position << ": " << this->suffix[pos - this->suffix_position];
		return this->suffix[pos - this->suffix_position];
	}
	else {
		//LOG(INFO) << "Extract new suffix chunk at " << pos;
		// Extract new suffix part
		this->loadSuffix(pos);
		return this->suffix[pos - this->suffix_position];
	}

}

char* CassieIndexer::loadSuffix(long pos)  {

	// Set initial position
	this->suffix_position = pos;

	assert(pos < this->seq_length);

	long suffix_len = min(this->MAX_SUFFIX,this->seq_length - pos - 1);

	//char* suffix = new char[suffix_len+1]();
	delete[] this->suffix;
	this->suffix = new char[suffix_len+1]();

	this->seqstream.seekg(pos, this->seqstream.beg);

	// Extract a suffix
	this->seqstream.read(this->suffix, suffix_len);

    //LOG(INFO) << "Load suffix chunk "<< suffix;
	//this->suffix = suffix;



	return this->suffix;
}

void CassieIndexer::reset_suffix() {
	this->suffix_position = -1;
}

void CassieIndexer::index() {

	LOG(INFO) << "Indexing " << this->filename ;


	for (long i=0; i<this->seq_length-1; i++) {
		this->filltree(i);
	}


}

string CassieIndexer::getSuffix(long pos) {

	char* suffix;

    this->seqstream.seekg(pos);
    // Get the rest of the line and print it
    this->seqstream.getline(suffix,this->seq_length-1);
	return suffix;
}

void CassieIndexer::fillTreeWithSuffix(long suffix_pos, long pos) {

	tree<TreeNode>::iterator sib;

	char node_char = this->getCharAtSuffix(pos+suffix_pos);
	long suffix_len = this->seq_length - pos - 1;

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

	delete node;

	if(suffix_len==1) {
		return;
	}

	this->fillTreeWithSuffix(sib, suffix_pos+1, pos);

}


void CassieIndexer::fillTreeWithSuffix(tree<TreeNode>::iterator sib, long suffix_pos, long pos) {


	long suffix_len = this->seq_length - pos - 1;

	for(long i=suffix_pos;i<suffix_len;i++) {
		//char node_char = suffix[i];
		char node_char = this->getCharAtSuffix(pos+i);
		if(node_char == '\0') {
			break;
		}
		//LOG(INFO) << "add char " << node_char << " from pos " << pos << " at " << pos+i << ", l= " << suffix_len;
		TreeNode* node = new TreeNode(node_char);
		if(i==suffix_len-1) {
			// If last node, append position of suffix
			//LOG(INFO) << "add position to node " << pos;
			node->positions.push_back(pos);
		}
		sib = this->tr.append_child(sib,*node);
		delete node;


		if(this->do_reduction) {
			sib->next_pos = pos + i + 1;
			sib->next_length = suffix_len - 1;
			//LOG(INFO) << "add position to node " << pos;
			if(i!=suffix_len-1) {
				// Position already added, this is a special case
				sib->positions.push_back(pos);
			}
			//LOG(INFO) << "apply reduction: " << sib->next_pos << "," << sib->next_length;
			break;
		}

	}
}

tree<TreeNode>* CassieIndexer::getTree() {
	return &(this->tr);
}

void CassieIndexer::filltree(long pos) {




	tree<TreeNode>::iterator sib;
	tree<TreeNode>::iterator last_sibling;
	tree<TreeNode>::iterator place_to_insert;

	long suffix_len = this->seq_length - pos - 1;


	//LOG(INFO) << "new suffix " << pos << " l= " << suffix_len;


	sib = this->tr.begin();
	place_to_insert = this->tr.end();
	last_sibling = this->tr.end();


	bool match = false;
	bool nomore = false;
	bool head = true;

	long counter = 0;

	char tree_char = sib->c;
	/*
	 * In case of tree reduction, position of char in next_pos characters
	 */
	long tree_reducted_pos = -1;

	while(!match  && !nomore) {

		char suffix_char = this->getCharAtSuffix(pos+counter);


		//LOG(INFO) << "compare " << suffix_char << " with " << tree_char << " at " << pos+counter << ", d: " << this->tr.depth(sib);


		if(tree_char == suffix_char) {
			head = false;
			int nb_childs = sib.number_of_children();
			//LOG(INFO) << "found match, check below, " << "node children " << nb_childs;
            //LOG(INFO) << "counter: " << counter << ", suffix: " << strlen(suffix);
			if(counter==suffix_len-1 && sib->next_pos<0) {
				//LOG(INFO) << "no more suffix ";
				match = true;
				tree_reducted_pos = -1;
				//LOG(INFO) << "suffix " << suffix << " at " << pos << ", d=" << tr.depth(sib);
				sib->positions.push_back(pos);
			}
			else if(sib->next_pos>=0 && tree_reducted_pos < sib->next_length) {
				while(tree_reducted_pos < sib->next_length && suffix_char == tree_char) {

					if(counter == suffix_len - 1) {
						// Match before parsing all chars
						// Insert a node here
						match = true;
						this->seqstream.seekg(sib->next_pos + tree_reducted_pos + 1, this->seqstream.beg);
						this->seqstream.read(&tree_char, 1);
		            	TreeNode * tmp_node = new TreeNode(tree_char);
		            	tmp_node->next_length = sib->next_length - tree_reducted_pos - 1;
		            	tmp_node->next_pos = sib->next_pos +  tree_reducted_pos + 1;
		            	sib->next_length = tree_reducted_pos ;
		            	tree<TreeNode>::iterator old = sib;
		            	//sib->positions.push_back(pos);
		            	sib = this->tr.append_child(sib, *tmp_node);
		            	for (std::list<long>::iterator it = old->positions.begin(); it != old->positions.end(); it++) {
		            		sib->positions.push_back(*it);
		  			  	}

		            	old->positions.clear();
		            	old->positions.push_back(pos);
		            	delete tmp_node;
		            	tree_reducted_pos = -1;

						break;
					}

					tree_reducted_pos++;
					counter++;
					suffix_char = this->getCharAtSuffix(sib->next_pos+counter);
					// Tree reduction case and refer to chars in sequence
					this->seqstream.seekg(sib->next_pos+tree_reducted_pos, this->seqstream.beg);
					// Extract a suffix
					this->seqstream.read(&tree_char, 1);
					//LOG(INFO) << "take next char in reduction: " << tree_char << " at " << sib->next_pos+tree_reducted_pos;
				}
				place_to_insert = sib;
				break;

			}
			else if(nb_childs >0) {
				//LOG(INFO) << "check children";
				// Continue parsing

				// reset anyway tree_reducted_pos
				tree_reducted_pos = -1;

				// Look at children now
				sib = tr.begin(sib);
				tree_char = sib->c;

				counter++;
			}
			else {
				//LOG(INFO) << "no more child, fill with suffix";
				match = true;
				tree_reducted_pos = -1;
				// Last matching node, fill the rest of the node with current suffix
				this->fillTreeWithSuffix(sib, counter+1, pos);
				//LOG(INFO) << "suffix " << suffix << " at " << pos;
			}

		}
		else {

			tree_reducted_pos = -1;
			// Switch to next node
			//LOG(INFO) << "no match, compare sibling " << this->tr.depth(sib);
			last_sibling = this->tr.end(sib);
			place_to_insert = sib;
			sib = tr.next_sibling(sib);
			if(sib.node==0) {
				nomore = true;
				//LOG(INFO) << "nomore";
			}
			else {
				tree_char = sib->c;
			}

		}
	}

	if(!match) {
		char node_char = this->getCharAtSuffix(pos+counter);
		//char node_char = suffix[counter];
		if(head) {
			//LOG(INFO) << "No match found, add new node " << node_char << " at head";
			this->fillTreeWithSuffix(counter, pos);
		}
		else {
			//LOG(INFO) << "No match found, add new node " << node_char << " in tree at " << tr.depth(place_to_insert);

			if(tree_reducted_pos > -1) {
            	// Add a new child matching last matched character
				this->seqstream.seekg(place_to_insert->next_pos + tree_reducted_pos, this->seqstream.beg);
				this->seqstream.read(&tree_char, 1);

				//LOG(INFO) << "add new node " << tree_char << ":" << place_to_insert->next_pos << ":" <<  tree_reducted_pos << " in tree at " << tr.depth(place_to_insert) << "," << place_to_insert->c;
				//LOG(INFO) <<"insert "<<node_char;

            	TreeNode * tmp_node = new TreeNode(tree_char);
            	tree<TreeNode>::iterator old = place_to_insert;
            	tmp_node->next_length = place_to_insert->next_length - tree_reducted_pos - 1;
            	tmp_node->next_pos = place_to_insert->next_pos +  tree_reducted_pos + 1;
            	place_to_insert->next_length = tree_reducted_pos ;
            	place_to_insert = this->tr.append_child(place_to_insert, *tmp_node);
            	for (std::list<long>::iterator it = old->positions.begin(); it != old->positions.end(); it++) {
            		place_to_insert->positions.push_back(*it);
  			  	}
            	old->positions.clear();


            	delete tmp_node;
            }

			tree_reducted_pos = -1;
			TreeNode* node = new TreeNode(node_char);

			place_to_insert = this->tr.insert_after(place_to_insert, *node);
			if(counter==suffix_len-1) {
				// If last node, append position of suffix
				place_to_insert->positions.push_back(pos);
			}
			else {
				this->fillTreeWithSuffix(place_to_insert, counter+1, pos);
			}
			delete node;
		}
		//LOG(INFO) << "suffix " << suffix << " at " << pos;
	}

}


