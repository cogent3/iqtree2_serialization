/***************************************************************************
 *   Copyright (C) 2009-2015 by                                            *
 *   BUI Quang Minh <minh.bui@univie.ac.at>                                *
 *   Lam-Tung Nguyen <nltung@gmail.com>                                    *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TOOLS_H
#define TOOLS_H

#include <iqtree_config.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <stdint.h>
#include <string.h>
#include <sstream>
#include <random>

//#include <sys/time.h>
//#include <time.h>
#include <sys/stat.h>
//#include <math.h>
#include "ncl/ncl.h"
#include "nclextra/msetsblock.h"

#define SPRNG
#include "sprng/sprng.h"

// add json library for serializing and deserializing Params
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// redefine assertion
inline void _my_assert(const char* expression, const char *func, const char* file, int line)
{
    char *sfile = (char*)strrchr(file, '/');
    if (!sfile) sfile = (char*)file; else sfile++;
    cerr << sfile << ":" << line << ": " << func << ": Assertion `" << expression << "' failed." << endl;
    abort();
}
 
#ifdef NDEBUG
#define ASSERT(EXPRESSION) ((void)0)
#else
    #if defined(__GNUC__) || defined(__clang__)
        #define ASSERT(EXPRESSION) ((EXPRESSION) ? (void)0 : _my_assert(#EXPRESSION, __PRETTY_FUNCTION__, __FILE__, __LINE__))
    #else
        #define ASSERT(EXPRESSION) ((EXPRESSION) ? (void)0 : _my_assert(#EXPRESSION, __func__, __FILE__, __LINE__))
    #endif
#endif


#define USE_HASH_MAP

#if defined(__GNUC__) && !defined(GCC_VERSION)
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
//#else
//#define GCC_VERSION 0
#endif

// for MSVC
#ifndef __func__
#define __func__ __FUNCTION__
#endif

#if defined(USE_HASH_MAP)
//    #include <unordered_map>
//    #include <unordered_set>

	#if defined(_MSC_VER)
		#include <unordered_map>
		#include <unordered_set>
    #elif defined(__clang__)
        // libc++ detected:     _LIBCPP_VERSION
        // libstdc++ detected:  __GLIBCXX__
            #include <unordered_map>
            #include <unordered_set>
	#elif !defined(__GNUC__)
		#include <hash_map>
		#include <hash_set>
		using namespace stdext;
	#elif GCC_VERSION < 40300
		#include <ext/hash_map>
		#include <ext/hash_set>
		using namespace __gnu_cxx;
		#define unordered_map hash_map
		#define unordered_set hash_set
	#else
		#include <unordered_map>
		#include <unordered_set>
	#endif

#else
	#include <map>
	#include <set>
#endif

using namespace std;


#if	defined(USE_HASH_MAP) && GCC_VERSION < 40300 && !defined(_MSC_VER) && !defined(__clang__)
/*
        Define the hash function of Split
 */
#if !defined(__GNUC__)
namespace stdext {
#else
namespace __gnu_cxx {
#endif

    template<>
    struct hash<string> {

        size_t operator()(string str) const {
            hash<const char*> hash_str;
            return hash_str(str.c_str());
        }
    };
} // namespace
#endif // USE_HASH_MAP

struct Distribution {
  string random_numbers_str;
  int pool_size;
} ;

/**
 *  Specify 5 distributions for indel-size generation.
 */
enum INDEL_DIS_TYPE {
    NEG_BIN,
    ZIPF,
    LAV,
    GEO,
    USER_DEFINED
};


/**
 * Serialize INDEL_DIS_TYPE to json
 * IMPORTANT: This function must be kept synchronized with the enum INDEL_DIS_TYPE
*/
inline void to_json(json& j, const INDEL_DIS_TYPE& value) {
    switch (value) {
        case NEG_BIN: j = "NEG_BIN"; break;
        case ZIPF: j = "ZIPF"; break;
        case LAV: j = "LAV"; break;
        case GEO: j = "GEO"; break;
        case USER_DEFINED: j = "USER_DEFINED"; break;
    }
}

/**
 * Deserialize INDEL_DIS_TYPE from json
 * IMPORTANT: This function must be kept synchronized with the enum INDEL_DIS_TYPE
*/
inline void from_json(const json& j, INDEL_DIS_TYPE& value) {
    std::string str = j.get<std::string>();
    if (str == "NEG_BIN") value = NEG_BIN;
    else if (str == "ZIPF") value = ZIPF;
    else if (str == "LAV") value = LAV;
    else if (str == "GEO") value = GEO;
    else if (str == "USER_DEFINED") value = USER_DEFINED;
    else throw std::runtime_error("INDEL_DIS_TYPE: unknown value " + str);
}

/**
 *  Specify 3 options for assigning site freqs, substitution rate to sites
 */
enum ASSIGNMENT_TYPE {
    POSTERIOR_MEAN,
    POSTERIOR_DIS,
    UNSPECIFIED
};
/**
 * Serialize ASSIGNMENT_TYPE to json 
 * IMPORTANT: This function must be kept synchronized with the enum ASSIGNMENT_TYPE
 */
inline void to_json(json& j, const ASSIGNMENT_TYPE& value) {
    switch (value) {
        case POSTERIOR_MEAN: j = "POSTERIOR_MEAN"; break;
        case POSTERIOR_DIS: j = "POSTERIOR_DIS"; break;
        case UNSPECIFIED: j = "UNSPECIFIED"; break;
    }
}

/**
 * Deserialize ASSIGNMENT_TYPE from json 
 * IMPORTANT: This function must be kept synchronized with the enum ASSIGNMENT_TYPE
 */
inline void from_json(const json& j, ASSIGNMENT_TYPE& value) {
    std::string str = j.get<std::string>();
    if (str == "POSTERIOR_MEAN") value = POSTERIOR_MEAN;
    else if (str == "POSTERIOR_DIS") value = POSTERIOR_DIS;
    else if (str == "UNSPECIFIED") value = UNSPECIFIED;
    else throw std::runtime_error("ASSIGNMENT_TYPE: unknown value " + str);
}

/**
 *  Specify 2 simulation approaches.
 */
enum SIMULATION_METHOD {
    TRANS_PROB_MATRIX,
    RATE_MATRIX
};

/**
 *  Specify 2 AliSim-OpenMP algorithms.
 */
enum ALI_OPENMP_ALG {
    IM,
    EM
};
/**
 * Serialize ALI_OPENMP_ALG to json 
 * IMPORTANT: This function must be kept synchronized with the enum ALI_OPENMP_ALG
 */
inline void to_json(json& j, const ALI_OPENMP_ALG& value) {
    switch (value) {
        case IM: j = "IM"; break;
        case EM: j = "EM"; break;
    }
}
/**
 * Deserialize ALI_OPENMP_ALG from json 
 * IMPORTANT: This function must be kept synchronized with the enum ALI_OPENMP_ALG
 */
inline void from_json(const json& j, ALI_OPENMP_ALG& value) {
    std::string str = j.get<std::string>();
    if (str == "IM") value = IM;
    else if (str == "EM") value = EM;
    else throw std::runtime_error("ALI_OPENMP_ALG: unknown value " + str);
}

/**
 *  Specify 2 simulation approaches.
 */
enum SEQ_CHUNK_STATUS {
    EMPTY,
    OCCUPIED,
    WRITING,
    READING
};

struct IndelDistribution {
    INDEL_DIS_TYPE indel_dis_type;
    double param_1 = -1, param_2 = -1;
    string user_defined_dis = "";
    // constructor
    IndelDistribution(){}
    IndelDistribution(INDEL_DIS_TYPE new_indel_dis_type, double new_param_1 = -1, double new_param_2 = -1, string new_user_defined_dis = "")
    {
        indel_dis_type = new_indel_dis_type;
        param_1 = new_param_1;
        param_2 = new_param_2;
        user_defined_dis = new_user_defined_dis;
    }
} ;

class Linear {
public:

    Linear(int n, double *x, double *y) {

        // calculate the averages of arrays x and y
        double xa = 0, ya = 0;
        for (int i = 0; i < n; i++) {
            xa += x[i];
            ya += y[i];
        }
        xa /= n;
        ya /= n;

        // calculate auxiliary sums
        double xx = 0, yy = 0, xy = 0;
        for (int i = 0; i < n; i++) {
            double tmpx = x[i] - xa, tmpy = y[i] - ya;
            xx += tmpx * tmpx;
            yy += tmpy * tmpy;
            xy += tmpx * tmpy;
        }

        // calculate regression line parameters

        // make sure slope is not infinite
        ASSERT(fabs(xx) != 0);

        m_b = xy / xx;
        m_a = ya - m_b * xa;
        m_coeff = (fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy);

    }

    double getValue(double x) {
        return m_a + m_b * x;
    }

    //! Returns the slope of the regression line

    double getSlope() {
        return m_b;
    }

    //! Returns the intercept on the Y axis of the regression line

    double getIntercept() {
        return m_a;
    }

    //! Returns the linear regression coefficient

    double getCoefficient() {
        return m_coeff;
    }

private:

    double m_a, m_b, m_coeff;
};

/**
        vector of double number
 */
typedef vector<double> DoubleVector;

/**
        vector of int
 */
typedef vector<int> IntList;


/**
        vector of int
 */
typedef vector<int> IntVector;

/**
        vector of bool
 */
typedef vector<bool> BoolVector;


/**
        vector of char
 */
typedef vector<char> CharVector;

/**
        vector of string
 */
typedef vector<string> StrVector;


/**
        matrix of double number
 */
#define mmatrix(T) vector< vector<T> >

/**
        matrix of double
 */

typedef mmatrix(double) DoubleMatrix;

typedef unsigned int UINT;

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

/**
        run mode of program
 */
enum RunMode {
    DETECTED, GREEDY, PRUNING, BOTH_ALG, EXHAUSTIVE, DYNAMIC_PROGRAMMING,
    CALC_DIST, PD_USER_SET, PRINT_TAXA, PRINT_AREA, SCALE_BRANCH_LEN,
    SCALE_NODE_NAME, PD_DISTRIBUTION, LINEAR_PROGRAMMING, STATS //, GBO, MPRO
}; //STATS and GBO added by MA (STATS for some statistics on tree, GBO = guided 'bootstrap'
/**
 * Serialize RunMode to json 
 * IMPORTANT: This function must be kept synchronized with the enum RunMode
 */
inline void to_json(json& j, const RunMode& value) {
    switch (value) {
        case DETECTED: j = "DETECTED"; break;
        case GREEDY: j = "GREEDY"; break;
        case PRUNING: j = "PRUNING"; break;
        case BOTH_ALG: j = "BOTH_ALG"; break;
        case EXHAUSTIVE: j = "EXHAUSTIVE"; break;
        case DYNAMIC_PROGRAMMING: j = "DYNAMIC_PROGRAMMING"; break;
        case CALC_DIST: j = "CALC_DIST"; break;
        case PD_USER_SET: j = "PD_USER_SET"; break;
        case PRINT_TAXA: j = "PRINT_TAXA"; break;
        case PRINT_AREA: j = "PRINT_AREA"; break;
        case SCALE_BRANCH_LEN: j = "SCALE_BRANCH_LEN"; break;
        case SCALE_NODE_NAME: j = "SCALE_NODE_NAME"; break;
        case PD_DISTRIBUTION: j = "PD_DISTRIBUTION"; break;
        case LINEAR_PROGRAMMING: j = "LINEAR_PROGRAMMING"; break;
        case STATS: j = "STATS"; break;
    }
}
/**
 * Deserialize RunMode from json 
 * IMPORTANT: This function must be kept synchronized with the enum RunMode
 */
inline void from_json(const json& j, RunMode& value) {
    std::string str = j.get<std::string>();
    if (str == "DETECTED") value = DETECTED;
    else if (str == "GREEDY") value = GREEDY;
    else if (str == "PRUNING") value = PRUNING;
    else if (str == "BOTH_ALG") value = BOTH_ALG;
    else if (str == "EXHAUSTIVE") value = EXHAUSTIVE;
    else if (str == "DYNAMIC_PROGRAMMING") value = DYNAMIC_PROGRAMMING;
    else if (str == "CALC_DIST") value = CALC_DIST;
    else if (str == "PD_USER_SET") value = PD_USER_SET;
    else if (str == "PRINT_TAXA") value = PRINT_TAXA;
    else if (str == "PRINT_AREA") value = PRINT_AREA;
    else if (str == "SCALE_BRANCH_LEN") value = SCALE_BRANCH_LEN;
    else if (str == "SCALE_NODE_NAME") value = SCALE_NODE_NAME;
    else if (str == "PD_DISTRIBUTION") value = PD_DISTRIBUTION;
    else if (str == "LINEAR_PROGRAMMING") value = LINEAR_PROGRAMMING;
    else if (str == "STATS") value = STATS;
    else throw std::runtime_error("RunMode: unknown value " + str);
}

/**
        type of generating trees or splits graphs
 */
enum TreeGenType {
    NONE, YULE_HARDING, UNIFORM, CATERPILLAR, BALANCED, BIRTH_DEATH,
    CIRCULAR_SPLIT_GRAPH, TAXA_SET, STAR_TREE
};
/**
 * Serialize TreeGenType to json 
 * IMPORTANT: This function must be kept synchronized with the enum TreeGenType
 */
inline void to_json(json& j, const TreeGenType& value) {
    switch (value) {
        case NONE: j = "NONE"; break;
        case YULE_HARDING: j = "YULE_HARDING"; break;
        case UNIFORM: j = "UNIFORM"; break;
        case CATERPILLAR: j = "CATERPILLAR"; break;
        case BALANCED: j = "BALANCED"; break;
        case BIRTH_DEATH: j = "BIRTH_DEATH"; break;
        case CIRCULAR_SPLIT_GRAPH: j = "CIRCULAR_SPLIT_GRAPH"; break;
        case TAXA_SET: j = "TAXA_SET"; break;
        case STAR_TREE: j = "STAR_TREE"; break;
    }
}
/**
 * Deserialize TreeGenType from json 
 * IMPORTANT: This function must be kept synchronized with the enum TreeGenType
 */
inline void from_json(const json& j, TreeGenType& value) {
    std::string str = j.get<std::string>();
    if (str == "NONE") value = NONE;
    else if (str == "YULE_HARDING") value = YULE_HARDING;
    else if (str == "UNIFORM") value = UNIFORM;
    else if (str == "CATERPILLAR") value = CATERPILLAR;
    else if (str == "BALANCED") value = BALANCED;
    else if (str == "BIRTH_DEATH") value = BIRTH_DEATH;
    else if (str == "CIRCULAR_SPLIT_GRAPH") value = CIRCULAR_SPLIT_GRAPH;
    else if (str == "TAXA_SET") value = TAXA_SET;
    else if (str == "STAR_TREE") value = STAR_TREE;
    else throw std::runtime_error("TreeGenType: unknown value " + str);
}

/**
        when writing tree:
                WT_BR_LEN - output branch length
                WT_BR_CLADE - put branch length into internal node name
                WT_TAXON_ID - output taxon ID
                WT_INT_NODE - for draw tree, draw the internal node ID
                WT_BR_SCALE - for draw tree, draw the branch proportional to its length
                WT_SORT_TAXA - sort the taxa s.t. subtrees with least taxon ID come first
                WT_APPEND    - append the output file
                WT_NEWLINE   - print a newline after
                WT_BR_LEN_FIXED_WIDTH - print branch length in fixed number format
 */
const int WT_BR_LEN = 1;
const int WT_BR_CLADE = 2;
const int WT_TAXON_ID = 4;
const int WT_INT_NODE = 8;
const int WT_BR_SCALE = 16;
const int WT_SORT_TAXA = 32;
const int WT_APPEND = 64;
const int WT_NEWLINE = 128;
const int WT_BR_LEN_FIXED_WIDTH = 256;
const int WT_BR_ID = 512;
const int WT_BR_LEN_ROUNDING = 1024;
const int WT_BR_LEN_SHORT = 2048; // store only 6 digits after the comma for branch lengths
const int WT_BR_ATTR = 4096; // print branch attributes
const int TRUE = 1;
const int FALSE = 0;
const double ONE_THIRD = 1.0 / 3.0;

/**
 *  Specify different ways of doing an NNI.
 *  TOPO_ONLY: only change the tree topology
 *  TOPO_UPDATE_LV: the same as above but the partial likelihoods are update in addition
 *  NNI1: optimize the central branch after changing the tree topology
 *  NNI5: optimized the 5 affected branches after changing the tree topology
 */
enum NNI_Type {
    TOPO_ONLY,
    TOPO_UPDATE_LV,
    NNI1,
    NNI5
};

/**
 * Serialize NNI_Type to json 
 * IMPORTANT: This function must be kept synchronized with the enum NNI_Type
*/
inline void to_json(json& j, const NNI_Type& value) {
    switch (value) {
        case TOPO_ONLY: j = "TOPO_ONLY"; break;
        case TOPO_UPDATE_LV: j = "TOPO_UPDATE_LV"; break;
        case NNI1: j = "NNI1"; break;
        case NNI5: j = "NNI5"; break;
    }
}
/**
 * Deserialize NNI_Type from json 
 * IMPORTANT: This function must be kept synchronized with the enum NNI_Type
*/
inline void from_json(const json& j, NNI_Type& value) {
    std::string str = j.get<std::string>();
    if (str == "TOPO_ONLY") value = TOPO_ONLY;
    else if (str == "TOPO_UPDATE_LV") value = TOPO_UPDATE_LV;
    else if (str == "NNI1") value = NNI1;
    else if (str == "NNI5") value = NNI5;
    else throw std::runtime_error("NNI_Type: unknown value " + str);
}


/**
        when computing Robinson-Foulds distances
 */
const int RF_ADJACENT_PAIR = 1;
const int RF_ALL_PAIR = 2;
const int RF_TWO_TREE_SETS = 3;
const int RF_TWO_TREE_SETS_EXTENDED = 4; // work for trees with non-equal taxon sets

/**
        split weight summarization
 */
const int SW_COUNT = 1; // just counting the number of splits
const int SW_SUM = 2; // take the sum of all split weights
const int SW_AVG_ALL = 3; // take the split weight average over all trees
const int SW_AVG_PRESENT = 4; // take the split weight average over all trees that the split is present

/**
        search mode
 */
//enum SearchMode {EXHAUSTIVE, EXHAUSTIVE_CIRCULAR};

/**
        input type, tree or splits graph
 */
enum InputType {
    IN_NEWICK, IN_NEXUS, IN_FASTA, IN_PHYLIP, IN_COUNTS, IN_CLUSTAL, IN_MSF, IN_MAPLE, IN_OTHER
};

