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
 *
 * If tree is reduced (DO_REDUCTION=1), memory usage is highly reduced, keeping in memory only the branching nodes.
 * Search is made after that looking at branch nodes and sequence content between nodes.
 * In addition, data from file are analysed per chunk, limiting again loaded data in memory.
 *
 * This treatment requires however more disk usage (reads) and tree manipulations, increasing the index and search time for a gain on memory requirements.
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
	 * When tree is reduced, store the location of next characters in input sequence file.
	 * Reduction can be made from a branch up to the leaf or between 2 branches.
	 * If next_pos is equal to -1, then no reduction is made.
	 */
	long next_pos;

	/**
	 * Length of remaining data to read in sequence file
	 */
	long next_length;


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
	 * Get the tree matching the indexed sequence
	 */
	tree<TreeNode>* getTree();

	/**
	 * List of positions in original sequence matching the search
	 */
	list<long> matches;

	/**
	 * Allow tree reduction
	 */
	bool do_reduction;

	/**
	 * Get char from suffix located at position pos.
	 */
	char getCharAtSuffix(long pos);

private:

	char* filename;
	ifstream seqstream;
	long seq_length;
	tree<TreeNode> tr;

	const long MAX_SUFFIX;
	long suffix_position;
	char* suffix;

	/**
	 * Extract parts of suffix located at pos from stream with a max size of MAX_SUFFIX.
	 */
	char* loadSuffix(long pos);


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

/**
 * Search object
 *
 */
class CassieSearch {

public:
	/**
	 * Main constructor
	 *
	 * \param index_ref Indexer for sequence
	 */
	CassieSearch(CassieIndexer* index_ref);

	/**
	 * List of positions in original sequence matching the search
	 */
	list<long> matches;

	/**
	 * Search for a string in the indexed sequence
	 *
	 * \param suffix pattern to search
	 * \param clear Clear existing matches?
	 * \return list of position in original sequence
	 */
	list<long> search(string suffix, bool clear);


	/**
	 * Search for a string in the indexed sequence.
	 * Clear all previous matches.
	 *
	 * \param suffix pattern to search
	 * \return list of position in original sequence
	 */
	list<long> search(string suffix);

	/**
	 * Search for multiple strings
	 *
	 * \param list of patterns to search
	 * \return list of position in original sequence
	 */
	list<long> search(list<string> suffixes);

	/**
	 * Sets all matching positions in matches from node
	 *
	 * \param sib Base node to search through in the tree
	 */
	void getMatchesFromNode(tree<TreeNode>::iterator sib);

private:
	CassieIndexer* indexer;
};



