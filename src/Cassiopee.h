#include <iostream>
#include <fstream>
#include <list>

#include "tree/tree.hh"

using namespace std;

/**
 * Node in the suffix tree
 */
class TreeNode {
public:
	/**
	 * Character to match
	 */
	char* c;
	/**
	 * List of positions in sequence matching this node
	 */
	list<long> positions;
	/**
	 * Creates a node from a char
	 */
	TreeNode(char* nc);
	TreeNode();

private:


};

std::ostream& operator<<(std::ostream &strm, const TreeNode &a) {
  if(a.c != NULL) {
	  return strm << "TreeNode()";
  }
  return strm << "TreeNode(" << a.c << ")";
}


/**
 * Cassiopee indexer
 *
 * Index an input file
 */
class CassieIndexer {
public:

	/**
	 * Main contructor
	 *
	 * \param path Path to the sequence file, sequence must be a one-line sequence.
	 */
	CassieIndexer(char* path);

	~CassieIndexer();

	/**
	 * Get the suffix content from a position
	 *
	 * \param pos Position in the file
	 * \return the suffix string
	 */
	string getSuffix(int pos);

	/**
	 * Launch the indexation of the input sequence
	 */
	void index();

private:

	char* filename;
	ifstream seqstream;
	int seq_length;

	/**
	 * Fills sufix tree with input suffix
	 */
	void filltree(tree<TreeNode> tr, tree<TreeNode>::iterator top, char* suffix);
};