  // TODO DS: SAMPLING_SAMPLED is DEPRECATED and it is not possible to run PoMo with SAMPLING_SAMPLED.
enum SamplingType {
  SAMPLING_WEIGHTED_BINOM, SAMPLING_WEIGHTED_HYPER, SAMPLING_SAMPLED
};

/**
        verbose mode, determine how verbose should the screen be printed.
 */
enum VerboseMode {
    VB_QUIET, VB_MIN, VB_MED, VB_MAX, VB_DEBUG
};

/**
        verbose level on the screen
 */
extern VerboseMode verbose_mode;

/**
        consensus reconstruction type
 */
enum ConsensusType {
    CT_NONE, CT_CONSENSUS_TREE, CT_CONSENSUS_NETWORK,
    CT_ASSIGN_SUPPORT, CT_ASSIGN_SUPPORT_EXTENDED, COMPARE,
    CT_ROOTSTRAP
};
/**
 * Serialize ConsensusType to json 
 * IMPORTANT: This function must be kept synchronized with the enum ConsensusType
 */
inline void to_json(json& j, const ConsensusType& value) {
    switch (value) {
        case CT_NONE: j = "CT_NONE"; break;
        case CT_CONSENSUS_TREE: j = "CT_CONSENSUS_TREE"; break;
        case CT_CONSENSUS_NETWORK: j = "CT_CONSENSUS_NETWORK"; break;
        case CT_ASSIGN_SUPPORT: j = "CT_ASSIGN_SUPPORT"; break;
        case CT_ASSIGN_SUPPORT_EXTENDED: j = "CT_ASSIGN_SUPPORT_EXTENDED"; break;
        case COMPARE: j = "COMPARE"; break;
        case CT_ROOTSTRAP: j = "CT_ROOTSTRAP"; break;
    }
}

/**
 * Deserialize ConsensusType from json 
 * IMPORTANT: This function must be kept synchronized with the enum ConsensusType
 */
inline void from_json(const json& j, ConsensusType& value) {
    std::string str = j.get<std::string>();
    if (str == "CT_NONE") value = CT_NONE;
    else if (str == "CT_CONSENSUS_TREE") value = CT_CONSENSUS_TREE;
    else if (str == "CT_CONSENSUS_NETWORK") value = CT_CONSENSUS_NETWORK;
    else if (str == "CT_ASSIGN_SUPPORT") value = CT_ASSIGN_SUPPORT;
    else if (str == "CT_ASSIGN_SUPPORT_EXTENDED") value = CT_ASSIGN_SUPPORT_EXTENDED;
    else if (str == "COMPARE") value = COMPARE;
    else if (str == "CT_ROOTSTRAP") value = CT_ROOTSTRAP;
    else throw std::runtime_error("ConsensusType: unknown value " + str);
}

enum TestType {
    TEST_NONE, TEST_COMPATIBLE, TEST_CIRCULAR, TEST_WEAKLY_COMPATIBLE, TEST_K_COMPATIBLE
};
/**
 * Serialize TestType to json 
 * IMPORTANT: This function must be kept synchronized with the enum TestType
 */
inline void to_json(json& j, const TestType& value) {
    switch (value) {
        case TEST_NONE: j = "TEST_NONE"; break;
        case TEST_COMPATIBLE: j = "TEST_COMPATIBLE"; break;
        case TEST_CIRCULAR: j = "TEST_CIRCULAR"; break;
        case TEST_WEAKLY_COMPATIBLE: j = "TEST_WEAKLY_COMPATIBLE"; break;
        case TEST_K_COMPATIBLE: j = "TEST_K_COMPATIBLE"; break;
    }
}
/**
 * Deserialize TestType from json 
 * IMPORTANT: This function must be kept synchronized with the enum TestType
 */
inline void from_json(const json& j, TestType& value) {
    std::string str = j.get<std::string>();
    if (str == "TEST_NONE") value = TEST_NONE;
    else if (str == "TEST_COMPATIBLE") value = TEST_COMPATIBLE;
    else if (str == "TEST_CIRCULAR") value = TEST_CIRCULAR;
    else if (str == "TEST_WEAKLY_COMPATIBLE") value = TEST_WEAKLY_COMPATIBLE;
    else if (str == "TEST_K_COMPATIBLE") value = TEST_K_COMPATIBLE;
    else throw std::runtime_error("TestType: unknown value " + str);
}

/**
        State frequency type
 */
enum StateFreqType {
    FREQ_UNKNOWN, FREQ_USER_DEFINED, FREQ_EQUAL, FREQ_EMPIRICAL, FREQ_ESTIMATE,
    FREQ_CODON_1x4, FREQ_CODON_3x4, FREQ_CODON_3x4C, // special frequency for codon model
    FREQ_MIXTURE, // mixture-frequency model
    // FREQ_DNA_RY has pi_A+pi_G = 0.5 = pi_C+pi_T. Similarly WS pairs (AT)(CG),
    // MK pairs (AC)(GT) in same way.
    FREQ_DNA_RY, FREQ_DNA_WS, FREQ_DNA_MK,
    // in following, digits indicate which frequencies must equal each other
    // (in ACGT order), e.g. 2131 means pi_C=pi_T (pi_A, pi_G unconstrained)
    FREQ_DNA_1112, FREQ_DNA_1121, FREQ_DNA_1211, FREQ_DNA_2111,
    FREQ_DNA_1122, FREQ_DNA_1212, FREQ_DNA_1221, 
    FREQ_DNA_1123, FREQ_DNA_1213, FREQ_DNA_1231, 
    FREQ_DNA_2113, FREQ_DNA_2131, FREQ_DNA_2311, 
};
/**
 * Serialize StateFreqType to json 
 * IMPORTANT: This function must be kept synchronized with the enum StateFreqType
 */
inline void to_json(json& j, const StateFreqType& value) {
    switch (value) {
        case FREQ_UNKNOWN: j = "FREQ_UNKNOWN"; break;
        case FREQ_USER_DEFINED: j = "FREQ_USER_DEFINED"; break;
        case FREQ_EQUAL: j = "FREQ_EQUAL"; break;
        case FREQ_EMPIRICAL: j = "FREQ_EMPIRICAL"; break;
        case FREQ_ESTIMATE: j = "FREQ_ESTIMATE"; break;
        case FREQ_CODON_1x4: j = "FREQ_CODON_1x4"; break;
        case FREQ_CODON_3x4: j = "FREQ_CODON_3x4"; break;
        case FREQ_CODON_3x4C: j = "FREQ_CODON_3x4C"; break;
        case FREQ_MIXTURE: j = "FREQ_MIXTURE"; break;
        case FREQ_DNA_RY: j = "FREQ_DNA_RY"; break;
        case FREQ_DNA_WS: j = "FREQ_DNA_WS"; break;
        case FREQ_DNA_MK: j = "FREQ_DNA_MK"; break;
        case FREQ_DNA_1112: j = "FREQ_DNA_1112"; break;
        case FREQ_DNA_1121: j = "FREQ_DNA_1121"; break;
        case FREQ_DNA_1211: j = "FREQ_DNA_1211"; break;
        case FREQ_DNA_2111: j = "FREQ_DNA_2111"; break;
        case FREQ_DNA_1122: j = "FREQ_DNA_1122"; break;
        case FREQ_DNA_1212: j = "FREQ_DNA_1212"; break;
        case FREQ_DNA_1221: j = "FREQ_DNA_1221"; break;
        case FREQ_DNA_1123: j = "FREQ_DNA_1123"; break;
        case FREQ_DNA_1213: j = "FREQ_DNA_1213"; break;
        case FREQ_DNA_1231: j = "FREQ_DNA_1231"; break;
        case FREQ_DNA_2113: j = "FREQ_DNA_2113"; break;
        case FREQ_DNA_2131: j = "FREQ_DNA_2131"; break;
        case FREQ_DNA_2311: j = "FREQ_DNA_2311"; break;
    }
}

/**
 * Deserialize StateFreqType from json 
 * IMPORTANT: This function must be kept synchronized with the enum StateFreqType
 */
inline void from_json(const json& j, StateFreqType& value) {
    std::string str = j.get<std::string>();
    if (str == "FREQ_UNKNOWN") value = FREQ_UNKNOWN;
    else if (str == "FREQ_USER_DEFINED") value = FREQ_USER_DEFINED;
    else if (str == "FREQ_EQUAL") value = FREQ_EQUAL;
    else if (str == "FREQ_EMPIRICAL") value = FREQ_EMPIRICAL;
    else if (str == "FREQ_ESTIMATE") value = FREQ_ESTIMATE;
    else if (str == "FREQ_CODON_1x4") value = FREQ_CODON_1x4;
    else if (str == "FREQ_CODON_3x4") value = FREQ_CODON_3x4;
    else if (str == "FREQ_CODON_3x4C") value = FREQ_CODON_3x4C;
    else if (str == "FREQ_MIXTURE") value = FREQ_MIXTURE;
    else if (str == "FREQ_DNA_RY") value = FREQ_DNA_RY;
    else if (str == "FREQ_DNA_WS") value = FREQ_DNA_WS;
    else if (str == "FREQ_DNA_MK") value = FREQ_DNA_MK;
    else if (str == "FREQ_DNA_1112") value = FREQ_DNA_1112;
    else if (str == "FREQ_DNA_1121") value = FREQ_DNA_1121;
    else if (str == "FREQ_DNA_1211") value = FREQ_DNA_1211;
    else if (str == "FREQ_DNA_2111") value = FREQ_DNA_2111;
    else if (str == "FREQ_DNA_1122") value = FREQ_DNA_1122;
    else if (str == "FREQ_DNA_1212") value = FREQ_DNA_1212;
    else if (str == "FREQ_DNA_1221") value = FREQ_DNA_1221;
    else if (str == "FREQ_DNA_1123") value = FREQ_DNA_1123;
    else if (str == "FREQ_DNA_1213") value = FREQ_DNA_1213;
    else if (str == "FREQ_DNA_1231") value = FREQ_DNA_1231;
    else if (str == "FREQ_DNA_2113") value = FREQ_DNA_2113;
    else if (str == "FREQ_DNA_2131") value = FREQ_DNA_2131;
    else if (str == "FREQ_DNA_2311") value = FREQ_DNA_2311;
    else throw std::runtime_error("StateFreqType: unknown value " + str);
}

/*
    outfile file format
 FORMAT_NORMAL: usual file format used so far
 FORMAT_CSV: csv file format
 FORMAT_TSV: tab separated file format
 */
enum FileFormat {
    FORMAT_NORMAL, FORMAT_CSV, FORMAT_TSV
};

enum ModelTestCriterion {
    MTC_AIC, MTC_AICC, MTC_BIC, MTC_ALL
};
/**
 * Serialize ModelTestCriterion to json 
 * IMPORTANT: This function must be kept synchronized with the enum ModelTestCriterion
 */
inline void to_json(json& j, const ModelTestCriterion& value) {
    switch (value) {
        case MTC_AIC: j = "MTC_AIC"; break;
        case MTC_AICC: j = "MTC_AICC"; break;
        case MTC_BIC: j = "MTC_BIC"; break;
        case MTC_ALL: j = "MTC_ALL"; break;
    }
}

/**
 * Deserialize ModelTestCriterion from json 
 * IMPORTANT: This function must be kept synchronized with the enum ModelTestCriterion
 */
inline void from_json(const json& j, ModelTestCriterion& value) {
    std::string str = j.get<std::string>();
    if (str == "MTC_AIC") value = MTC_AIC;
    else if (str == "MTC_AICC") value = MTC_AICC;
    else if (str == "MTC_BIC") value = MTC_BIC;
    else if (str == "MTC_ALL") value = MTC_ALL;
    else throw std::runtime_error("ModelTestCriterion: unknown value " + str);
}

/**
 PartitionFinder merging algorithm
 */
enum PartitionMerge {
    MERGE_NONE, MERGE_GREEDY, MERGE_RCLUSTER, MERGE_RCLUSTERF, MERGE_KMEANS
};
/**
 * Serialize PartitionMerge to json 
 * IMPORTANT: This function must be kept synchronized with the enum PartitionMerge
 */
inline void to_json(json& j, const PartitionMerge& value) {
    switch (value) {
        case MERGE_NONE: j = "MERGE_NONE"; break;
        case MERGE_GREEDY: j = "MERGE_GREEDY"; break;
        case MERGE_RCLUSTER: j = "MERGE_RCLUSTER"; break;
        case MERGE_RCLUSTERF: j = "MERGE_RCLUSTERF"; break;
        case MERGE_KMEANS: j = "MERGE_KMEANS"; break;
    }
}
/**
 * Deserialize PartitionMerge from json 
 * IMPORTANT: This function must be kept synchronized with the enum PartitionMerge
 */
inline void from_json(const json& j, PartitionMerge& value) {
    std::string str = j.get<std::string>();
    if (str == "MERGE_NONE") value = MERGE_NONE;
    else if (str == "MERGE_GREEDY") value = MERGE_GREEDY;
    else if (str == "MERGE_RCLUSTER") value = MERGE_RCLUSTER;
    else if (str == "MERGE_RCLUSTERF") value = MERGE_RCLUSTERF;
    else if (str == "MERGE_KMEANS") value = MERGE_KMEANS;
    else throw std::runtime_error("PartitionMerge: unknown value " + str);
}

/**
        Stopping condition type
 */
enum STOP_CONDITION {
    SC_FIXED_ITERATION, SC_WEIBULL, SC_UNSUCCESS_ITERATION, SC_BOOTSTRAP_CORRELATION, SC_REAL_TIME
};
/**
 * Serialize STOP_CONDITION to json 
 * IMPORTANT: This function must be kept synchronized with the enum STOP_CONDITION
 */
inline void to_json(json& j, const STOP_CONDITION& value) {
    switch (value) {
        case SC_FIXED_ITERATION: j = "SC_FIXED_ITERATION"; break;
        case SC_WEIBULL: j = "SC_WEIBULL"; break;
        case SC_UNSUCCESS_ITERATION: j = "SC_UNSUCCESS_ITERATION"; break;
        case SC_BOOTSTRAP_CORRELATION: j = "SC_BOOTSTRAP_CORRELATION"; break;
        case SC_REAL_TIME: j = "SC_REAL_TIME"; break;
    }
}
/**
 * Deserialize STOP_CONDITION from json 
 * IMPORTANT: This function must be kept synchronized with the enum STOP_CONDITION
 */
inline void from_json(const json& j, STOP_CONDITION& value) {
    std::string str = j.get<std::string>();
    if (str == "SC_FIXED_ITERATION") value = SC_FIXED_ITERATION;
    else if (str == "SC_WEIBULL") value = SC_WEIBULL;
    else if (str == "SC_UNSUCCESS_ITERATION") value = SC_UNSUCCESS_ITERATION;
    else if (str == "SC_BOOTSTRAP_CORRELATION") value = SC_BOOTSTRAP_CORRELATION;
    else if (str == "SC_REAL_TIME") value = SC_REAL_TIME;
    else throw std::runtime_error("STOP_CONDITION: unknown value " + str);
}

enum IQP_ASSESS_QUARTET {
    IQP_DISTANCE, IQP_PARSIMONY, IQP_BOOTSTRAP
};
/**
 * Serialize IQP_ASSESS_QUARTET to json 
 * IMPORTANT: This function must be kept synchronized with the enum IQP_ASSESS_QUARTET
*/
inline void to_json(json& j, const IQP_ASSESS_QUARTET& value) {
    switch (value) {
        case IQP_DISTANCE: j = "IQP_DISTANCE"; break;
        case IQP_PARSIMONY: j = "IQP_PARSIMONY"; break;
        case IQP_BOOTSTRAP: j = "IQP_BOOTSTRAP"; break;
    }
}
/**
 * Deserialize IQP_ASSESS_QUARTET from json 
 * IMPORTANT: This function must be kept synchronized with the enum IQP_ASSESS_QUARTET
*/
inline void from_json(const json& j, IQP_ASSESS_QUARTET& value) {
    std::string str = j.get<std::string>();
    if (str == "IQP_DISTANCE") value = IQP_DISTANCE;
    else if (str == "IQP_PARSIMONY") value = IQP_PARSIMONY;
    else if (str == "IQP_BOOTSTRAP") value = IQP_BOOTSTRAP;
    else throw std::runtime_error("IQP_ASSESS_QUARTET: unknown value " + str);
}

enum LEAST_SQUARE_VAR {
    OLS, WLS_FIRST_TAYLOR, WLS_FITCH_MARGOLIASH, WLS_SECOND_TAYLOR, WLS_PAUPLIN
};
/**
 * Serialize LEAST_SQUARE_VAR to json
 * IMPORTANT: This function must be kept synchronized with the enum LEAST_SQUARE_VAR
*/
inline void to_json(json& j, const LEAST_SQUARE_VAR& value) {
    switch (value) {
        case OLS: j = "OLS"; break;
        case WLS_FIRST_TAYLOR: j = "WLS_FIRST_TAYLOR"; break;
        case WLS_FITCH_MARGOLIASH: j = "WLS_FITCH_MARGOLIASH"; break;
        case WLS_SECOND_TAYLOR: j = "WLS_SECOND_TAYLOR"; break;
        case WLS_PAUPLIN: j = "WLS_PAUPLIN"; break;
    }
}
/**
 * Deserialize LEAST_SQUARE_VAR from json
 * IMPORTANT: This function must be kept synchronized with the enum LEAST_SQUARE_VAR
*/
inline void from_json(const json& j, LEAST_SQUARE_VAR& value) {
    std::string str = j.get<std::string>();
    if (str == "OLS") value = OLS;
    else if (str == "WLS_FIRST_TAYLOR") value = WLS_FIRST_TAYLOR;
    else if (str == "WLS_FITCH_MARGOLIASH") value = WLS_FITCH_MARGOLIASH;
    else if (str == "WLS_SECOND_TAYLOR") value = WLS_SECOND_TAYLOR;
    else if (str == "WLS_PAUPLIN") value = WLS_PAUPLIN;
    else throw std::runtime_error("LEAST_SQUARE_VAR: unknown value " + str);
 }
enum START_TREE_TYPE {
	STT_BIONJ, STT_PARSIMONY, STT_PLL_PARSIMONY, STT_RANDOM_TREE, STT_USER_TREE
};
/**
 * Serialize START_TREE_TYPE to json 
 * IMPORTANT: This function must be kept synchronized with the enum START_TREE_TYPE
 */
inline void to_json(json& j, const START_TREE_TYPE& value) {
    switch (value) {
        case STT_BIONJ: j = "STT_BIONJ"; break;
        case STT_PARSIMONY: j = "STT_PARSIMONY"; break;
        case STT_PLL_PARSIMONY: j = "STT_PLL_PARSIMONY"; break;
        case STT_RANDOM_TREE: j = "STT_RANDOM_TREE"; break;
        case STT_USER_TREE: j = "STT_USER_TREE"; break;
    }
}
/**
 * Deserialize START_TREE_TYPE from json 
 * IMPORTANT: This function must be kept synchronized with the enum START_TREE_TYPE
 */
inline void from_json(const json& j, START_TREE_TYPE& value) {
    std::string str = j.get<std::string>();
    if (str == "STT_BIONJ") value = STT_BIONJ;
    else if (str == "STT_PARSIMONY") value = STT_PARSIMONY;
    else if (str == "STT_PLL_PARSIMONY") value = STT_PLL_PARSIMONY;
    else if (str == "STT_RANDOM_TREE") value = STT_RANDOM_TREE;
    else if (str == "STT_USER_TREE") value = STT_USER_TREE;
    else throw std::runtime_error("START_TREE_TYPE: unknown value " + str);
}

const int MCAT_LOG = 1; // categorize by log(rate) for Meyer & von Haeseler model
const int MCAT_MEAN = 2; // take the mean of rates for each category for Meyer & von Haeseler model
const int MCAT_PATTERN = 4; // categorize site-patterns instead of sites for Meyer & von Haeseler model

/* TODO DS: For PoMo, this setting does not make sense.  At the
   moment, when using PoMo, MAX_GENETIC_DIST is amended, wherever it
   is used. */
const double MAX_GENETIC_DIST = 9.0;

struct NNIInfo {
    double lh_score[4]; // tree log-likelihood of zero-branch, current tree, NNI tree 1, NNI tree 2
    double br_len[4]; // length of current branch, optimized branch, NNI branch 1, NNI branch 2
    int nni_round;
    int iqpnni_iteration;
};

/*
    0           = 80386 instruction set
    1  or above = SSE (XMM) supported by CPU (not testing for O.S. support)
    2  or above = SSE2
    3  or above = SSE3
    4  or above = Supplementary SSE3 (SSSE3)
    5  or above = SSE4.1
    6  or above = SSE4.2
    7  or above = AVX supported by CPU and operating system
    8  or above = AVX2
    9  or above = AVX512F
*/
enum LikelihoodKernel {
	LK_386, LK_SSE, LK_SSE2, LK_SSE3, LK_SSSE3, LK_SSE41, LK_SSE42, LK_AVX, LK_AVX_FMA, LK_AVX512
};
/**
 * Serialize LikelihoodKernel to json 
 * IMPORTANT: This function must be kept synchronized with the enum LikelihoodKernel
 */
inline void to_json(json& j, const LikelihoodKernel& value) {
    switch (value) {
        case LK_386: j = "LK_386"; break;
        case LK_SSE: j = "LK_SSE"; break;
        case LK_SSE2: j = "LK_SSE2"; break;
        case LK_SSE3: j = "LK_SSE3"; break;
        case LK_SSSE3: j = "LK_SSSE3"; break;
        case LK_SSE41: j = "LK_SSE41"; break;
        case LK_SSE42: j = "LK_SSE42"; break;
        case LK_AVX: j = "LK_AVX"; break;
        case LK_AVX_FMA: j = "LK_AVX_FMA"; break;
        case LK_AVX512: j = "LK_AVX512"; break;
    }
}
/**
 * Deserialize LikelihoodKernel from json 
 * IMPORTANT: This function must be kept synchronized with the enum LikelihoodKernel
 */
inline void from_json(const json& j, LikelihoodKernel& value) {
    std::string str = j.get<std::string>();
    if (str == "LK_386") value = LK_386;
    else if (str == "LK_SSE") value = LK_SSE;
    else if (str == "LK_SSE2") value = LK_SSE2;
    else if (str == "LK_SSE3") value = LK_SSE3;
    else if (str == "LK_SSSE3") value = LK_SSSE3;
    else if (str == "LK_SSE41") value = LK_SSE41;
    else if (str == "LK_SSE42") value = LK_SSE42;
    else if (str == "LK_AVX") value = LK_AVX;
    else if (str == "LK_AVX_FMA") value = LK_AVX_FMA;
    else if (str == "LK_AVX512") value = LK_AVX512;
    else throw std::runtime_error("LikelihoodKernel: unknown value " + str);
}

enum LhMemSave {
	LM_PER_NODE, LM_MEM_SAVE
};
/**
 * Serialize LhMemSave to json 
 * IMPORTANT: This function must be kept synchronized with the enum LhMemSave
 */
inline void to_json(json& j, const LhMemSave& value) {
    switch (value) {
        case LM_PER_NODE: j = "LM_PER_NODE"; break;
        case LM_MEM_SAVE: j = "LM_MEM_SAVE"; break;
    }
}
/**
 * Deserialize LhMemSave from json 
 * IMPORTANT: This function must be kept synchronized with the enum LhMemSave
 */
inline void from_json(const json& j, LhMemSave& value) {
    std::string str = j.get<std::string>();
    if (str == "LM_PER_NODE") value = LM_PER_NODE;
    else if (str == "LM_MEM_SAVE") value = LM_MEM_SAVE;
    else throw std::runtime_error("LhMemSave: unknown value " + str);
}

enum SiteLoglType {
    WSL_NONE, WSL_SITE, WSL_RATECAT, WSL_MIXTURE, WSL_MIXTURE_RATECAT
};
/**
 * Serialize SiteLoglType to json 
 * IMPORTANT: This function must be kept synchronized with the enum SiteLoglType
 */
inline void to_json(json& j, const SiteLoglType& value) {
    switch (value) {
        case WSL_NONE: j = "WSL_NONE"; break;
        case WSL_SITE: j = "WSL_SITE"; break;
        case WSL_RATECAT: j = "WSL_RATECAT"; break;
        case WSL_MIXTURE: j = "WSL_MIXTURE"; break;
        case WSL_MIXTURE_RATECAT: j = "WSL_MIXTURE_RATECAT"; break;
    }
}

/**
 * Deserialize SiteLoglType from json 
 * IMPORTANT: This function must be kept synchronized with the enum SiteLoglType
 */
inline void from_json(const json& j, SiteLoglType& value) {
    std::string str = j.get<std::string>();
    if (str == "WSL_NONE") value = WSL_NONE;
    else if (str == "WSL_SITE") value = WSL_SITE;
    else if (str == "WSL_RATECAT") value = WSL_RATECAT;
    else if (str == "WSL_MIXTURE") value = WSL_MIXTURE;
    else if (str == "WSL_MIXTURE_RATECAT") value = WSL_MIXTURE_RATECAT;
    else throw std::runtime_error("SiteLoglType: unknown value " + str);
}

enum SiteFreqType {
    WSF_NONE, WSF_POSTERIOR_MEAN, WSF_POSTERIOR_MAX
};
/**
 * Serialize SiteFreqType to json 
 * IMPORTANT: This function must be kept synchronized with the enum SiteFreqType
 */
inline void to_json(json& j, const SiteFreqType& value) {
    switch (value) {
        case WSF_NONE: j = "WSF_NONE"; break;
        case WSF_POSTERIOR_MEAN: j = "WSF_POSTERIOR_MEAN"; break;
        case WSF_POSTERIOR_MAX: j = "WSF_POSTERIOR_MAX"; break;
    }
}
/**
 * Deserialize SiteFreqType from json 
 * IMPORTANT: This function must be kept synchronized with the enum SiteFreqType
 */
inline void from_json(const json& j, SiteFreqType& value) {
    std::string str = j.get<std::string>();
    if (str == "WSF_NONE") value = WSF_NONE;
    else if (str == "WSF_POSTERIOR_MEAN") value = WSF_POSTERIOR_MEAN;
    else if (str == "WSF_POSTERIOR_MAX") value = WSF_POSTERIOR_MAX;
    else throw std::runtime_error("SiteFreqType: unknown value " + str);
}

enum MatrixExpTechnique { 
    MET_SCALING_SQUARING, 
    MET_EIGEN3LIB_DECOMPOSITION,
    MET_EIGEN_DECOMPOSITION, 
    MET_LIE_MARKOV_DECOMPOSITION
};
/**
 * Serialize MatrixExpTechnique to json 
 * IMPORTANT: This function must be kept synchronized with the enum MatrixExpTechnique
 */
inline void to_json(json& j, const MatrixExpTechnique& value) {
    switch (value) {
        case MET_SCALING_SQUARING: j = "MET_SCALING_SQUARING"; break;
        case MET_EIGEN3LIB_DECOMPOSITION: j = "MET_EIGEN3LIB_DECOMPOSITION"; break;
        case MET_EIGEN_DECOMPOSITION: j = "MET_EIGEN_DECOMPOSITION"; break;
        case MET_LIE_MARKOV_DECOMPOSITION: j = "MET_LIE_MARKOV_DECOMPOSITION"; break;
    }
}

/**
 * Deserialize MatrixExpTechnique from json 
 * IMPORTANT: This function must be kept synchronized with the enum MatrixExpTechnique
 */
inline void from_json(const json& j, MatrixExpTechnique& value) {
    std::string str = j.get<std::string>();
    if (str == "MET_SCALING_SQUARING") value = MET_SCALING_SQUARING;
    else if (str == "MET_EIGEN3LIB_DECOMPOSITION") value = MET_EIGEN3LIB_DECOMPOSITION;
    else if (str == "MET_EIGEN_DECOMPOSITION") value = MET_EIGEN_DECOMPOSITION;
    else if (str == "MET_LIE_MARKOV_DECOMPOSITION") value = MET_LIE_MARKOV_DECOMPOSITION;
    else throw std::runtime_error("MatrixExpTechnique: unknown value " + str);
}


/** ascertainment bias correction type */
enum ASCType {
    ASC_NONE, // no ASC
    ASC_VARIANT, // Lewis's correction for variant sites
    ASC_VARIANT_MISSING, // Holder's correction for variant sites with missing data
    ASC_INFORMATIVE, // correction for parsimony-informative sites
    ASC_INFORMATIVE_MISSING // Holder's correction for informative sites with missing data
};

enum AncestralSeqType {
    AST_NONE, AST_MARGINAL, AST_JOINT
};
/**
 * Serialize AncestralSeqType to json 
 * IMPORTANT: This function must be kept synchronized with the enum AncestralSeqType
 */
inline void to_json(json& j, const AncestralSeqType& value) {
    switch (value) {
        case AST_NONE: j = "AST_NONE"; break;
        case AST_MARGINAL: j = "AST_MARGINAL"; break;
        case AST_JOINT: j = "AST_JOINT"; break;
    }
}

/**
 * Deserialize AncestralSeqType from json 
 * IMPORTANT: This function must be kept synchronized with the enum AncestralSeqType
 */
inline void from_json(const json& j, AncestralSeqType& value) {
    std::string str = j.get<std::string>();
    if (str == "AST_NONE") value = AST_NONE;
    else if (str == "AST_MARGINAL") value = AST_MARGINAL;
    else if (str == "AST_JOINT") value = AST_JOINT;
    else throw std::runtime_error("AncestralSeqType: unknown value " + str);
}

enum SymTest {
    SYMTEST_NONE, SYMTEST_BINOM, SYMTEST_MAXDIV
};
/**
 * Serialize SymTest to json 
 * IMPORTANT: This function must be kept synchronized with the enum SymTest
 */
inline void to_json(json& j, const SymTest& value) {
    switch (value) {
        case SYMTEST_NONE: j = "SYMTEST_NONE"; break;
        case SYMTEST_BINOM: j = "SYMTEST_BINOM"; break;
        case SYMTEST_MAXDIV: j = "SYMTEST_MAXDIV"; break;
    }
}

/**
 * Deserialize SymTest from json 
 * IMPORTANT: This function must be kept synchronized with the enum SymTest
 */
inline void from_json(const json& j, SymTest& value) {
    std::string str = j.get<std::string>();
    if (str == "SYMTEST_NONE") value = SYMTEST_NONE;
    else if (str == "SYMTEST_BINOM") value = SYMTEST_BINOM;
    else if (str == "SYMTEST_MAXDIV") value = SYMTEST_MAXDIV;
    else throw std::runtime_error("SymTest: unknown value " + str);
}

const int BRLEN_OPTIMIZE = 0; // optimize branch lengths
const int BRLEN_FIX      = 1; // fix branch lengths
const int BRLEN_SCALE    = 2; // scale branch lengths
const int TOPO_UNLINKED  = 3; // unlinked/separate tree topologies between partitions

const int OUT_LOG       = 1; // .log file written or not
const int OUT_TREEFILE  = 2; // .treefile file written or not
const int OUT_IQTREE    = 4; // .iqtree file written or not
const int OUT_UNIQUESEQ = 8; // .uniqueseq file written or not


const double MIN_GAMMA_RATE = 1e-6;
// change from 0.01 to 0.02 as 0.01 causes numerical problems
const double MIN_GAMMA_SHAPE = 0.02;
const double MAX_GAMMA_SHAPE = 1000.0;
const double TOL_GAMMA_SHAPE = 0.001;


/** maximum number of newton-raphson steps for NNI branch evaluation */
extern int NNI_MAX_NR_STEP;

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

/**
        program parameters, everything is specified here
        Use singleton pattern to avoid using global variable or
        having to pass the params variable around
 */
class Params {
public:
    static Params& getInstance();
    json to_json() const;
    void from_json(const json& j);
    json get_param(const std::string& name) const;
    void setDefault();
private:
    Params () {}; // Disable constructor
    // Temoprarily commented out because void PhyloSuperTree::readPartition(Params &params)
    // make a copy of params?
    //Params (Params const&) {}; // Disable copy constructor
    //void operator=(Params const&) {}; // Disable assignment
public:

