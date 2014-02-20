// A simple program that computes the square root of a number
#include <stdio.h>
#include <glog/logging.h>
#include <unistd.h>

#include "Cassiopee.h"

using namespace std;


Match::Match(): in(0),del(0), subst(0), pos(-1) {

}


TreeNode::TreeNode(char nc): c(nc), next_pos(-1), next_length(0) {

}

TreeNode::TreeNode(char nc, long pos): c(nc), next_pos(-1), next_length(0) {

}

TreeNode::TreeNode(): c(), next_pos(-1), next_length(0) {

}

const char Ambiguous::K_MATCH[] = { 'g', 't', 'u' };
const char Ambiguous::M_MATCH[] = { 'a', 'c' };
const char Ambiguous::R_MATCH[] = { 'a', 'g' };
const char Ambiguous::Y_MATCH[] = { 'c', 't', 'u' };
const char Ambiguous::S_MATCH[] = { 'c', 'g' };
const char Ambiguous::W_MATCH[] = { 'a', 't', 'u' };
const char Ambiguous::B_MATCH[] = { 'c', 'g', 't', 'u' };
const char Ambiguous::V_MATCH[] = { 'a', 'c', 'g' };
const char Ambiguous::H_MATCH[] = { 'a', 'c', 't', 'u' };
const char Ambiguous::D_MATCH[] = { 'a', 'g', 't', 'u' };
const char Ambiguous::N_MATCH[] = { 'a', 'c', 'g', 't', 'u' };

bool Ambiguous::ismatchequal(char a, const char b[],int len) {
	for(int i=0;i<len;i++) {
		if(a==b[i]) {
			return true;
		}
	}
	return false;
}

bool Ambiguous::isequal(char a, char b) {
	char ambi,nonambi;
	if(a=='a' || a=='c' || a=='g' || a=='t' || a=='u') {
		// ambiguity is on input b
		ambi = b;
		nonambi = a;
	}
	else {
		// ambiguity is on input a
		ambi = a;
		nonambi = b;
	}

	switch(ambi) {
	case 'k':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::K_MATCH, 3);
	case 'm':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::M_MATCH, 2);
	case 'r':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::R_MATCH, 2);
	case 'y':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::Y_MATCH, 3);
	case 's':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::S_MATCH, 2);
	case 'w':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::W_MATCH, 3);
	case 'b':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::B_MATCH, 4);
	case 'v':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::V_MATCH, 3);
	case 'h':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::H_MATCH, 4);
	case 'd':
		return Ambiguous::ismatchequal(nonambi,Ambiguous::D_MATCH, 4);
	case 'n':
		return true;
	}
	return false;
}

CassieSearch::CassieSearch(CassieIndexer* index_ref): indexer(index_ref), ambiguity(false), nmax(0), mode(0), max_subst(0), max_indel(0) {
	match_limits = new Match();
}

CassieSearch::~CassieSearch() {
	// Delete matches
	while(!this->matches.empty()) delete this->matches.front(), this->matches.pop_front();
	delete match_limits;
}

bool CassieSearch::isequal(char a, char b) {

	if(this->mode!=2 && (a=='n' || 'b'=='n')) {
		// No N support for DNA/RNA
		return false;
	}

	if(a==b) {
		return true;
	}
	if(this->ambiguity && this->mode!=2) {
		return Ambiguous::isequal(a,b);
	}
	else {
		return a==b;
	}
}

