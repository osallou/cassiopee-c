#include <iostream>
#include <fstream>
#include <list>

#include "tree/tree.hh"

#include "CassiopeeConfig.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
// Provide an implementation of serialize for std::list
#include <boost/serialization/list.hpp>

using namespace std;




/**
 * Object to manage insertion and deletions
 */
class Match {
public:

	/**
	 * define equality between matches
	 */
	bool operator==(const Match& p) const {
	        return pos == p.pos && in+del == p.in + p.del;
	}

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


	long pos;

	Match();


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
	  friend class boost::serialization::access;
	  template<class Archive>
	  void serialize(Archive & ar, const unsigned int /*version*/)
	  {
	    ar & c;
	    ar & next_pos;
	    ar & next_length;
	    ar & positions;
	  }

};



#ifndef __CASSIOPEE_H_
#define __CASSIOPEE_H_
//inline std::ostream& operator<<(std::ostream &strm, const TreeNode &a) {
//  return strm << "TreeNode(" << a.c << ")";
//}
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
	CassieIndexer(const char* path);

	~CassieIndexer();

	/**
	 * Save the index in a file
	 */
	void save();

	/**
	 * Load index from a file
	 */
	void load();


	/**
	 * Generates a dot file representing the tree.
	 */
	void graph();

	/**
	 * Generates a dot file representing the tree up to depth
	 *
	 * \param depth Max depth to print
	 */
	void graph(int depth);

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
	list<Match> matches;

	/**
	 * Allow tree reduction
	 */
	bool do_reduction;

	/**
	 * Get char from suffix located at position pos.
	 */
	char getCharAtSuffix(long pos);

	/**
	 * Fills suffix tree with input suffix
	 *
	 * \param pos Position of suffix in sequence
	 */
	void filltree(long pos);

	/**
	 * Max depth to index, can limit index size if pattern to search are known of a maximum size
	 */
	long max_depth;

	long seq_length;

	/**
	 * Checks if index has been loaded from a saved index or if index has been build dynamically
	 */
	bool index_loaded_from_file();

private:
	list<TreeNode> serialized_nodes;

	bool loaded_from_file;

	const char* filename;
	ifstream seqstream;
	tree<TreeNode> tr;

	const long MAX_SUFFIX;
	long suffix_position;
	char* suffix;

	/**
	 * Graph the children of node
	 */
	long graphNode(tree<TreeNode>::iterator node, long counter, ofstream& myfile, int maxdepth);

	/**
	 * Extract parts of suffix located at pos from stream with a max size of MAX_SUFFIX.
	 */
	char* loadSuffix(long pos);


	/**
	 * Reset current suffix; Mandatory before calling getCharAtSuffix on a new suffix
	 */
	void reset_suffix();



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
 * Utility functions
 */
class CassiopeeUtils {
public:
	/**
	 * Transform a Fasta input sequence in a sequence manageable by index.
	 */
	static void transform_fasta(const string in, const string out);
};

/**
 * Ambiguity character matches for DNA
 */
class Ambiguous {
public:
	/**
	 * Checks if a char is equal to an other according to ambiguity
	 *
	 * \param a input char
	 * \param b input char
	 * \return true if characters match
	 */
	static bool isequal(char a, char b);
private:
	/**
	 * Compares input character with a list of possible matches
	 *
	 * \param a input char, non ambiguous
	 * \param b list of possible matches for ambiguity
	 * \param len length of b array
	 * \return true if a match one character of b
	 */
	static bool ismatchequal(char a, const char b[], int len);
	static const char K_MATCH[];
	static const char M_MATCH[];
	static const char R_MATCH[];
	static const char Y_MATCH[];
	static const char S_MATCH[];
	static const char W_MATCH[];
	static const char B_MATCH[];
	static const char V_MATCH[];
	static const char H_MATCH[];
	static const char D_MATCH[];
	static const char N_MATCH[];
};


struct per_position
{
    inline bool operator() (const Match* struct1, const Match* struct2)
    {
        return (struct1->pos < struct2->pos);
    }
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

	~CassieSearch();

	/**
	 * Remove matches with same position and same length.
	 */
	void removeDuplicates();

	/**
	 * Used to store max errors allowed
	 */
	Match* match_limits;

	/**
	 * List of positions in original sequence matching the search
	 */
	list<Match*> matches;

	/**
	 * Allow alphabet ambiguity search. False by default.
	 * N are by default ignored. To allow N matching, the nmax attribute must be set
	 */
	bool ambiguity;

	/**
	 * Allow N matching with alphabet ambiguity. nmax defines the maximum number of consecutive N allowed in match.
	 */
	int nmax;

	/**
	 * Search mode:
	 *
	 *  - 0: DNA
	 *  - 1: RNA
	 *  - 2: Protein
	 */
	int mode;

	/**
	 * Search for a string in the indexed sequence
	 *
	 * \param suffix pattern to search
	 * \param clear Clear existing matches?
	 */
	void search(string suffix, bool clear);


	/**
	 * Search for a string in the indexed sequence.
	 * Clear all previous matches.
	 *
	 * \param suffix pattern to search
	 */
	void search(string suffix);

	/**
	 * Search for multiple strings
	 *
	 * \param array of patterns to search
	 */
	void search(string suffixes[]);

	/**
	 * Compare two chars, with ambiguity is option is set
	 */
	bool isequal(char a,char b);


	/**
	 * Maximum number of insertion or deletion
	 */
	int max_indel;
	/**
	 * Maximum number of substitution
	 */
	int max_subst;

    long pattern_length;

	/**
	 * Sort matches according to positions i nascending order
	 */
	void sort();

private:

	/**
	 * Compare two Match pointers
	 */
	static bool same_match (Match* first, Match* second)
	{ return ( *first == *second ); }

	CassieIndexer* indexer;

	/**
	 * Sets all matching positions in matches from node
	 *
	 * \param sib Base node to search through in the tree
	 * \param nbSubst Number of substition found
	 * \param nbIn Number of insertion found
	 * \param nbDel Number of deletion found
	 */
	void getMatchesFromNode(tree<TreeNode>::iterator sib, const int nbSubst, const int nbIn, const int nbDel);



	/**
	 * Compare suffix at a specific start root node (check with root children).
	 *
	 * \param suffix Suffix to analyse
	 * \param suffix_pos position on suffix
	 * \param root Tree node root to analyse, starts with first child
	 * \param nbSubst current number of substitutions found
	 * \param nbIn current number of insertions found
	 * \param nbDel current number of deletions found
	 * \param nbN current consecutive N found
	 */
	void searchAtNode(string suffix, const long suffix_pos, const tree<TreeNode>::iterator root, int nbSubst, int nbIn, int nbDel, int nbN);

	/**
	 * Compare suffix at a specific start root node (check with root children).
	 * \param suffix Suffix to analyse
	 * \param suffix_pos position on suffix
	 * \param root Tree node root to analyse
	 * \param start_node root child node to start with (no first element)
	 * \param nbSubst current number of substitutions found
	 * \param nbIn current number of insertions found
	 * \param nbDel current number of deletions found
	 * \param nbN current consecutive N found
	 */
	void searchAtNode(string suffix, const long suffix_pos, const tree<TreeNode>::iterator root, const tree<TreeNode>::iterator start_node, int nbSubst, int nbIn, int nbDel, int nbN);

	/**
	 * Compare suffix in reducted string at node sib.
	 * \return true if matched
	 */
	bool searchAtreduction(const string suffix, const tree<TreeNode>::iterator sib, long counter, long tree_reducted_pos, int nbSubst, int nbIn, int nbDel, int nbN);


};