    /**
    *  Fast and accurate optimiation for alpha and p_invar
    */
    bool fai;

    /**
     *  Option to check memory consumption only
     */
    bool memCheck;

    /**
     *  The support threshold for stable splits (Default = 0.9)
     */
    double stableSplitThreshold;

    /**
     *  Option for adaptive perturbation.
     *  Branches that are shared among all candidate trees will be perturbed
     */
    bool adaptPertubation;

	/**
	 *  Option to do mutlipe start for estimating alpha and p_invar
	 */
	bool testAlpha;

    /**
     *  Restart the optimization of alpha and pinvar from different starting
     *  pinv values (supercedes the option testAlpha
     */
    bool opt_gammai;

    /**
     *  A faster version of opt_gammai using a heuristic similar to binary search.
     *  Thus, one does not need to perform 10 independent trials as in opt_gammai.
     */
    bool opt_gammai_fast;

    bool opt_gammai_keep_bran;

    /**
     *  Automatic adjust the log-likelihood espilon using some heuristic
     */
    bool testAlphaEpsAdaptive;

    /**
     *  Use random starting points for alpha
     */
    bool randomAlpha;

    /**
     *  Logl epsilon to test for initial alpha and pinvar values.
     *  This does not need to be small (default value = 100)
     */
    double testAlphaEps;