void CassieSearch::getMatchesFromNode(tree<TreeNode>::iterator sib, const int nbSubsts, const int nbIn, const int nbDel) {
    //LOG(INFO) << "getMatchesFromNode " << sib->c << "," << indexer->getTree()->depth(sib) << ":" << sib.number_of_children();
	std::list<long> positions = sib->positions;

#pragma omp critical(dataupdate)
	for (std::list<long>::iterator it = positions.begin(); it != positions.end(); it++) {
		Match* match = new Match();
		match->subst = nbSubsts;
		match->in = nbIn;
		match->del = nbDel;
		//LOG(INFO) << "match1 at " << *it;
		match->pos = *it;
		this->matches.push_back(match);
		//delete match;
	}

	if(sib.number_of_children() > 0) {
	  tree<TreeNode>::iterator leaf_iterator = sib.begin();
	  while(leaf_iterator!= sib.end()) {

			  std::list<long> positions = leaf_iterator->positions;
#pragma omp critical(dataupdate)
			  for (std::list<long>::iterator it = positions.begin(); it != positions.end(); it++) {
				  //LOG(INFO) << "match2 at " << *it;
				  Match* match = new Match();
				  match->subst = nbSubsts;
				  match->in = nbIn;
				  match->del = nbDel;
				  match->pos = *it;
				  this->matches.push_back(match);
				  //delete match;
			  }

	  ++leaf_iterator;
	  }

	}
}

void CassieSearch::sort() {
	this->matches.sort(per_position());
}


void CassieSearch::search(string suffixes[]) {
	this->matches.clear();
#pragma omp parallel for if(use_openmp)
	for (int it = 0; it < suffixes->size(); it++) {
		this->search(suffixes[it],false);
	}

}

void CassieSearch::search(string suffix) {
	this->search(suffix,true);
}

void CassieSearch::search(string suffix, bool clear) {
	transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
	if(clear) {
		this->matches.clear();
	}

	tree<TreeNode>* tr = this->indexer->getTree();

	// Search from root
	this->searchAtNode(suffix, 0, NULL, 0, 0, 0, 0);

}

bool CassieSearch::searchAtreduction(const string suffix, const tree<TreeNode>::iterator sib, long counter, long tree_reducted_pos, int nbSubst, int nbIn, int nbDel, int nbN) {

	//LOG(INFO) << "SEARCH AT REDUCTION";
	char tree_char;
	char suffix_char;
	bool isequal = true;
	while(counter < suffix.length() -1 && tree_reducted_pos < sib->next_length - 1 && isequal && sib->next_pos+tree_reducted_pos <  this->indexer->seq_length - 1 ) {

		counter++;
		suffix_char = suffix[counter];
		tree_reducted_pos++;

		if(this->max_indel > 0 && nbIn+nbDel < this->max_indel) {
			//LOG(INFO) << "CALL REDUCTION AGAIN";
			this->searchAtreduction(suffix, sib, counter, tree_reducted_pos+1, nbSubst, nbIn+1, nbDel, nbN);
			this->searchAtreduction(suffix, sib, counter+1, tree_reducted_pos, nbSubst, nbIn, nbDel+1, nbN);

		}

		tree_char = this->indexer->getCharAtSuffix(sib->next_pos+tree_reducted_pos);
		//LOG(INFO) << "match " << suffix_char << " with " << tree_char << " at " << tree_reducted_pos << ", max=" << sib->next_length;

		bool isequal = this->isequal(tree_char,suffix_char);
		if(!isequal) {
			// If DNA/RNA and tree matches a N, check on max consecutive N allowed
			if(this->mode!=2 && tree_char == 'n') {
				nbN++;
				if(nbN <= this->nmax) {
					isequal = true;
				}
			}
			else {
				nbN = 0;
			}
		}

		if(!isequal && this->max_subst>0 && nbSubst < this->max_subst) {
			// Check for substitutions
			isequal = true;
			nbSubst++;
		}
	}
	if(isequal) {
		if(counter == suffix.length() -1) {
		this->getMatchesFromNode(sib, nbSubst, nbIn, nbDel);
		return true;
		}
		else {
			//LOG(INFO) << "full match but not complete, search childs";
			// TODO complete match but suffix not over, should look at childs now
			this->searchAtNode(suffix, counter+1, sib, NULL, nbSubst, nbIn, nbDel, nbN);

		}
	}
	return false;

}


