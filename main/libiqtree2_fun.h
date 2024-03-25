#include "tree/mtree.h"
#include "tree/phylotree.h"
#include "utils/tools.h"
#include "main/alisim.h"
#include <vector>
#include <string>

using namespace std;

// Calculates the RF distance between two trees
int calculate_RF_distance(const string& tree1, const string& tree2);

// Generates a random phylogenetic tree
void generate_random_tree_file(int numtaxa, int seed, string tree_gen_mode, string outfile);