    /**
     *  Perform exhaustive search for parameter alpha and p_invar
     */
    bool exh_ai;

	/**
	 *  Text file contain all pairs of alpha and p_invar to
	 *  evaluate.
	 *  TODO Remove this option and implement the exhaustive search
	 *  directly into IQ-TREE
	 */
	char* alpha_invar_file;

	/**
	 *  Enable tabu search for NNI
	 */
	bool tabu;

    /**
	 *  Use (5+5)-ES strategy
	 */
	bool five_plus_five;

	/**
	 * Turn on feature to identify stable splits and fix them during tree search
	 */
	bool fixStableSplits;

	/**
	 *  Number of best trees used to compute stable splits
	 */
	int numSupportTrees;

	/**
	 *  Maximum number of trees stored in the candidate tree set
	 */
	int maxCandidates;

	/**
	 *  Number of starting parsimony trees
	 */
	int numInitTrees;

	/**
	 *  SPR distance (radius) for parsimony tree
	 */
	int sprDist;

    /** cost matrix file for Sankoff parsimony */
    char *sankoff_cost_file;
    
	/**
	 *  Number of NNI locally optimal trees generated from the set of parsimony trees
	 *  Default = 20 (out of 100 parsimony trees)
	 */
	int numNNITrees;

	/**
	 *  Number of best trees in the candidate set used to generate perturbed trees
	 *  In term of evolutionary algorithm, this is the population size
	 */
	int popSize;


	/**
	 *  heuristics for speeding up NNI evaluation
	 */
	bool speednni;


	/**
	 *  portion of NNI used for perturbing the tree
	 */
	double initPS;

	/**
	 *  logl epsilon for model parameter optimization
	 */
	double modelEps;
    
    /**
     logl epsilon for ModelFinder
     */
    double modelfinder_eps;

	/**
	 *  New search heuristics (DEFAULT: ON)
	 */
	bool snni;

	/**
	 *  Specify how the branch lengths are optimzed after each NNI operation
	 *  (No optimization, 1 branch optimization, 5 branch optimization)
	 */
    NNI_Type nni_type;

    /**
     *  Different type of Least Square variances
     */
	LEAST_SQUARE_VAR ls_var_type;

	/**
	 *  Threshold (likelihood difference between NNI and current tree)
	 *  to start optimizing 5 branches
	 */
	double nniThresHold;

	/**
	 *  Optimize 5 branches on NNI tree
	 */
	bool nni5;

	/**
	 *  Number of steps for the loop evaluating 5 branches around NNI 
	 */
	int nni5_num_eval;

	/**
	 *  Number of traversal for all branch lengths optimization of the initial tree 
	 */
	int brlen_num_traversal;

    /**
     *  Number of branch length optimization rounds performed after
     *  each NNI step (DEFAULT: 1)
     */
    int numSmoothTree;

    /**
     *   compute least square branches for a given tree
     */
    bool leastSquareBranch;

    /** TRUE to apply Manuel's analytic approximation formulae for branch length */
    bool manuel_analytic_approx;

    /** TRUE to compute parsimony branch length of final tree */
    bool pars_branch_length;

    /** TRUE to compute bayesian branch length for the final tree */
    bool bayes_branch_length;

    /**
     *  use Least Square to evaluate NNI
     */
    bool leastSquareNNI;

    /**
     *  epsilon value used to compare log-likelihood between trees
     */
    double loglh_epsilon;

    /*
     *  reinsert leaves back to tree using parsimony
     */
    bool reinsert_par;

    /*
     *  Option to evaluate 10 different starting tree and take the best
     */
    bool bestStart;

    /**
     *  Maximum running time of the tree search in minutes
     */
    double maxtime;

    /**
     *  Turn on parsimony branch length estimation
     */
    bool parbran;

    /**
     *  option to turn on phylogenetic library
     */
    bool pll;

    /**
     *  OBSOLETE! Stopping rule for the tree search
     */
//    bool autostop;

    /**
     *  Number of maximum unsuccessful iterations after the search is stopped.
     *  Used for the automatic stopping rule
     */
    int unsuccess_iteration;

    char *binary_aln_file;

    /**
     *  the speed up heuristic will be used after
     *  speedup_iter iteration
     */
    int speedup_iter;

    /**
     *  starting CPU time of the program
     */
    double startCPUTime;

    /** starting real time of the program */
    double start_real_time;

    /**
     *  Number iteration = num_taxa * iteration_multiple
     */
    int iteration_multiple;
    /**
             input file name
     */
    char *user_file;

    /* type of starting tree */
    START_TREE_TYPE start_tree;
    std::string start_tree_subtype_name;

    /** TRUE to infer fast ML tree for ModelFinder */
    bool modelfinder_ml_tree;
    
    /** TRUE to perform final model optimization */
    bool final_model_opt;
    
    /** name of constraint tree file in NEWICK format */
    char *constraint_tree_file;

    /**
            prefix of the output file, default is the same as input file
     */
    char *out_prefix;

    /**
            alignment file name
     */
    char *aln_file;

    /** true if sequential phylip format is used, default: false (interleaved format) */
    bool phylip_sequential_format;

    /**
     SYMTEST_NONE to not perform test of symmetry of Jermiin et al. (default)
     SYMTEST_MAXDIV to perform symmetry test on the pair with maximum divergence
     SYMTEST_BINOM to perform binomial test of all pair p-values
    */
    SymTest symtest;
    
    /** TRUE to do symtest then exist */
    bool symtest_only;
    
    /**
     1 to remove bad loci by SymTest
     2 to remove good loci by SymTest
     */
    int symtest_remove;
    
    /** true to keep zero which may result in many taxon pairs not testable (default: false) */
    bool symtest_keep_zero;
    
    /**
        which test used when removing loci
        0 test of symmetry (default)
        1 test of marginal symmetry
        2 test of internal symmetry
     */
    int symtest_type;
    
    /** pvalue cutoff (default: 0.05) */
    double symtest_pcutoff;

    /** TRUE to print all pairwise statistics */
    double symtest_stat;

    /** Times to shuffle characters within columns of the alignment */
    int symtest_shuffle;

    /**
            file containing multiple trees to evaluate at the end
     */
    string treeset_file;

    /** number of bootstrap replicates for tree topology test */
    int topotest_replicates;

    /** TRUE to optimize model parameters for topology test,
     FALSE (default) to only optimize branch lengths */
    bool topotest_optimize_model;

    /** true to perform weighted SH and KH test */
    bool do_weighted_test;

    /** true to do the approximately unbiased (AU) test */
    bool do_au_test;

    /**
            file specifying partition model
     */
    char *partition_file;

    /**
     *      IMPORTANT (2012-12-21): refactor this variable as below
     * 		defines the relation between edge lengths in supertree and subtrees
     * 		BRLEN_OPTIMIZE (0) for separate edge length (default)
     * 		BRLEN_FIX (1) for joint edge length
     * 		BRLEN_SCALE (2) for proportional edge length
     */
    int partition_type;

    /** PartitionFinder algorithm, default MERGE_NONE */
    PartitionMerge partition_merge;
    
    /** percentage for rcluster algorithm like PartitionFinder */
    double partfinder_rcluster; 

    /** absolute limit on #partition pairs for rcluster algorithm */
    size_t partfinder_rcluster_max;

    /** set of models used for model merging phase */
    string merge_models;

    /** set of rate models used for model merging phase */
    string merge_rates;

    /** use logarithm of rates for clustering algorithm */
    bool partfinder_log_rate;
    
    /************************************************/
    /******* variables for Terrace analysis *********/
    
    /** remove all-gap sequences in partition model to account for terrace default: TRUE */
    bool remove_empty_seq;

    /** use terrace aware data structure for partition models, default: TRUE */
    bool terrace_aware;

    /** check if the tree lies on a terrace using terraphast lib*/
    bool terrace_analysis_tphast;
    
    /**
        following partition model analysis, check if the inferred tree lies on a terrace
     */
    bool terrace_check;
    
    /*
       Variables below are used for terrace analysis using general algorithm (TerraGen)
     */
    bool terrace_analysis;
    const char *pr_ab_matrix;
    bool print_terrace_trees;
    bool print_induced_trees;
    bool print_pr_ab_matrix;
    bool print_m_overlap;
    
    /** file with tree set to be tested, whether from the same terrace with the representative tree */
    char *terrace_query_set;
    
    /**
     *  Options to set different stopping rules for generation of terrace trees
     */
    
    int terrace_stop_intermediate_num;
    int terrace_stop_terrace_trees_num;
    double terrace_stop_time;
    
    bool terrace_non_stop;
    
    /**
        Start generation process from a larger initial tree, i.e. remove m leaves and start generation process as in the typical analysis. This does not guarantee generating all trees, but can help investigate complicated datasets, if there are more than 1 tree on a terrace.
     */
    int terrace_remove_m_leaves;
    
    /**
        number of terrace trees to be output to the file: default 100K
     */
    int terrace_print_lim;
    
    /**
        flag: only order pr_ab_matrix
     */
    bool matrix_order;
    
    /*
        generate all NNI neighbours for the input tree and output them (not terrace-specific variable)
     */
    bool gen_all_NNI;

    /************************************************/
    
    /**
            B, D, or P for Binary, DNA, or Protein sequences
     */
    char *sequence_type;

    /**
            alignment output file name
     */
    char *aln_output;

    /**
            file containing site likelihood as input for 'guided bootstrap' (added by MA)
     */
    char *siteLL_file;