void CassieSearch::removeDuplicates() {
	// TODO deletion of elements, as we manage pointers, list does not free memory for elements
	  matches.unique(CassieSearch::same_match);
}

void CassieSearch::searchAtNode(string suffix, const long suffix_pos, const tree<TreeNode>::iterator root, int nbSubst, int nbIn, int nbDel, int nbN) {
	this->searchAtNode(suffix, suffix_pos, root, NULL, nbSubst, nbIn, nbDel, nbN);
}

void CassieSearch::searchAtNode(string suffix, const long suffix_pos, const tree<TreeNode>::iterator root, const tree<TreeNode>::iterator start_node,int nbSubst, int nbIn, int nbDel, int nbN) {

	//LOG(INFO) << "searchAtNode" << suffix_pos << ", " << nbSubst;

	if(root!=NULL && root.number_of_children()==0) {
		return;
	}

	tree<TreeNode>* tr = this->indexer->getTree();

	tree<TreeNode>::iterator sib;
	tree<TreeNode>::iterator last_sibling;
	tree<TreeNode>::iterator parentnode = root;

	if(root==NULL) {

		if(start_node == NULL) {
			sib = tr->begin();
		}
		else {
			sib = start_node;
		}
		last_sibling = tr->end();
	}
	else {
		if(start_node == NULL) {
			sib = tr->begin(root);
		}
		else {
			sib = start_node;
		}
		last_sibling = tr->end(root);
	}

	long counter = suffix_pos;

	char tree_char = sib->c;
	char suffix_char = suffix[counter];

	while(sib != last_sibling && sib.node!=0) {


			if(this->max_indel > 0 && nbIn+nbDel < this->max_indel) {
				//LOG(INFO) << "Check for indel, cur= " << sib->c;
				// Move on suffix, keep same base node
				this->searchAtNode(suffix, counter+1, parentnode, sib, nbSubst, nbIn, nbDel+1, nbN);

				// Go to childs, skip one element
				tree<TreeNode>::iterator start,end;
				if(parentnode!=NULL) {
					start = tr->begin(parentnode);
					end = tr->end(parentnode);
				}
				else {
					start = tr->begin();
					end = tr->end();
				}
				while(start.node!=0 && start!=end) {
					//LOG(INFO) << "skip " << start->c << "," << tr->depth(start);
					this->searchAtNode(suffix, counter, start, nbSubst, nbIn+1, nbDel, nbN);
					start = tr->next_sibling(start);
				}


			}

		    //LOG(INFO) << *sib << "," << *last_sibling;
			//LOG(INFO) << "compare " << suffix_char << " with " << tree_char  << ", " << counter << "," << tr->depth(sib);

			bool isequal = this->isequal(tree_char,suffix_char);
			if(!isequal) {
				// If DNA/RNA and tree matches a N, check on max consecutive N allowed
				if(this->mode!=2 && tree_char == 'n') {

					if(this->nmax>0 && nbN < this->nmax) {
						if(counter == suffix.length()-1) {
							// Last character, allow it
							this->getMatchesFromNode(sib, nbSubst, nbIn, nbDel);
						}
						else {
							// Allow substitutions, so try child
							this->searchAtNode(suffix, counter+1, sib, nbSubst, nbIn, nbDel, nbN+1);
						}
					}
				}
			}

			if(isequal) {
				//LOG(INFO) << "compare " << suffix_char << " with " << tree_char  << ", " << counter << "," << tr->depth(sib);

				int nb_childs = sib.number_of_children();
				//LOG(INFO) << "partial match, check below - " << nb_childs;
				//LOG(INFO) << "filled? " << counter << ":" << suffix.length()-1;
				if(counter == suffix.length()-1) {
					// Exact match, no more char to parse
					// Search leafs
					this->getMatchesFromNode(sib, nbSubst, nbIn, nbDel);

					if(this->max_subst>0 && nbSubst < this->max_subst) {
						// If one last substitution is allowed, also check with remaining siblings
						sib = tr->next_sibling(sib);
						while(sib.node!=0) {
							this->getMatchesFromNode(sib, nbSubst+1, nbIn, nbDel);
							sib = tr->next_sibling(sib);
						}

					}
					break;


				}
				else if(sib->next_pos>=0){
					//LOG(INFO) << "next " << sib->next_pos << ", " << sib->next_length;
					long tree_reducted_pos = -1;
					bool matched =  this->searchAtreduction(suffix, sib, counter, tree_reducted_pos, nbSubst, nbIn, nbDel, nbN);
					break;


				}
				else if(nb_childs > 0) {
					last_sibling = tr->end(sib);
					parentnode = sib;
					sib = tr->begin(sib);
					//last_sibling = tr->end(sib);
					tree_char = sib->c;
					counter++;
					suffix_char = suffix[counter];
				}
				else {
					break;
				}
			}
			else {
				if(this->max_subst>0 && nbSubst < this->max_subst) {
					if(counter == suffix.length()-1) {
						// Last character, allow it
						this->getMatchesFromNode(sib, nbSubst+1, nbIn, nbDel);
					}
					else {
						// Allow substitutions, so try child
						this->searchAtNode(suffix, counter+1, sib, nbSubst+1, nbIn, nbDel, nbN);
					}
				}

				// anyway, test siblings

				sib = tr->next_sibling(sib);

				if(sib.node != 0) {
					tree_char = sib->c;
				}
				else {
					tree_char = '\0';
				}
			}
	}

}


