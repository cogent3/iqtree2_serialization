#include "libiqtree2_fun.h"

// Calculates the RF distance between two trees
int calculate_RF_distance(const string& tree1, const string& tree2) {
    // Placeholder implementation
    MTree first_tree;
    bool is_rooted = false;
    std::vector<double> rfdist;

    // read in the first tree
    first_tree.read_TreeString(tree1, is_rooted);
    
    // second tree
    stringstream second_tree_str;
    second_tree_str << tree2;
    second_tree_str.seekg(0, ios::beg);

    // compute the RF distance
    first_tree.computeRFDist(second_tree_str, rfdist);

    return (int)rfdist[0];
}

// Generates a random phylogenetic tree
void generate_random_tree_file(int numtaxa, int seed, string tree_gen_mode, string outfile) {
    PhyloTree ptree;
    
    
    init_random(seed);
    
    TreeGenType genMode;
    if (tree_gen_mode == "YULE_HARDING") {
        genMode = YULE_HARDING;
    } else if (tree_gen_mode == "UNIFORM") {
        genMode = UNIFORM;
    } else if (tree_gen_mode == "CATERPILLAR") {
        genMode = CATERPILLAR;
    } else if (tree_gen_mode == "BALANCED") {
        genMode = BALANCED;
    } else if (tree_gen_mode == "BIRTH_DEATH") {
        genMode = BIRTH_DEATH;
    } else if (tree_gen_mode == "STAR_TREE") {
        genMode = STAR_TREE;
    } else if (tree_gen_mode == "CIRCULAR_SPLIT_GRAPH") {
        genMode = CIRCULAR_SPLIT_GRAPH;
    } else if (tree_gen_mode == "TAXA_SET") {
        genMode = TAXA_SET;
    } else {
        cerr << "Unknown mode: " << tree_gen_mode << endl;
        return;
    }

    Params params = Params::getInstance();
    params.sub_size = numtaxa;
    params.tree_gen = genMode;
    params.user_file = (char *) outfile.c_str();
    
    // default parameter values
    params.repeated_time = 1; // one random tree to generate
    params.max_len = 1.0; // maximum branch length
    params.min_len = 1e-6; // minimum branch length
    params.mean_len = 0.1; // mean branch length
    params.ignore_checkpoint = true; // overrid the output file if exists

    generateRandomTree(params);
}