    /**
            alignment where the gappy patterns will be superimposed into the input alignment
     */
    char *gap_masked_aln;

    /**
            alignment to be concatenated into the input alignment
     */
    char *concatenate_aln;

    /**
            file containing list of sites posititon to keep, format:
            pos1 pos2
            ....
     */
    char *aln_site_list;

    /**
            name of the reference sequence where aln_site_list is based on,
            NULL to take alignment positions.
     */
    char *ref_seq_name;

    /**
            alignment output format
     */
    InputType aln_output_format;
    
    /**
        output file format
     */
    FileFormat output_format;

    /**
     tree in extended newick format with node label like [&label=""]
     */
    bool newick_extended_format;
    
    /**
            TRUE to discard all gappy positions
     */
    bool aln_nogaps;

    /**
     * TRUE to discard all constant sites
     */
    bool aln_no_const_sites;

    /** TRUE to print .alninfo file */
    bool print_aln_info;

    /**
            OBSOLETE compute parsimony score on trees
     */
//    bool parsimony;

    /**
            compute random step-wise addition parsimony tree instead of BIONJ
     */
//    bool parsimony_tree;

    /**
             output file name
     */
    char *out_file;

    /**
             size of the maximal PD-tree
     */
    int sub_size;

    /**
             min size of the maximal PD-tree
             used to calculate all PD-k trees from min_size to sub_size
     */
    int min_size;

    /**
            step_size when running from min_size to sub_size
     */
    int step_size;

    /**
            conservation proprotion, another way of input set size
     */
    double pd_proportion;

    /**
            min conservation proprotion
     */
    double min_proportion;

    /**
            step conservation proprotion
     */
    double step_proportion;

    /**
            sample size for computing PD distribution
     */
    int sample_size;


    /**
            TRUE if want to find all optimal PD-k set
            with the same maximal PD score
     */
    bool find_all;

    /**
             type of random tree to be generated
     */
    TreeGenType tree_gen;

    /**
            when generating random split graph, specify the number of
            splits here!
     */
    int num_splits;

    /**
             running mode: which algorithms to be applied
     */
    RunMode run_mode;

    /**
             real running mode if run_mode == DETECTED
     */
    RunMode detected_mode;

    /**
             parameter file
     */
    char *param_file;

    /**
            file containing taxa names to be included into the PD-tree
     */
    char *initial_file;

    /**
            file containing area names to be included into the PD set
     */
    char *initial_area_file;

    /**
            file containing a list of specific taxa sets which user wants
            to compute PD score on these sets only
     */
    char *pdtaxa_file;

    /**
            sets relation file, in form of a distance matrix file
     */
    char *areas_boundary_file;

    /**
            boundary length modifier
     */
    double boundary_modifier;

    /**
            output file to store the distance matrix
     */
    char *dist_file;

    /**
            TRUE to compute the observed distances instead of Juke-Cantor distances, default: FALSE
     */
    bool compute_obs_dist;

    /**
            TRUE to compute the Juke-Cantor distances, default: FALSE
     */
    bool compute_jc_dist;
    
    
    /**
            TRUE to use experimental implementation to calculate observed (or Jukes-Cantor) distances
     */
    bool experimental;

    /**
            TRUE to compute the maximum-likelihood distances
     */
    bool compute_ml_dist;

    /**
            TRUE to compute the maximum-likelihood tree
     */
    bool compute_ml_tree;

    /**
            TRUE to compute *only* the maximum-likelihood tree
      (without using parsimony or observed distances to compute an NJ tree first)
     */
    bool compute_ml_tree_only;
    /**
            file containing budget information
     */
    char *budget_file;

    /**
            used when generating pair of taxa set with overlapping
     */
    int overlap;

    // private use
    /**
             number of times to repeat the algorithms
     */
    int repeated_time;

    /**
             print no tree to output
     */
    int nr_output;

    /**
            input type, tree or splits graph
     */
    InputType intype;

    /**
            total budget, for cost constrained PD problem
     */
    int budget;

    /**
            minimum budget, for cost constrained PD problem
     */
    int min_budget;

    /**
            step_budget when running from min_budget to budget
     */
    int step_budget;

    /**
            name of the root taxon
     */
    const char *root;

    /**
            true if tree is forced to be rooted
     */
    bool is_rooted;

    /**
        maximum distance to move root
     */
    int root_move_dist;

    /**
     TRUE to find best root when optimizing model
     */
    bool root_find;

    /**
     TRUE to test all rooting positions at the end of the run
     */
    bool root_test;

    /**
            min branch length, used to create random tree/network
     */
    double min_len;

    /**
            mean branch length, used to create random tree/network
     */
    double mean_len;

    /**
            max branch length, used to create random tree/network
     */
    double max_len;

    /**
            number of internal branches to set zero length
     */
    int num_zero_len;

    /**
            random number seed
     */
    int ran_seed;

    /**
            run time of the algorithm
     */
    double run_time;

    /**
            limit on the number of optimal PD sets
     */
    int pd_limit;

    /**
            TRUE if one wants to calculate the PD gain matrix in terms of delta_k^j = pd(PD_k \/ {j}) - pd_k
     */
    bool calc_pdgain;

    /**
            TRUE if tree file contains more than 1 tree
     */
    bool multi_tree;

    /**
            2nd user tree used in assignBootstrapSupport
     */
    char *second_tree;

    /** 
        tag each branch with the tree ID where it occurs; "ALL" to tag all branches
    */
    char *support_tag;

    /**
        number of quartets for site concordance factor
     */
    int site_concordance;
    
    /**
        0 (default): do the published sCF (Minh, Matt, Rob 2020)
        1 to compute site concordance using ancestral states at four surrounding nodes
        2 to use ancestral state based on separate subtrees
    */
    int ancestral_site_concordance;

    /**
     TRUE to print concordant sites per partition
     */
    bool site_concordance_partition;
    
    /** TRUE to print sCF for all sampled quartet */
    bool print_cf_quartets;

    /** TRUE to print trees associated with discordance factor 1 (NNI-1 tree) */
    bool print_df1_trees;
    
    /** 1 to compute internode certainty */
    int internode_certainty;
    
    /**
            2nd alignment used in computing multinomialProb (Added by MA)
     */
    char *second_align;
    /**
            type of consensus building
     */
    ConsensusType consensus_type;

    /**
            file containing weights for every tree in the input tree file
     */
    char *tree_weight_file;

    /**
            set the TRUE if want to find the minimal PD set, instead of the default maximal PD set
     */
    bool find_pd_min;

    /**
            set TRUE to find area's endemic PD instead of regular PD
     */
    bool endemic_pd;

    /**
            set TRUE to find exclusive PD instead of regular PD
     */
    bool exclusive_pd;

    /**
            to find PD complementarity given this area
     */
    char *complement_area;

    /**
            used for likelihood mapping: for each branch, print the four cluster
     */
    int branch_cluster;

    /**
            file containing taxa order
     */
    char *taxa_order_file;

    /**
            to scale branch length or clade support with a factor
     */
    double scaling_factor;

    /**
            TRUE if always use binary linear programming
     */
    bool binary_programming;

    /**
            test the input split system in one of the TestType
     */
    TestType test_input;

    /**
            burnin value: number of beginning trees to be discarded
     */
    int tree_burnin;

    /**
            maximum number of trees to consider (for e.g. consensus tree construction)
     */
    int tree_max_count;

    /**
        threshold of split frequency, splits appear less than threshold will be discarded
     */
    double split_threshold;

    /**
        thresholds of split frequency with back-slash separator
     */
    char* split_threshold_str;

    /**
            threshold of split weight, splits with weight less than or equal to threshold will be discarded
     */
    double split_weight_threshold;

    /** TRUE to collapse zero branches, default FALSE */
    bool collapse_zero_branch;

    /**
            Way to summarize split weight in the consensus tree or network: SW_SUM, SW_AVG_ALL, or SW_AVG_PRESENT
     */
    double split_weight_summary;

    /**
            TRUE if use quadratic programming (for GUROBI)
     */
    bool quad_programming;

    /**
            true if one wants to optimize tree by subtree pruning and regrafting
     */
    bool tree_spr;

    /**
            true if printing out of optimal sets in NEXUS format
     */
    bool nexus_output;

    /**
            k-representative parameter, used for IQP algorithm
     */
    int k_representative;

    /**
            probability of deleting a leaf, used for IQP algorithm
     */
    double p_delete;

    /**
            min number of iqpnni iterations
     */
    int min_iterations;

    /**
            max number of iqpnni iterations
     */
    int max_iterations;

    /**
            stop condition, SC_FIXED_ITERATION or SC_STOP_PREDICT
     */
    STOP_CONDITION stop_condition;

    /**
            confidence value for stop rule
     */
    double stop_confidence;

    /** number iterations for parameter optimization, default: 100 */
    int num_param_iterations;

    /** number of independent runs (-nrun option) */
    int num_runs;
    
    /**
            name of the substitution model (e.g., HKY, GTR, TN+I+G, JC+G, etc.)
     */
    string model_name;

    /** model name to initialize GTR20 or NONREV protein model */
    char* model_name_init;

    /** number of steps for linked model optimisation, default: 1 */
    int model_opt_steps;
    
    /** set of models for testing */
    string model_set;

    /** set of models to be added into default set */
    char *model_extra_set;

    /** subset of models for testing, e.g. viral, mitochondrial */
    char *model_subset;

    /** set of state frequencies model for testing */
    char *state_freq_set;

    /** set of rate heterogeneity model for testing */
    string ratehet_set;

    /** all models with score worse than the best score + this threshold will be ignored */
    double score_diff_thres;
    
    /** model defition file */
    char *model_def_file;

    /** TRUE to perform ModelOMatic method of Whelan et al. 2015 */
    bool modelomatic;
    
    /** true to redo model testing even if .model file exists */
    bool model_test_again;

    /** 0: use the same tree for model testing 
        1: estimate tree for each model, but initialize the tree for next model 
           by the tree reconstructed from the previous model
        2: estimate tree for each model independently
        */
    short int model_test_and_tree;

    /** true to fist test equal rate model, then test rate heterogeneity (default: false) */
    bool model_test_separate_rate;

    /** TRUE to optimize mixture model weights */
    bool optimize_mixmodel_weight;

    /** number of mixture branch lengths, default 1 */
    int num_mixlen;
    /** TRUE to always optimize rate matrix even if user parameters are specified in e.g. GTR{1,2,3,4,5} */
    bool optimize_rate_matrix;

    /**
            TRUE to store transition matrix into a hash table for computation efficiency
     */
    bool store_trans_matrix;

    /**
            state frequency type
     */
    StateFreqType freq_type;

    /** FALSE to set zero state frequency to 1e-4.
        Default: FALSE (version <= 1.5.5), TRUE (ver >= 1.5.6) */
    bool keep_zero_freq;
    
    /** minimal state frequency for optimisation, default=0.0001 */
    double min_state_freq;


    /**
            the number of rate categories
     */
    int num_rate_cats;

    /**
            maximum number of rate categories
     */
    int min_rate_cats;

    /**
            maximum number of rate categories
     */
    int max_rate_cats;

    /**
            shape parameter (alpha) of the Gamma distribution for site rates
     */
    double gamma_shape;

    /**
            minimum shape parameter (alpha) of the Gamma distribution for site rates
     */
    double min_gamma_shape;

    /**
            TRUE to use median rate for discrete categories, FALSE to use mean rate instead
     */
    bool gamma_median;

    /**
            proportion of invariable sites
     */
    double p_invar_sites;

    /** TRUE to optimize all model and rate parameters jointly by BFGS, default: FALSE */
    bool optimize_model_rate_joint;

    /**
            TRUE if you want to optimize branch lengths by Newton-Raphson method
     */
    bool optimize_by_newton;

    /** optimization algorithm for free rate model: 1-BFGS, 2-BFGS, EM */
    string optimize_alg_freerate;

    /** optimization algorithm for mixture (heterotachy) branch length models */
    string optimize_alg_mixlen;

    /**
     *  Optimization algorithm for +I+G
     */
    string optimize_alg_gammai;
    
    /**
     *  Optimization algorithm for tree weights in tree-mixture model
     */
    string optimize_alg_treeweight;

    /**
     * If given model parameters on command line (e.g. -m RY3.4{0.2,-0.4})
     * treat these as fixed model parameters (if false), or treat them as 
     * starting point for optimization search (if true)?
     */

    bool optimize_from_given_params;

    /**
            BRLEN_OPTIMIZE optimize branch lengths during model optimization
            BRLEN_FIX      fix branch lengths during model optimization
            BRLEN_SCALE    scale all branch lengths by the same factor during model optimization
     */
    int fixed_branch_length;

    /** minimum branch length for optimization, default 0.000001 */
    double min_branch_length;

    /** maximum branch length for optimization, default 100 */
    double max_branch_length;


    /**
            criterion to assess important quartet
     */
    IQP_ASSESS_QUARTET iqp_assess_quartet;

    /**
     *      Using IQP algorithm to do tree perturbation
     */
    bool iqp;

    /**
            the LP file is in gurobi format or not
     */
    bool gurobi_format;

    /**
            number of threads for gurobi call
     */
    bool gurobi_threads;

    /**
            TRUE if doing bootstrap on the input trees (good, bad, ugly)
     */
    int num_bootstrap_samples;

    /** bootstrap specification of the form "l1:b1,l2:b2,...,lk:bk"
        to randomly draw b1 sites from the first l1 sites, etc. Note that l1+l2+...+lk
        must equal m, where m is the alignment length. Otherwise, an error will occur.
        The default bootstrap_spec == NULL, a standard procedure is applied, i.e., randomly draw m sites.
    */
    char *bootstrap_spec;

    /** 1 or 2 to perform transfer boostrap expectation (TBE) */
    int transfer_bootstrap;
    
    /** subsampling some number of partitions / sites for analysis */
    int subsampling;

    /** random seed number for subsampling */
    int subsampling_seed;

    /**
            1 if output all intermediate trees (initial trees, NNI-optimal trees and trees after each NNI step)
            2 if output all intermediate trees + 1-NNI-away trees
     */
    int write_intermediate_trees;

    /**
     *  Write all distinct intermediate trees and there likelihoods
     *  Note: intermediate trees are trees that have been visited by the search. These include trees created by
     *  NNI-steps within each NNI iteration.
     */
    bool writeDistImdTrees;

    /**
     *  Write trees obtained at the end of each NNI search
     */
    bool write_candidate_trees;


    /**
        TRUE to avoid duplicated trees while writing intermediate trees
     */
//    bool avoid_duplicated_trees;

    /**
            Robinson-Foulds distance computation mode: RF_ADJACENT PAIR, RF_ALL_PAIR
     */
    int rf_dist_mode;

    /**
     true to compute distance between the same k-th tree in two sets
     */
    bool rf_same_pair;
    
    /**
     true to normalize tree distances, false otherwise
     */
    bool normalize_tree_dist;
    
    /**
            compute the site-specific rates by Meyer & von Haeseler method
     */
    bool mvh_site_rate;

    /**
            FALSE to use MH Model, FALSE for using tree-likelihood
     */
    bool rate_mh_type;

    /**
            TRUE to discard saturated for Meyer & von Haeseler (2003) model
     */
    bool discard_saturated_site;

    /**
            rates will be normalized to this mean value
     */
    double mean_rate;

    /**
            Percentage threshold to accept a branch of the approximate likelihood ratio test
            (aLRT) with SH-like interpretation. See Guindon et al. (2010) Syst. Biol. for details.
            Default: 90%.
     */
    int aLRT_threshold;