CassieIndexer::~CassieIndexer() {
	if(this->seqstream) {
		this->seqstream.close();
	}
	delete[] this->suffix;
}

CassieIndexer::CassieIndexer(char* path): max_depth(0),do_reduction(false), filename(path), seqstream(path, ios_base::in | ios_base::binary), matches(), serialized_nodes(), MAX_SUFFIX(SUFFIX_CHUNK_SIZE),suffix_position(-1), suffix(NULL)
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
		return tolower(this->suffix[pos - this->suffix_position]);
	}
	else {
		//LOG(INFO) << "Extract new suffix chunk at " << pos;
		// Extract new suffix part
		this->loadSuffix(pos);
		return tolower(this->suffix[pos - this->suffix_position]);
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

	*this->suffix = tolower(*this->suffix);

    //LOG(INFO) << "Load suffix chunk "<< suffix;
	//this->suffix = suffix;



	return this->suffix;
}

void CassieIndexer::reset_suffix() {
	this->suffix_position = -1;
}


void CassieIndexer::graph() {
	this->graph(0);
}

void CassieIndexer::graph(int depth) {
	  ofstream myfile;
	  myfile.open ("cassiopee.dot");
	  int counter = 0;
	  myfile << "digraph cassiopee {\n";
	  myfile << "node" << 0 << " [label=\"head\"];\n";

	  this->graphNode(NULL, counter, myfile, depth);
	  myfile << "}\n";
	  myfile.close();
}

long CassieIndexer::graphNode(tree<TreeNode>::iterator node, long parent, ofstream& myfile, int maxdepth) {
	tree<TreeNode>::iterator first;
	tree<TreeNode>::iterator last;
	if(node == NULL) {
		 first = tr.begin();
		 last = tr.end();
	}
	else {
		first= node.begin();
		last= node.end();
	}
	long child = parent+1;
	while(first!=last && first.node!=0) {
		//char* next_chars = new char[first->next_length];
		//this->seqstream.seekg(first->next_pos, this->seqstream.beg);
		//this->seqstream.read(next_chars, first->next_length);
		myfile << "node" << parent << " -> " << "node" << child << ";\n";
		myfile << "node" << child << " [label=\"" << first->c << "-" << first->next_length  << "\"];\n";
		if(first.number_of_children()>0 && (maxdepth==0 || tr.depth(first) < maxdepth)) {
		  child = this->graphNode(first, child, myfile, maxdepth);
		}
		child++;
		first = tr.next_sibling(first);
	}
    return child;
}

