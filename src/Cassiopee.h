#include <iostream>
#include <fstream>

using namespace std;

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
};