    /**
            number of replicates, default: 1000
     */
    int aLRT_replicates;

    /** true to perform aLRT branch test of Anisimova & Gascuel (2006) */
    bool aLRT_test;

    /** true to perform aBayes branch test of Anisimova et al (2011) */
    bool aBayes_test;

    /**
            number of replicates for local bootstrap probabilities method of Adachi & Hasegawa (1996) in MOLPHY
     */
    int localbp_replicates;

    /**
            SSE Option
     */
    LikelihoodKernel SSE;

    /** TRUE for safe numerical scaling (per category; used for large trees), default: FALSE */
    bool lk_safe_scaling;

    /** minimum number of sequences to always use safe scaling, default: 2000 */
    int numseq_safe_scaling;

    /** TRUE to force using non-reversible likelihood kernel */
    bool kernel_nonrev;

    /**
     	 	WSL_NONE: do not print anything
            WSL_SITE: print site log-likelihood
            WSL_RATECAT: print site log-likelihood per rate category
            WSL_MIXTURE: print site log-likelihood per mixture class
            WSL_MIXTURE_RATECAT: print site log-likelihood per mixture class per rate category
            WSL_STATE: print site log-likelihood per state
     */
    SiteLoglType print_site_lh;

    /** TRUE to print partition log-likelihood, default: FALSE */
    bool print_partition_lh;

    /**
        control printing posterior probability of each site belonging to a rate/mixture categories
        same meaning as print_site_lh, but results are printed to .siteprob file
        WSL_RATECAT: print site probability per rate category
        WSL_MIXTURE: print site probability per mixture class
        WSL_MIXTURE_RATECAT: print site probability per mixture class per rate category
    */
    SiteLoglType print_site_prob;

    /**
        AST_NONE: do not print ancestral sequences (default)
        AST_MARGINAL: print ancestral sequences by marginal reconstruction
        AST_JOINT: print ancestral sequences by joint reconstruction
    */
    AncestralSeqType print_ancestral_sequence;

    /** minimum probability to assign an ancestral state */
    double min_ancestral_prob;

    /**
        0: print nothing
        1: print site state frequency vectors
    */
    SiteFreqType print_site_state_freq;

    /**
     0 (default): do not print .rate file
     1: print site-specific rates by empirical Bayes
     2: site-specific rates by maximum-likelihood
     */
    int print_site_rate;

    /* 1: print site posterior probability for many trees during tree search */
    int print_trees_site_posterior;

    /**
            TRUE to print tree log-likelihood
     */
    bool print_tree_lh;

    bool print_branch_lengths;

    /****** adaptive NNI search heuristic ******/

    /**
     *  Output log-likelihood
     */
    bool nni_lh;

    /**
     *  The number of iqp iteration before the heuristics is applied
     */
    int speedUpFromIter;

    /**
     *  Lambda in PhyML algorithm
     */
    double lambda;

    /**
     * Confidence level for the speed up heuristics
     */
    double speed_conf;

    bool new_heuristic;

    /***** WH-test (Weiss & von Haeseler 2003) *****/

    /**
            Results of Weiss & Haeseler test of model homogeneity
     */
    double whtest_simulations;
    double whtest_delta;
    double whtest_delta_quantile;
    double whtest_p_value;


    /**
            bit-wise type including MCAT_LOG, MCAT_MEAN
     */
    int mcat_type;

    /**
            initial rate file in format:
            Site Rate
            1  f_1
            2  f_2
            ...
     */
    char *rate_file;

    /***** NGS stuffs   ********/

    /**
            next-generation sequencing input file for Fritz project
     */
    char *ngs_file;

    /**
            next-generation sequencing input file containing mapped reads to the reference genome
     */
    char *ngs_mapped_reads;

    bool ngs_ignore_gaps;

    bool do_pars_multistate;

    /**
            File containing p-values of the genes, for GSS project with Roland
     */
    char *gene_pvalue_file;

    /**
            scaling factor for the p-values
     */
    double gene_scale_factor;

    /**
            transforming pvalues to logarithms
     */
    bool gene_pvalue_loga;

    /***** variables for reading NCBI taxonomy tree *******/

    /**
            NCBI taxonomy ID, for processing nodes.dmp file
     */
    int ncbi_taxid;

    /**
            NCBI taxon rank, restricting the tree to that rank
     */
    const char *ncbi_taxon_level;

    /**
            rank to ingore, e.g., "no rank", branch length to such node will be set to zero
     */
    const char *ncbi_ignore_level;

    /**
            typically names.dmp from NCBI
     */
    const char *ncbi_names_file;

    /**********************************************/
    /******* variables for ECOpd analysis *********/

	/**
		eco_dag_file - contains the food web in matrix form (n species, nxn matrix), 0 for no connection, 1 for predation of j predator on i prey
	*/
	char *eco_dag_file;

    /**
		eco_detail_file - contains IDs of species present in the final set and/or species absent in the TREE or SPLIT system, but present in the food web
	*/
	const char *eco_detail_file;

	/*
	 * the type of the phylo input - tree or network
	 */
	const char *eco_type;

	/*
		k% - percent of species to be conserved
	 */
	int k_percent;

    /*
		diet - percent of species diet to be preserved for species survival
	*/
	int diet_min;
	int diet_max;
	int diet_step;

    /*
		eco_run - run number, used when random branch length is assigned to the edges of an input tree
	*/
	int eco_run;

    /*
		eco_weighted - indicates whether to treat the food web as weighted or not weighted
	*/
	bool eco_weighted;

    /**********************************************/
    /****** variables for upper bound tests *******/
	bool upper_bound;
	bool upper_bound_NNI;
	/*
	 * fraction of current likelihood by which UB will be increased.
	 * if UBincreased < L, ignore corresponding NNI Add a comment to this line
	 */
	double upper_bound_frac;


    /**********************************************/
    /**** variables for ultra-fast bootstrap ******/

    /**
            number of replicates for guided bootstrap
     */
    int gbo_replicates;

	/* interval (l-epsilon,l+epsilon) indicates tie for bootstrap tree
	 * in this case, one tree is picked up at random
	 */
	double ufboot_epsilon;

    /**
            TRUE to check with different max_candidate_trees
     */
    int check_gbo_sample_size;

    /**
            TRUE to use RELL method of Simodaira Hasegawa, FALSE otherwise
     */
    bool use_rell_method;

    /**
            TRUE to use ELW method of Strimmer & Rambaut for new bootstrap, FALSE otherwise
     */
    bool use_elw_method;

    /**
            TRUE to weight each bootstrap sample by its probability, FALSE otherwise
     */
    bool use_weighted_bootstrap;

    /**
            TRUE to use the single ML tree per bootstrap, FALSE to include several sup-optima
     */
    bool use_max_tree_per_bootstrap;

    /** maximum number of candidate trees to consider for new bootstrap */
    int max_candidate_trees;

    /** TRUE if user_file contains topologically distinct trees */
    bool distinct_trees;

    /** NEW: TRUE to update bootstrap trees during the search (do not store treels_ptnlh).
            FALSE to call runGuidedBootstrap() at the end */
    bool online_bootstrap;

    /** minimal correlation coefficient for bootstrap stopping rule */
    double min_correlation;

    /** number of iterations between bootstrap stopping rule check */
    int step_iterations;

    /** TRUE to store all candidate trees in memory */
//    bool store_candidate_trees;

	/** true to print all UFBoot trees to a file */
	int print_ufboot_trees;

    /**********************************************/
    /**** variables for jackknife ******************/

    /** proportion of sites to be dropped in jackknife */
    double jackknife_prop;
    
    /**********************************************/
    /* variables for robust phylogeny (Lanfear & Holland project */
    
    /** proportion of sites to keep in robust phylogeny idea */
    double robust_phy_keep;

    /** use median log-likelihood instead of sum log-likelihood */
    double robust_median;
    
    /****** variables for NNI cutoff heuristics ******/

    /**
            TRUE to empirically estimate nni_cutoff
     */
    bool estimate_nni_cutoff;

    /**
            logl difference with zero-branch tree, to cutoff before evaluating NNI
     */
    double nni_cutoff;

    /**
            sort the NNI before evaluating
     */
    bool nni_sort;

    /**
            Obsolete: TRUE to optimize 5 branches around NNI
     */
    //bool nni_opt_5branches;

    /** print some output info for NNI */
    bool testNNI;

    /** TRUE to do approximate NNIs with approximate branch lengths before a normal NNI */
    bool approximate_nni;


    /** TRUE to compress big file using zlib */
    bool do_compression;

    /**
            number of bootstrap samples for AvH curiosity
     */
    int avh_test;

    /**
            number of bootstrap samples for Arndt's bootstrap plot
     */
    int bootlh_test;

    /**
            partition definition for Arndt's bootstrap plot
     */
    char* bootlh_partitions;

    /** precision when printing out for floating-point number */
    int numeric_precision;

    /** file containing state-frequencies per site for site-specific state frequency model
     * each line has n+1 entries (n=number of states):
     * site_ID state1_freq state2_freq ... staten_freq
     * where site_ID is from 1 to m (m=number of sites)
     */
    char *site_freq_file;

    /**
        user tree file used to estimate site-specific state frequency model 
    */
    char *tree_freq_file;

    /** number of threads for OpenMP version     */
    int num_threads;
    
    /** maximum number of threads, default: #CPU scores  */
    int num_threads_max;
    
    /** true to parallel ModelFinder by models instead of sites */
    bool openmp_by_model;

    /** either MTC_AIC, MTC_AICc, MTC_BIC */
    ModelTestCriterion model_test_criterion;

    /** either MTC_AIC, MTC_AICc, MTC_BIC, or MTC_ALL to stop +R increasing categories */
//    ModelTestCriterion model_test_stop_rule;

    /** sample size for AICc and BIC */
    int model_test_sample_size;

    /** root state, for Tina's zoombie domain */
    char *root_state;

	/**
	 * TRUE to print bootstrap alignments, default: false
	 */
	bool print_bootaln;

    /** TRUE to print bootstrapped site frequency for e.g. PMSF */
    bool print_boot_site_freq;

	/** true to print sub alignments of super alignment, default: false */
	bool print_subaln;

	/** print partition information */
	bool print_partition_info;

	/** TRUE to print concatenated alignment, default: false */
	bool print_conaln;

	/** TRUE to link alpha among Gamma model over partitions */
	bool link_alpha;

    /** TRUE to link substitution models over partitions */
    bool link_model;

    /** name of the joint model across partitions */
    char* model_joint;
    
	/** true to count all distinct trees visited during tree search */
	bool count_trees;

    /// True if PoMo is run; otherwise false.
    bool pomo;

    /// True if sampled input method is used (-st CR..); otherwise false.
    bool pomo_random_sampling;

	/// Virtual population size for PoMo model.
	int pomo_pop_size;

	/* -1 (auto-detect): will be set to 0 if there is enough memory, 1 otherwise
	 * 0: store all partial likelihood vectors
	 * 1: only store 1 partial likelihood vector per node */
	LhMemSave lh_mem_save;
    
    /** true to save buffer, default: false */
    bool buffer_mem_save;

    /** maximum size of memory allowed to use */
    double max_mem_size;

	/* TRUE to print .splits file in star-dot format */
	bool print_splits_file;
    
    /* TRUE to print .splits.nex file in NEXUS format */
    bool print_splits_nex_file;

    
    /** TRUE (default) to ignore identical sequences and add them back at the end */
    bool ignore_identical_seqs;

    /** TRUE to write initial tree to a file (default: false) */
    bool write_init_tree;

    /** TRUE to write branch lengths of partition trees for each branch of supertree */
    bool write_branches;
    
    /** frequencies of const patterns to be inserted into alignment */
    char *freq_const_patterns;
    /** BQM 2015-02-25: true to NOT rescale Gamma+Invar rates by (1-p_invar) */
    bool no_rescale_gamma_invar;

    /** true to compute sequence identity along tree */
    bool compute_seq_identity_along_tree;
    
    /** true to compute sequence composition */
    bool compute_seq_composition;
    
    /** true to ignore checkpoint file */
    bool ignore_checkpoint;
    /** number of quartets for likelihood mapping */
    int64_t lmap_num_quartets;

    /**
            file containing the cluster information for clustered likelihood mapping
     */
    char *lmap_cluster_file;

    /** time (in seconds) between checkpoint dump */
    int checkpoint_dump_interval;
    /** TRUE to print quartet log-likelihoods to .quartetlh file */
    bool print_lmap_quartet_lh;

    /** true if ignoring the "finished" flag in checkpoint file */
    bool force_unfinished;
    
    /** TRUE to print checkpoints to 1.ckp.gz, 2.ckp.gz,... */
    bool print_all_checkpoints;

    /** control output files to be written
     * OUT_LOG
     * OUT_TREEFILE
     * OUT_IQTREE
     */
    int suppress_output_flags;

    /** matrix exponentiation technique for nonreversible models, either 
        MET_SCALING_SQUARING 
        MET_EIGEN_DECOMPOSITION 
        MET_LIE_MARKOV_DECOMPOSITION
    */
    MatrixExpTechnique matrix_exp_technique;

    /**
     * Diep:
     * Data members for UFBoot2-Corr
     */
	bool ufboot2corr; // to turn on the correction mode for UFBoot under model violations, enable by "-bb <nrep> -correct
	bool u2c_nni5; // to use NNI5 during Refinement Step of UFBoot2-Corr

    /** method for phylogenetic dating, currently only LSD is supported */
    string dating_method;

    /** extra commands passed to the dating method */
    string dating_options;

    /** date file that has several lines, each line with a taxon name and date in YYYY-MM-DD */
    string date_file;
    
    /** tip date, a real number or YYYY-MM-DD */
    string date_tip;
    
    /** root date, a real number or YYYY-MM-DD */
    string date_root;
    
    /** false to remove outgroup from the dated tree, default: true */
    bool date_with_outgroup;
    
    /** true to print internal date files for debugging purpose */
    bool date_debug;
    
    /** number of replicates to compute date confidence interval */
    int date_replicates;
    
    /** standard deviation of lognormal relaxed clock model for confidence interval estimate */
    double clock_stddev;

    /** z-score for detecting outlier nodes */
    double date_outlier;

    /** supress the list of sequences */
    double suppress_list_of_sequences;

    /** supress warnings about low or zero distances */
    double suppress_zero_distance_warnings;

    /** supress notes about duplicate sequences */
    double suppress_duplicate_sequence_warnings;
    
    /**
    *  TRUE to execute alisim
    */
    bool alisim_active;
    
    /**
    *  TRUE if AliSim is running with inference mode
    */
    bool alisim_inference_mode;
    
    /**
    *  TRUE to disable copying gaps from input sequences
    */
    bool alisim_no_copy_gaps;
    
    /**
    *  original parameters
    */
    string original_params;
    
    /**
    *  output filename
    */
    string alisim_output_filename;
    
    /**
    *  length of output sequences of alisim
    */
    int alisim_sequence_length;
    
    /**
    *  number of output datasets of alisim
    */
    int alisim_dataset_num;
    
    /**
    *  alignment_filepath containing the ancestral sequence of alisim
    */
    char * alisim_ancestral_sequence_aln_filepath;
    
    /**
    *  the sequence name of the ancestral sequence of alisim
    */
    string alisim_ancestral_sequence_name;
    
    /**
    *  the maximum number of rate_categories that cached_trans_matrix could be applied
    */
    int alisim_max_rate_categories_for_applying_caching;
    
