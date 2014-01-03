#include <iostream>
#include <fstream>
#include <list>

#include "tree/tree.hh"

#include "CassiopeeConfig.h"

using namespace std;


/**
 * Object to manage insertion and deletions
 */
class InDel {
public:

	/**
	 * Number of insertion error
	 */
	int in;
	/**
	 * Number of deletion error
	 */
	int del;
	/**
	 * Number of substitution error
	 */
	int subst;

	/**
	 * Maximum number of insertion or deletion
	 */
	int max_indel;
	/**
	 * Maximum number of substitution
	 */
	int max_subst;

	InDel();

	/**
	 * Checks if an error threshold has been reached
	 */
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


	/**
	 * List of positions in sequence matching this node
	 */
	list<long> positions;

	/**
	 * Creates a node from a char
	 *
	 * \param nc character for this node
	 */
	TreeNode(char nc);

	/**
	 * Creates a node from a char
	 *
	 * \param nc character for this node
	 * \param pos position of the character in the sequence
	 */
	TreeNode(char nc, long pos);
	TreeNode();


private:


};

#ifndef __CASSIOPEE_H_
#define __CASSIOPEE_H_
inline std::ostream& operator<<(std::ostream &strm, const TreeNode &a) {
  return strm << "TreeNode(" << a.c << ")";
}
#endif


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

	/**
	 * Search for a string in the indexed sequence
	 *
	 * \param suffix pattern to search
	 * \return list of position in original sequence
	 */
	list<long> search(string suffix);

	/*
	void searchExact(string suffix, tree<TreeNode>::iterator sib);
	void searchWithError(string suffix, InDel errors, tree<TreeNode>::iterator sib);
	*/

	/**
	 * Sets all matching positions in matches from node
	 *
	 * \param sib Base node to search through in the tree
	 */
	void getMatchesFromNode(tree<TreeNode>::iterator sib);

	/**
	 * Get the tree matching the indexed sequence
	 */
	tree<TreeNode> getTree();

	/**
	 * List of positions in original sequence matching the search
	 */
	list<long> matches;

private:

	char* filename;
	ifstream seqstream;
	long seq_length;
	tree<TreeNode> tr;

	const long MAX_SUFFIX = SUFFIX_CHUNK_SIZE;
	long suffix_position = -1;
	char* suffix=NULL;

	/**
	 * Extract parts of suffix located at pos from stream with a max size of MAX_SUFFIX.
	 */
	char* loadSuffix(long pos);
	/**
	 * Get char from suffix located at position pos.
	 */
	char getCharAtSuffix(long pos);

	/**
	 * Reset current suffix; Mandatory before calling getCharAtSuffix on a new suffix
	 */
	void reset_suffix();

	/**
	 * Fills suffix tree with input suffix
	 *
	 * \param pos Position of suffix in sequence
	 */
	void filltree(long pos);

	/**
	 * Fills a tree branch with the suffix starting at suffix_pos
	 *
	 * \param sib Insert at node sib.
	 * \param suffix_pos Starting inserting from suffix position index
	 * \param pos Position of suffix in sequence
	 */
	void fillTreeWithSuffix(tree<TreeNode>::iterator sib, long suffix_pos, long pos);
	void fillTreeWithSuffix(long suffix_pos, long pos);



};





