#include <iostream>
#include <fstream>
#include <list>

#include "tree/tree.hh"

using namespace std;


/**
 * Object to manage insertion and deletions
 */
class InDel {
public:

	int in;
	int del;

	int max_in;
	int max_del;

	InDel();

	bool maxReached();


};

/**
 * Node in the suffix tree
 */
class TreeNode {
public:
	/**
	 * Character to match
	 */
	char c;

	int next;

	/**
	 * List of positions in sequence matching this node
	 */
	list<long> positions;
	/**
	 * Creates a node from a char
	 */
	TreeNode(char nc);
	TreeNode(char nc, long pos);
	TreeNode();


private:


};

std::ostream& operator<<(std::ostream &strm, const TreeNode &a) {
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
	string getSuffix(long pos);

	/**
	 * Launch the indexation of the input sequence
	 */
	void index();

	list<long> search(string suffix);

	void getMatchesFromNode(tree<TreeNode>::iterator sib);

	tree<TreeNode> getTree();

	list<long> matches;

private:

	char* filename;
	ifstream seqstream;
	long seq_length;
	tree<TreeNode> tr;

	/**
	 * Fills suffix tree with input suffix
	 *
	 * \param suffix Suffix to insert
	 * \param pos Position of suffix in sequence
	 */
	void filltree(const char* suffix, long suffix_len, long pos);

	/**
	 * Fills a tree branch with the suffix starting at suffix_pos
	 *
	 * \param sib Insert at node sib.
	 * \param suffix Suffix to insert
	 * \param suffix_pos Starting inserting from suffix position index
	 * \param suffix_len length of the suffix (to avoid too many calculations)
	 * \param pos Position of suffix in sequence
	 */
	void fillTreeWithSuffix(tree<TreeNode>::iterator sib, const char* suffix, long suffix_pos, long suffix_len, long pos);
	void fillTreeWithSuffix(const char* suffix, long suffix_pos, long suffix_len, long pos);

};