    /**
    *  number of states (SEQ_MORPH)
    */
    int alisim_num_states_morph;
    
    /**
    *  number of taxa
    */
    int alisim_num_taxa_uniform_start;
    int alisim_num_taxa_uniform_end;
    IntVector alisim_num_taxa_list;
    
    /**
    *  birth_rate (Birth_Death model)
    */
    double birth_rate;
    
    /**
    *  death_rate (Birth_Death model)
    */
    double death_rate;
    
    /**
    *  rate to generate abundant states (with +ASC)
    *  default is 2.0;
    */
    double alisim_length_ratio;
    
    /**
    *  fundi model - set of taxa
    */
    vector<string> alisim_fundi_taxon_set;
    
    /**
    *  fundi model - proportion
    */
    double alisim_fundi_proportion;
    
    /**
    *  distribution_definition_file
    */
    char* alisim_distribution_definitions;
    
    /**
    *  TRUE to skip checking the memory capacity for large mixture models
    */
    bool alisim_skip_checking_memory;
    
    /**
    *  TRUE to write sequences of internal nodes
    */
    bool alisim_write_internal_sequences;
    
    /**
    *  list of distributions to generate random numbers
    */
    map<string, Distribution> distributions;
    
    /**
    *  TRUE to convert tree from rooted to unrooted and stop
    */
    bool alisim_only_unroot_tree;
    
    /**
    *  distribution to randomly generate branch lengths
    */
    char* branch_distribution;
    
    /**
    *  the ratio of insertion rate to substitution rate
    */
    double alisim_insertion_ratio;
    
    /**
    *  the ratio of deletion rate to substitution rate
    */
    double alisim_deletion_ratio;
    
    /**
    *  the insertion-distribution for drawing the number of inserted sites
    */
    IndelDistribution alisim_insertion_distribution;
    
    /**
    *  the deletion-distribution for drawing the number of deleted sites
    */
    IndelDistribution alisim_deletion_distribution;
    
    /**
    *  mean of deletion-size
    */
    double alisim_mean_deletion_size;
    
    /**
    *  threshold to switch between two simulation methods: Rate_Matrix and Trans_Prob_Matrix
    */
    double alisim_simulation_thresh;
    
    /**
    *  messages which are delayed to show
    */
    string delay_msgs;
    
    /**
    *  TRUE to disable outputing sequences without gaps (when using Indels)
    */
    bool alisim_no_export_sequence_wo_gaps;
    
    /**
    *  TRUE to enable mixture model at substitution level
    */
    bool alisim_mixture_at_sub_level;
    
    /**
    *  branch-scale factor
    */
    double alisim_branch_scale;
    
    /**
    *  TRUE to output all replicate alignments into a single file
    */
    bool alisim_single_output;
    
    /**
    *  Type to assign rate heterogeneity to sites (default: posterior mean)
    */
    ASSIGNMENT_TYPE alisim_rate_heterogeneity;
    
    /**
    *  Type to assign site freqs (in mixture) (default: posterior mean)
    */
    ASSIGNMENT_TYPE alisim_stationarity_heterogeneity;
    
    /**
    *  path to the tmp_data file to temporarily output sequences at tips when using Indels
    */
    string tmp_data_filename;
    
    /**
    *  TRUE to keep the order of output sequences
    */
    bool keep_seq_order;
    
    /**
    *  TRUE to delete output file (for testing only)
    */
    bool delete_output;
    
    /**
    *  param to rebuild the indel history when using indel
    */
    double rebuild_indel_history_param;
    
    /**
    *  factor to limit memory usage
    */
    double mem_limit_factor;
    
    /**
    *  TRUE to allow indel-rate variation
    */
    bool indel_rate_variation;
    
    /**
    *  AliSim-OpenMP algorithm
    */
    ALI_OPENMP_ALG alisim_openmp_alg;
    
    /**
    *  TRUE to skip concatenate sequence chunks from intermediate files in AliSim-OpenMP-EM algorithm
    */
    bool no_merge;
    
    /**
    *  TRUE to include predefined mutations
    */
    bool include_pre_mutations;
    
    /**
    *  Alignment index, which was used to generate different random seed for each alignment when simulating multiple alignments
    */
    int alignment_id;
    
    /**
    *  Mutation file that specifies pre-defined mutations occurs at nodes
    */
    std::string mutation_file;
    
    /**
    *  site starting index (for predefined mutations in AliSim)
    */
    int site_starting_index;
};

/**
        related measures for PD
 */
struct PDRelatedMeasures {
    /**
            names of areas
     */
    vector<string> setName;

    /**
            pd scores of areas
     */
    DoubleVector PDScore;

    /**
            exclusive PD scores
     */
    DoubleVector exclusivePD;

    /**
            endemic pd scores of an area given all other areas
     */
    DoubleVector PDEndemism;

    /**
            pd-complementarity scores of an area given some provided area
     */
    DoubleVector PDComplementarity;

};



/*--------------------------------------------------------------*/
    
inline size_t get_safe_upper_limit(size_t cur_limit) {
    if (Params::getInstance().SSE >= LK_AVX512)
        // AVX-512
        return ((cur_limit+7)/8)*8;
    else
        if (Params::getInstance().SSE >= LK_AVX)
            // AVX
            return ((cur_limit+3)/4)*4;
        else
            // SSE
            return ((cur_limit+1)/2)*2;
}

inline size_t get_safe_upper_limit_float(size_t cur_limit) {
    if (Params::getInstance().SSE >= LK_AVX512)
        // AVX-512
        return ((cur_limit+15)/16)*16;
    else
        if (Params::getInstance().SSE >= LK_AVX)
            // AVX
            return ((cur_limit+7)/8)*8;
        else
            // SSE
            return ((cur_limit+3)/4)*4;
}
/*--------------------------------------------------------------*/

/**
        @return TRUE of ch is a control character (ascii <= 32)
 */
inline bool controlchar(char ch) {
    return ch <= 32;
}

inline bool is_newick_token(char ch) {
    return ch == ':' || ch == ';' || ch == ',' || ch == ')' || ch == '(' || ch == '[' || ch == ']';
}

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

/**
        print error message then exit program
 */
//void outError(char *error);

/**
        print error message then exit program
 */
void outError(const char *error, bool quit = true);

/**
        print error message then exit program
 */
void outError(string error, bool quit = true);


/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

/**
        print double error messages then exit program
 */
void outError(const char *error, const char *msg, bool quit = true);

/**
        print double error messages then exit program
 */
void outError(const char *error, string msg, bool quit = true);

/**
        Output a warning message to screen
        @param error warning message
 */
void outWarning(const char *warn);
void outWarning(string warn);


/** safe version of std::getline to deal with files from different platforms */ 
std::istream& safeGetline(std::istream& is, std::string& t);

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/


/**
        generate a random branch length under an exponential distribution
        with mean params.mean_len. Also make sure that the resulting
        length is in the range (params.min_len, params.max_len)
        @return the random branch length
 */
double randomLen(Params &params);

/**
    Try to generate a branch length but it could be out of the range (params.min_len, params.max_len)
 */
double tryGeneratingBlength(Params &params);

/**
        Compute the logarithm of the factorial of an integer number
        @param num: the number
        @return logarithm of (num! = 1*2*...*num)
 */
double logFac(const int num);

/**
 * Function to randomly select an element in a C++ container
 *
 * @param begin
 * @param end
 * @return
 */
template <typename I>
I random_element(I begin, I end);

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/
/*
        Error messages
 */
const char ERR_NO_TAXON[] = "Find no taxon with name ";
const char ERR_NO_AREA[] = "Find no area with name ";
const char ERR_NO_ROOT[] = "Root taxon not found: ";
const char ERR_ROOT_NET[] = "-root option is not available for network";
const char ERR_CONFLICT_ROOT[] = "Tree is already rooted, -o <taxon> is not allowed.";
const char ERR_DUPLICATED_TAXA[] = "Duplicated taxa name in the tree.";
const char ERR_FEW_TAXA[] = "Number of taxa must be greater than 2.";
const char ERR_NO_SPLITS[] = "No splits found!";
const char ERR_FEW_SPLITS[] = "Number of splits must be at least equal to the number of taxa";
const char ERR_NEG_BRANCH[] = "Negative branch length not allowed.";
const char ERR_NO_MEMORY[] = "Not enough memory!";

const char ERR_READ_INPUT[] = "File not found or incorrect input, pls check it again.";
const char ERR_UNEXPECTED_EOF[] = "Unexpected end of file.";
const char ERR_READ_ANY[] = "Unidentified error while reading file, pls check it carefully again.";
const char ERR_WRITE_OUTPUT[] = "Cannot write to file ";

const char ERR_NO_K[] = "You must specify the number of taxa in the PD set.";
const char ERR_TOO_SMALL_K[] = "Size of PD-set must be at least the size of initial set.";
const char ERR_NO_BUDGET[] = "Total budget is not specified or less than zero.";
const char ERR_TOO_SMALL_BUDGET[] = "Not enough budget to conserve the initial set of taxa.";
const char ERR_INTERNAL[] = "Internal error, pls contact authors!";

/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

/**
 * convert int to string
 * @param int
 * @return string
 */
string convertIntToString(int number);
string convertInt64ToString(int64_t number);

string convertDoubleToString(double number);

/**
 case-insensitive comparison between two strings
 @return true if two strings are equal.
 */
bool iEquals(const string a, const string b);
    
/**
 *
 * @param SRC
 * @param DEST
 * @return bool
 */
bool copyFile(const char SRC[], const char DEST[]);

/**
 * Check if the file exists
 * @param strFilename
 * @return
 */
bool fileExists(string strFilename);

/**
    check that path is a directory
 */
int isDirectory(const char *path);

/**
    get all file names in a directory
    @param path directory name
    @param[out] filenames vector of file names
    return 0 if FAIL, non-zero otherwise
 */
int getFilesInDir(const char *path, StrVector &filenames);

/**
        convert string to int, with error checking
        @param str original string
        @return the number
 */
int convert_int(const char *str);

/**
    convert string to int64, with error checking
    @param str original string
    @return the number
 */
int64_t convert_int64(const char *str);

/**
        convert string to int, with error checking
        @param str original string
        @param end_pos end position
        @return the number
 */
int convert_int(const char *str, int &end_pos);

/**
        convert comma-separated string to integer vector, with error checking
        @param str original string with integers separated by comma
        @param vec (OUT) integer vector
 */
void convert_int_vec(const char *str, IntVector &vec);

/**
        convert string to int64_t, with error checking
        @param str original string
        @return the number
 */
int64_t convert_int64(const char *str);

/**
        convert string to int64_t, with error checking
        @param str original string
        @param end_pos end position
        @return the number
 */
int64_t convert_int64(const char *str, int &end_pos);

/**
        convert string to double, with error checking
        @param str original string
        @return the double
 */
double convert_double(const char *str);

/**
        convert string to double, with error checking
        @param str original string
        @param end_pos end position
        @return the double
 */
double convert_double(const char *str, int &end_pos);

/**
        convert string to double, or generate it from a distribution
        @param str original string
        @param end_pos end position
        @param separator char separating elements
        @param non_negative TRUE to only return non-negative number
        @return the double
 */
double convert_double_with_distribution(const char *str, int &end_pos, bool non_negative, char separator = ',');

/**
        convert comma-separated string to integer vector, with error checking
        @param str original string with integers separated by comma
        @param vec (OUT) integer vector
        @param separator char separating elements
 */
void convert_double_vec(const char *str, DoubleVector &vec, char separator = ',');

/**
        convert comma-separated string to double vector or generate double vector from distributions
        @param str original string with doubles separated by comma
        @param vec (OUT) double vector
        @param separator char separating elements
        @param non_negative TRUE to only return non-negative number
 */
void convert_double_vec_with_distributions(const char *str, DoubleVector &vec, bool non_negative, char separator = ',');

/**
        convert separated string to an array of double number (double*) or generate them from distributions
        @param tmp_str original string with doubles separated by separator
        @param array an array of double number (double*)
        @param num_items the number of items in the array
        @param separator char separating elements
        @param non_negative TRUE to only return non-negative number
 */
void convert_double_array_with_distributions(string tmp_str, double* array, int num_items, bool non_negative, char separator);

/**
        normalize state frequencies so that sum of them is equal to 1
        @param freqs original state frequencies
        @param num_states the number of states
        @param total_freq sum of all original state frequencies
 */
void normalize_frequencies_from_index(double* freqs, int num_states, int starting_index);

/**
        normalize state frequencies so that sum of them is equal to 1
        @param freqs original state frequencies
        @param num_states the number of states
        @param total_freq sum of all original state frequencies
 */
void normalize_frequencies(double* freqs, int num_states, double total_freqs = -1, bool show_warning = false);

/**
 * Convert seconds to hour, minute, second
 * @param sec
 * @return string represent hour, minute, second
 */
string convert_time(const double sec);


/**
        convert a string to to range lower:upper:step_size with error checking
        @param str original string
        @param lower (OUT) lower bound of the range
        @param upper (OUT) upper bound of the range
        @param step_size (OUT) step size of the range
 */
void convert_range(const char *str, int &lower, int &upper, int &step_size);

/**
        convert a string to to range lower:upper:step_size with error checking
        @param str original string
        @param lower (OUT) lower bound of the range
        @param upper (OUT) upper bound of the range
        @param step_size (OUT) step size of the range
 */
void convert_range(const char *str, double &lower, double &upper, double &step_size);

void convert_string_vec(const char *str, StrVector &str_vec, char separator = ',');

/**
        read distributions from built-in string or user-specified file
 */

void read_distributions(char* filepath = NULL);

/**
        randomly select a number from the pool of random numbers of a distribution
        @param distribution_name storing name of distribution
        @param non_negative TRUE to only return non-negative number
 */
double random_number_from_distribution(string distribution_name, bool non_negative);

/**
        initialize a number by converting string to double (if the user supplies a number) or randomly generating it from a distribution (if the user supplies a distribution name)
        @param input storing a number or a distribution name
        @param non_negative TRUE to only return non-negative number
 */
double convert_double_with_distribution(const char *str, bool non_negative);

/**
        initialize a number (with an upper bound constraint) by converting string to double (if the user supplies a number) or randomly generating it (with up to 1000 attempts) from a distribution (if the user supplies a distribution name)
        @param input storing a number or a distribution name
        @param upper_bound storing the upper bound value
        @param non_negative TRUE to only return non-negative number
 */
double convert_double_with_distribution_and_upperbound(string input, double upper_bound, bool non_negative);

/**
        randomly generating a double (with up to 1000 attempts) from a distribution with an upper bound constraint
        @param input storing a distribution name
        @param upper_bound storing the upper bound value, lower_bound is 0 (implicitly)
        @param non_negative TRUE to only return non-negative number
 */
double random_number_from_distribution_with_upperbound(string distribution_name, double upper_bound, bool non_negative);

/**
       check whether a string is a number
        @param s storing a string
 */
bool is_number(const string& s);

/**
        randomly generate the (nucleotide) frequencies from empirical (built-in) distributions
        @param freqs storing the output base frequencies
        @param list_distribution_names specifying a list of distributions corresponding for each state
        @param num_states the number of states
 */
void random_frequencies_from_distributions(double *freqs, int num_states = 4, string list_distribution_names = "Generalized_logistic,Exponential_normal,Power_log_normal,Exponential_Weibull");

/**
    change unusual character in names into underscore (_)
    @param[in/out] name string name
    @return true if renamed, false otherwise
 */
bool renameString(string &name);

/**
        read the file containing branch/split scaling factor and taxa weights
        @param params program parameters
        @param ntaxa total number of taxa
        @param scale (OUT) scaling factor
        @param tax_name (OUT) vector of taxa names
        @param tax_weight (OUT) vector of corresponding taxa weights
 */
void readWeightFile(Params &params, int ntaxa, double &scale, StrVector &tax_name, DoubleVector &tax_weight);

/**
        read the initial taxa set from the file
        @param params program parameters
        @param ntaxa number of taxa
        @param tax_name (OUT) vector of taxa names
 */
void readInitTaxaFile(Params &params, int ntaxa, StrVector &tax_name);

/**
        read the initial area set from the file
        @param params program parameters
        @param nareas number of areas
        @param area_name (OUT) vector of area names
 */
void readInitAreaFile(Params &params, int nareas, StrVector &area_name);


/**
        read a list of taxa set from a file, not in nexus format but as follows:
        n1
        tax-name-1
        ...
        tax-name-n1

        n2
        tax-name-1
        ...
        tax-name-n2
        ....

        @param filename file name
        @param sets (OUT) the returned sets of taxa
 */
void readTaxaSets(char *filename, MSetsBlock *sets);

/**
        read areas shared boundary file, in form of a standard distance matrix
        @param file_name file name
        @param areas the read sets block
        @param areas_shared_boundary (OUT) shared boundary length between areas.
                Diagonal elements represent the boundary length of single areas
 */
void readAreasBoundary(char *file_name, MSetsBlock *areas, double *areas_shared_boundary);

/**
        parse program argument into params
        @param argc number of arguments
        @param argv list of arguments
        @param params (OUT) program parameters
 */
void parseArg(int argc, char *argv[], Params &params);

/**
        detect the format of input file
        @param input_file file name
        @return
                IN_NEWICK if file in newick format,
                IN_NEXUS if in nexus format,
                IN_FASTA if in fasta format,
                IN_PHYLIP if in phylip format,
		IN_COUNTSFILE if in counts format (PoMo),
                IN_OTHER if file format unknown.
 */
InputType detectInputFile(const char *input_file);

/**
        if file exists, ask user to overwrite it or not
        @param filename file name
        @return TRUE if agree to overwrite an existing file, or simply file does not exist
 */
bool overwriteFile(char *filename);

/**
        print usage information
        @param argv program arguments list
 */
void usage(char* argv[]);

/**
        print usage information of AliSim
 */
void usage_alisim();

/**
 *   Print a string into a file
 */
void printString2File(string myString, string filename);

/**
 * print usage for iq-tree
 * @param program arguments list
 * @param full_command TRUE to print all available commands, FALSE to print normal usage dialog
 */
void usage_iqtree(char* argv[], bool full_command);

/**
        parse area name string, where names are separated by commas
        @param area_names a string of name
        @param areas (OUT) a set of name string
 */
void parseAreaName(char *area_names, set<string> &areas);

/**
 * generate 2 different random integer numbers smaller than a specific integer threshold
 * @param size integer threshold
 * @param &first first random integer number
 * @param @second second random integer number
 */
void get2RandNumb(const int size, int &first, int &second);

/*
inline double getCPUTime(clock_t startTime) {
        return double(clock() - startTime) / CLOCKS_PER_SEC;
}*/


/**
 *  Fills the range [first, last) with sequentially increasing values,
 *  starting with value and repetitively evaluating ++value.
 *  Introduced in C++11 --> this is a reimplementation
 */
template<class ForwardIterator, class T>
void iota( ForwardIterator first, ForwardIterator last, T value );

/**
        compute p-value for a chi-square value
        @param chi_square chi-square value
        @param df degree of freedom
        @return p-value
 */
double computePValueChiSquare(double x, int df);

/*--------------------------------------------------------------*/
/* random number generator */
/*--------------------------------------------------------------*/

extern int *randstream;

/**
 * initialize the random number generator
 * @param seed seed for generator
 * @param write_info true to write information, false otherwise (default)
 */
int init_random(int seed, bool write_info = false, int** rstream = NULL);

/**
 * finalize random number generator (e.g. free memory
 */
int finish_random(int *rstream = NULL);

/**
 * returns a random integer in the range [0; n - 1]
 * @param n upper-bound of random number
 */
int random_int(int n, int *rstream = NULL);

/**
 *  return a random integer in the range [a,b]
 */
//int randint(int a, int b);

/**
 * returns a random integer in the range [0; RAND_MAX - 1]
 * = random_int(RAND_MAX)
 */
//int random_int(int *rstream = NULL);

/**
 * returns a random floating-point nuber in the range [0; 1)
 */
double random_double(int *rstream = NULL);

/**
 * returns a random double based on an exponential distribution
 * @param mean the mean of exponential distribution
 */
double random_double_exponential_distribution(double mean, int *rstream = NULL);

/**
 * geometric random number generation
 * Modified from W. Fletcher and Z. Yang, “INDELible: A flexible simulator of biological sequence evolution,” Mol. Biol. Evol., vol. 26, no. 8, pp. 1879–1888, 2009.
 * @param p
 */
int random_int_geometric(double p);

/**
 * negative binomial distribution
 * Modified from W. Fletcher and Z. Yang, “INDELible: A flexible simulator of biological sequence evolution,” Mol. Biol. Evol., vol. 26, no. 8, pp. 1879–1888, 2009.
 * @param r, q
 */
int random_int_nebin(int r, double q);

/**
 * Zipfian distribution
 * algorithm from DAWG (Cartwright, 2005).
 * Draw from Zipf distribution, with parameter a > 1.0
 * Devroye Luc (1986) Non-uniform random variate generation.
 * Springer-Verlag: Berlin. p551
 * @param a, m
 */
int random_int_zipf(double a, int m = -1);

/**
 * Lavalette distribution
 * Modified from W. Fletcher and Z. Yang, “INDELible: A flexible simulator of biological sequence evolution,” Mol. Biol. Evol., vol. 26, no. 8, pp. 1879–1888, 2009.
 * @param a, m
 */
int random_int_lav(double a, int m);

/**
 * Parse indel-size distribution
 */
IndelDistribution parseIndelDis(string input, string event_name);

template <class T>
void my_random_shuffle (T first, T last, int *rstream = NULL)
{
	int n = last - first;
	for (int i=n-1; i>0; --i) {
		swap (first[i],first[random_int(i+1, rstream)]);
	}
}

/**
 random resampling according to bootstrap or jackknife
 @param n sample size
 @param[in/out] sample array of size n with frequency of resampling
 @param rstream random number generator stream
*/
void random_resampling(int n, IntVector &sample, int *rstream = NULL);

#define RESAMPLE_NAME ((Params::getInstance().jackknife_prop == 0.0) ? "bootstrap" : "jackknife")
#define RESAMPLE_NAME_I ((Params::getInstance().jackknife_prop == 0.0) ? "Bootstrap" : "Jackknife")
#define RESAMPLE_NAME_UPPER ((Params::getInstance().jackknife_prop == 0.0) ? "BOOTSTRAP" : "JACKKNIFE")

/**
 * generic function for sorting by index
 */
template <class T>
void quicksort_index(T* arr, int* index, int left, int right) {
    int i = left, j = right, tmp2;
    T tmp, pivot = arr[(left + right) / 2];

    /* partition */
    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (pivot < arr[j])
            j--;
        if (i <= j) {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            tmp2 = index[i];
            index[i] = index[j];
            index[j] = tmp2;
            i++;
            j--;
        }
    };