void CassieIndexer::index() {

	string index_file = string(this->filename)+".cass.idx";
	if (ifstream(index_file.c_str()))
	{
	     LOG(INFO) << "Index file exists, loading it" << std::endl;
	     this->load();
	     return;
	}

	if(!(this->tr).empty()) {
		DLOG(INFO) << "Indexed already filled";
		return;
	}

	DLOG(INFO) << "Indexing " << this->filename ;


	for (long i=0; i<this->seq_length-1; i++) {
		this->filltree(i);
	}

}



void CassieIndexer::save() {
	if((this->tr).empty()) {
		cerr << "Index is empty" << endl;
	}

	tree<TreeNode>::iterator node = this->tr.begin();
	tree<TreeNode>::iterator end = this->tr.end();
	int depth;
	while(node!=end) {
		depth = tr.depth(node);
        // write class instance to archive
		TreeNode ar_node = node.node->data;
        this->serialized_nodes.push_back(ar_node);


		++node;
		// If we go up, we have finished this level
		if(tr.depth(node) <= depth ) {
			for(int i = tr.depth(node);i <= depth; i++){

				TreeNode end_node = TreeNode('0');
				this->serialized_nodes.push_back(end_node);
			}
		}
	}

	//DLOG(INFO) << "Number of nodes: " << this->serialized_nodes.size();

	string index_file = string(this->filename)+".cass.idx";
	ofstream ofs(index_file.c_str());
	boost::archive::text_oarchive oa(ofs);

	oa << this->serialized_nodes;
	ofs.close();
	this->serialized_nodes.clear();
}


void CassieIndexer::load() {
    // create and open an archive for input
	string index_file = string(this->filename)+".cass.idx";
    ifstream ifs(index_file.c_str());
    boost::archive::text_iarchive ia(ifs);


	tree<TreeNode>::iterator sib;
	sib = this->tr.begin();
	int depth = 0;

    ia >> this->serialized_nodes;
    for (std::list<TreeNode>::const_iterator iterator = this->serialized_nodes.begin(), end = this->serialized_nodes.end(); iterator != end; ++iterator) {
    	if(iterator->c != '0') {
    		if(depth==0) {
    			//cout << "insert child";
    			sib = this->tr.insert(this->tr.begin(), *iterator);
    		}
    		else {
    			//cout << "append child";
    			sib = this->tr.append_child(sib, *iterator);
    		}
    		depth++;
    	}
    	else {
    		depth--;
    		// Should be back to parent
    		sib = sib.node->parent;
    	}
    }
    this->serialized_nodes.clear();

    DLOG(INFO) << "Loaded " << this->getTree()->size() << " nodes";

    // archive and stream closed when destructors are called
}

string CassieIndexer::getSuffix(long pos) {

	char* suffix;

    this->seqstream.seekg(pos);
    // Get the rest of the line and print it
    this->seqstream.getline(suffix,this->seq_length-1);
    *suffix = tolower(*suffix);
	return suffix;
}