    /* recursion */
    if (left < j)
        quicksort_index(arr, index, left, j);
    if (i < right)
        quicksort_index(arr, index, i, right);
}

/**
 * generic function for sorting by index preseving entries in [first,last)
 * @param first first element
 * @param last last element
 * @param index (OUT) ordered index of elements from smallest to largest
 */
template <class T>
void sort_index(T* first, T* last, int *index) {
    T* x;
    int i;
    T* arr = new T[last - first];
    for (x = first, i = 0; x != last; x++, i++) {
        index[i] = i;
        arr[i] = *x;
    }
    ASSERT(last - first == i);
    quicksort_index(arr, index, 0, (last - first) - 1);
    delete [] arr;
}

/**
 * print the header of summary file
 */
void summarizeHeader(ostream &out, Params &params, bool budget_constraint, InputType analysis_type);

/**
 * print footer of summary file
 */
void summarizeFooter(ostream &out, Params &params);


/**
    remove white space at the beginning and end of the string
    @param str (IN/OUT) string to be trimmed
*/
void trimString(string &str);

/**
    get number of processor cores
*/
int countPhysicalCPUCores();

void print_stacktrace(ostream &out, unsigned int max_frames = 63);

/**
    quicksort template
*/
template<class T1, class T2>
void quicksort(T1* arr, int left, int right, T2* arr2 = NULL) {
    if (left > right) return;
    ASSERT(left <= right);
      int i = left, j = right;
      T1 pivot = arr[(left + right) / 2];

      /* partition */
      while (i <= j) {
            while (arr[i] < pivot)
                  i++;
            while (arr[j] > pivot)
                  j--;
            if (i <= j) {
                  T1 tmp = arr[i];
                  arr[i] = arr[j];
                  arr[j] = tmp;
                  if (arr2) {
                      T2 tmp2 = arr2[i];
                      arr2[i] = arr2[j];
                      arr2[j] = tmp2;
                  }
                  i++;
                  j--;
            }
      };

      /* recursion */
      if (left < j)
            quicksort(arr, left, j, arr2);
      if (i < right)
            quicksort(arr, i, right, arr2);
}

/* An optimized version of C̩dric Lauradoux's 64-bit merging3 algorithm
   implemented by Kim Walisch, see:
   http://code.google.com/p/primesieve/source/browse/trunk/src/soe/bithacks.h
   Modified ever so slightly to maintain the same API. Note that
   it assumes the buffer is a multiple of 64 bits in length.
*/
inline uint32_t popcount_lauradoux(unsigned *buf, int n) {
  const uint64_t* data = (uint64_t*) buf;
  uint32_t size = n/(sizeof(uint64_t)/sizeof(int));
  const uint64_t m1  = (0x5555555555555555ULL);
  const uint64_t m2  = (0x3333333333333333ULL);
  const uint64_t m4  = (0x0F0F0F0F0F0F0F0FULL);
  const uint64_t m8  = (0x00FF00FF00FF00FFULL);
  const uint64_t m16 = (0x0000FFFF0000FFFFULL);
  const uint64_t h01 = (0x0101010101010101ULL);

  uint32_t bitCount = 0;
  uint32_t i, j;
  uint64_t count1, count2, half1, half2, acc;
  uint64_t x;
  uint32_t limit30 = size - size % 30;

  // 64-bit tree merging (merging3)
  for (i = 0; i < limit30; i += 30, data += 30) {
    acc = 0;
    for (j = 0; j < 30; j += 3) {
      count1  =  data[j];
      count2  =  data[j+1];
      half1   =  data[j+2];
      half2   =  data[j+2];
      half1  &=  m1;
      half2   = (half2  >> 1) & m1;
      count1 -= (count1 >> 1) & m1;
      count2 -= (count2 >> 1) & m1;
      count1 +=  half1;
      count2 +=  half2;
      count1  = (count1 & m2) + ((count1 >> 2) & m2);
      count1 += (count2 & m2) + ((count2 >> 2) & m2);
      acc    += (count1 & m4) + ((count1 >> 4) & m4);
    }
    acc = (acc & m8) + ((acc >>  8)  & m8);
    acc = (acc       +  (acc >> 16)) & m16;
    acc =  acc       +  (acc >> 32);
    bitCount += (uint32_t)acc;
  }

  // count the bits of the remaining bytes (MAX 29*8) using
  // "Counting bits set, in parallel" from the "Bit Twiddling Hacks",
  // the code uses wikipedia's 64-bit popcount_3() implementation:
  // http://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
  for (i = 0; i < size - limit30; i++) {
    x = data[i];
    x =  x       - ((x >> 1)  & m1);
    x = (x & m2) + ((x >> 2)  & m2);
    x = (x       +  (x >> 4)) & m4;
    bitCount += (uint32_t)((x * h01) >> 56);
  }
  return bitCount;
}

/**
 * combination of memcmp and memcpy.
 * @param destination destination memory to copy to
 * @param source code memory to copy from
 * @param num number of bytes to copy
 * @return TRUE of memory are different, FALSE if identical
 */
bool memcmpcpy(void * destination, const void * source, size_t num);

/**
 *  Generating a unique integer from a pair of 2 integer
 *  This method is called cantor pairing function (see wikepedia).
 *  @param int1 the first integer
 *  @param int2 the second integer
 *  @return the encoding of the 2 integer
 */
int pairInteger(int int1, int int2);

/*
 * Given a model name, look in it for "+F..." and 
 * determine the StateFreqType. Returns FREQ_UNKNOWN if
 * unable to find a good +F... specifier
 */
StateFreqType parseStateFreqFromPlusF(string model_name);

/*
 * Given a string of 4 digits, return a StateFreqType according to
 * equality constraints expressed by those digits.
 * E.g. "1233" constrains pi_G=pi_T (ACGT order, 3rd and 4th equal)
 * which results in FREQ_DNA_2311. "5288" would give the same result.
 */
StateFreqType parseStateFreqDigits(string digits);

/*
 * All params in range [0,1] 
 * returns true if base frequencies have changed as a result of this call
 */
bool freqsFromParams(double *freq_vec, double *params, StateFreqType freq_type);

/*
 * For given freq_type, derives frequency parameters from freq_vec
 * All parameters are in range [0,1] (assuming freq_vec is valid)
 */
void paramsFromFreqs(double *params, double *freq_vec, StateFreqType freq_type);

/* 
 * Given a DNA freq_type and a base frequency vector, alter the
 * base freq vector to conform with the constraints of freq_type
 */
void forceFreqsConform(double *base_freq, StateFreqType freq_type);

/*
 * For given freq_type, how many parameters are needed to
 * determine frequenc vector?
 * BQM 2017-04-28: works for DNA and other data types
 */
 int nFreqParams(StateFreqType freq_type);

/*
 * For freq_type, and given every base must have frequency >= min_freq, set upper
 * and lower bounds for parameters.
 */
 void setBoundsForFreqType(double *lower_bound, 
                           double *upper_bound, 
                           bool *bound_check, 
                           double min_freq, 
                           StateFreqType freq_type);

template <typename T>
string NumberToString ( T Number )
{
    ostringstream ss;
    ss << Number;
    return ss.str();
}

template <typename T>
T StringToNumber ( const string &Text )
{
    istringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

// Calculate logarithm of binomial coefficient N choose i.
double binomial_coefficient_log(unsigned int N, unsigned int i);

// Calculate probability of having k out of N successes when the probability of
// a success is p under the binomial distribution.
double binomial_dist(unsigned int k, unsigned int N, double p);

// Calculate probability of having k out of n successes when there are K out of
// N successes in the pool under the hypergeometric distribution.
double hypergeometric_dist(unsigned int k, unsigned int n, unsigned int K, unsigned int N);

// Calculate the Frobenius norm of an N x N matrix M (flattened, rows
// concatenated) and linearly scaled by SCALE.
double frob_norm (double m[], int n, double scale=1.0);

/**
    concatenate the output file name with corresponding extension (for AliSim)
*/
string getOutputNameWithExt(const InputType& format, const string& output_filepath);


#endif