void CassieIndexer::fillTreeWithSuffix(long suffix_pos, long pos) {

	tree<TreeNode>::iterator sib;

	char node_char = this->getCharAtSuffix(pos+suffix_pos);
	long suffix_len = this->seq_length - (pos+suffix_pos) -1;

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
	long suffix_len = this->seq_length - pos -1 ;

	for(long i=suffix_pos;i<suffix_len;i++) {
		//char node_char = suffix[i];
		char node_char = this->getCharAtSuffix(pos+i);
		if(node_char == '\0') {
			//LOG(INFO) << "break";
			break;
		}
		//LOG(INFO) << "add char " << node_char << " from pos " << pos <<  "to" << pos + suffix_len << " at " << pos+i << ", l= " << suffix_len;
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
			//LOG(INFO) << "next_pos " << sib->next_pos;
			//LOG(INFO) << "next_length " << sib->next_length;
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
						tree_char = tolower(tree_char);
		            	TreeNode * tmp_node = new TreeNode(tree_char);
		            	tmp_node->next_length = sib->next_length - tree_reducted_pos - 2;
		            	tmp_node->next_pos = sib->next_pos +  tree_reducted_pos + 1;
		            	sib->next_length = tree_reducted_pos ;
		            	if(sib->next_length<=0) {
		            		sib->next_pos = -1;
		            		sib->next_length = 0;
		            	}
		            	if(tmp_node->next_length<=0) {
		            		tmp_node->next_pos = -1;
		            		tmp_node->next_length = 0;
		            	}
		            	tree<TreeNode>::iterator old = sib;

		            	if(sib.number_of_children()>0) {
		            		LOG(INFO) << "insert node between children";
		            		// Move children to new node
		            		tree_node_<TreeNode> *first = sib.node->first_child;
		            		tree_node_<TreeNode> *last = sib.node->last_child;
		            		sib.node->first_child=0;
		            		sib.node->last_child=0;
		            		sib = this->tr.append_child(sib, *tmp_node);
		            		sib.node->first_child = first;
		            		sib.node->last_child = last;
		            		sib.node->first_child->parent=sib.node;
		            		sib.node->last_child->parent=sib.node;
		            	}
		            	else {
		            		sib = this->tr.append_child(sib, *tmp_node);
		            	}

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
					suffix_char = this->getCharAtSuffix(pos+counter);
					// Tree reduction case and refer to chars in sequence
					this->seqstream.seekg(sib->next_pos+tree_reducted_pos, this->seqstream.beg);
					// Extract a suffix
					this->seqstream.read(&tree_char, 1);
					tree_char = tolower(tree_char);
					//LOG(INFO) << "take next char in reduction: " << tree_char << " at " << sib->next_pos+tree_reducted_pos;
				}
				place_to_insert = sib;
				break;

			}
			else if(nb_childs >0 && (this->max_depth==0 || tr.depth(sib) < this->max_depth)) {
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
				//LOG(INFO) << "no more";
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
				tree_char = tolower(tree_char);

				//LOG(INFO) << "add new node " << tree_char << ":" << place_to_insert->next_pos << ":" <<  tree_reducted_pos << " in tree at " << tr.depth(place_to_insert) << "," << place_to_insert->c;
				//LOG(INFO) <<"insert "<<node_char << "," << tree_reducted_pos;

            	TreeNode * tmp_node = new TreeNode(tree_char);
            	tree<TreeNode>::iterator old = place_to_insert;
            	tmp_node->next_length = place_to_insert->next_length - tree_reducted_pos - 2;
            	tmp_node->next_pos = place_to_insert->next_pos +  tree_reducted_pos + 1;
            	place_to_insert->next_length = tree_reducted_pos ;
            	if(place_to_insert->next_length<=0) {
            		place_to_insert->next_pos = -1;
            		place_to_insert->next_length = 0;
            	}
            	if(tmp_node->next_length<=0) {
            		tmp_node->next_pos = -1;
            		tmp_node->next_length = 0;
            	}


            	if(place_to_insert.number_of_children()>0) {
            		//LOG(INFO) << "- insert node between children";
            		// Move children to new node
            		tree_node_<TreeNode> *first = place_to_insert.node->first_child;
            		tree_node_<TreeNode> *last = place_to_insert.node->last_child;
            		place_to_insert.node->first_child=0;
            		place_to_insert.node->last_child=0;
            		place_to_insert = this->tr.append_child(place_to_insert, *tmp_node);
            		place_to_insert.node->first_child = first;
            		place_to_insert.node->last_child = last;
            		place_to_insert.node->first_child->parent=place_to_insert.node;
            		place_to_insert.node->last_child->parent=place_to_insert.node;
            	}
            	else {
            		place_to_insert = this->tr.append_child(place_to_insert, *tmp_node);
            	}

            	//place_to_insert = this->tr.append_child(place_to_insert, *tmp_node);
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


