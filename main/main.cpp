/***************************************************************************
 *   Copyright (C) 2006 by BUI Quang Minh, Steffen Klaere, Arndt von Haeseler   *
 *   minh.bui@univie.ac.at   *
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iqtree_config.h>

#if defined WIN32 || defined _WIN32 || defined __WIN32__ || defined WIN64
//#include <winsock2.h>
//#include <windows.h>
//extern __declspec(dllexport) int gethostname(char *name, int namelen);
#else
#include <sys/resource.h>
#endif

#include <stdio.h>
#include "tree/phylotree.h"
#include <signal.h>
#include <cstdio>
#include <streambuf>
#include <iostream>
#include <cstdlib>
#include <errno.h>
#include "pda/greedy.h"
#include "pda/pruning.h"
#include "pda/splitgraph.h"
#include "pda/circularnetwork.h"
#include "tree/mtreeset.h"
#include "tree/mexttree.h"
#include "ncl/ncl.h"
#include "nclextra/msetsblock.h"
#include "nclextra/myreader.h"
#include "phyloanalysis.h"
#include "alisim.h"
#include "tree/matree.h"
#include "obsolete/parsmultistate.h"
#include "alignment/maalignment.h" //added by MA
#include "tree/ncbitree.h"
#include "pda/ecopd.h"
#include "tree/upperbounds.h"
#include "terraceanalysis.h"
#include "pda/ecopdmtreeset.h"
#include "pda/gurobiwrapper.h"
#include "utils/timeutil.h"
#include "utils/operatingsystem.h" //for getOSName()
#include <stdlib.h>
#include "vectorclass/instrset.h"

#include "utils/MPIHelper.h"

#ifdef _OPENMP
    #include <omp.h>
#endif

#include "libiqtree2_fun.h"
#include "suppFunc.h"

using namespace std;

/********************************************************
    main function
********************************************************/
/*
int main(){
    IQTree tree;
    char * str = "(1, (2, 345));";
    string k;
    tree.pllConvertTaxaID2IQTreeForm(str, k);
    cout << str << endl;
    cout << k << endl;
    cout << "WHAT" << endl;
    return 0;
}
*/


int main(int argc, char *argv[]) {

    
    // ====================================================
    // for testing the available functions for PiQTree
    // ====================================================
    if (argc > 1 && strcmp(argv[1],"--piqtree") == 0) {
        
        if (argc < 3) {
            // show the available options for testing piqtree
            std::cout << "available options:" << endl;
            std::cout << "calculate RF distance: --piqtree 1 [tree1 newick string] [tree2 newick string]" << endl;
            std::cout << "generate random tree: --piqtree 2 [num taxa] [seed] [mode (e.g. 'YULE_HARDING')] [out filename]" << endl;
            std::cout << "phylogenetic analysis: --piqtree 3 [alignment file]" << endl;
            return 0;
        }

        if (atoi(argv[2]) == 1) {
            // calculate the RF distance
            if (argc < 5) {
                std::cout << "Syntax: " << argv[0] << " --piqtree 1 [tree1 newick string] [tree2 newick string]" << endl;
                return 0;
            }
            std::string tree1 = argv[3];
            std::string tree2 = argv[4];
            std::cout << "RF dist: " << calculate_RF_distance(tree1, tree2) << std::endl;
            return 0;
        }
        
        if (atoi(argv[2]) == 2) {
            // generate a random tree
            if (argc < 7) {
                std::cout << "Syntax: " << argv[0] << " --piqtree 2 [num taxa] [seed] [mode (e.g. 'YULE_HARDING')] [out filename]" << endl;
                return 0;
            }
            int numtaxa = atoi(argv[3]);
            int seed = atoi(argv[4]);
            string gen_mode = argv[5];
            string outfilename = argv[6];
            generate_random_tree_file(numtaxa, seed, gen_mode, outfilename);
            return 0;
        }

        if (atoi(argv[2]) == 3) {
            // perform phylogenetic analysis
            if (argc < 4) {
                std::cout << "Syntax: " << argv[0] << " --piqtree 3 [alignment file] [# cpu threads]" << endl;
                return 0;
            }
            string align_file = argv[3];
            if (argc < 5)
                phylogenetic_analysis(align_file);
            else {
                int ncpus = atoi(argv[4]);
                phylogenetic_analysis(align_file, ncpus);
            }
            return 0;
        }
        
        return 0;
    }

    
    /*
    Instruction set ID reported by vectorclass::instrset_detect
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
    int instruction_set;

    MPIHelper::getInstance().init(argc, argv);
    
    atexit(funcExit);

    /*************************/
    { /* local scope */
        int found = FALSE;              /* "click" found in cmd name? */
        int n, dummyint;
        char *tmpstr;
        int intargc;
        char **intargv;
        intargc = 0;
        intargv = NULL;

        for (n = strlen(argv[0]) - 5;
             (n >= 0) && !found && (argv[0][n] != '/')
             && (argv[0][n] != '\\'); n--) {

            tmpstr = &(argv[0][n]);
            dummyint = 0;
            (void) sscanf(tmpstr, "click%n", &dummyint);
            if (dummyint == 5) found = TRUE;
            else {
                dummyint = 0;
                (void) sscanf(tmpstr, "CLICK%n", &dummyint);
                if (dummyint == 5) found = TRUE;
                else {
                    dummyint = 0;
                    (void) sscanf(tmpstr, "Click%n", &dummyint);
                    if (dummyint == 5) found = TRUE;
                }
            }
        }
        if (found) _exit_wait_optn = TRUE;

        if (_exit_wait_optn) { // get commandline parameters from keyboard
            getintargv(&intargc, &intargv);
            fprintf(stdout, "\n\n");
            if (intargc > 1) { // if there were option entered, use them as argc/argv
                argc = intargc;
                argv = intargv;
            }
        }
    } /* local scope */
    /*************************/

    parseArg(argc, argv, Params::getInstance());

    // 2015-12-05
    Checkpoint *checkpoint = new Checkpoint;
    string filename = (string)Params::getInstance().out_prefix +".ckp.gz";
    checkpoint->setFileName(filename);
    
    bool append_log = false;
    
    if (!Params::getInstance().ignore_checkpoint && fileExists(filename)) {
        checkpoint->load();
        if (checkpoint->hasKey("finished")) {
            if (checkpoint->getBool("finished")) {
                if (Params::getInstance().force_unfinished) {
                    if (MPIHelper::getInstance().isMaster())
                        cout << "NOTE: Continue analysis although a previous run already finished" << endl;
                } else {
                    delete checkpoint;
                    if (MPIHelper::getInstance().isMaster())
                        outError("Checkpoint (" + filename + ") indicates that a previous run successfully finished\n" +
                            "Use `-redo` option if you really want to redo the analysis and overwrite all output files.\n" +
                            "Use `--redo-tree` option if you want to restore ModelFinder and only redo tree search.\n" +
                            "Use `--undo` option if you want to continue previous run when changing/adding options."
                        );
                    else
                        exit(EXIT_SUCCESS);
                    exit(EXIT_FAILURE);
                } 
            } else {
                append_log = true;
            }
        } else {
            if (MPIHelper::getInstance().isMaster())
                outWarning("Ignore invalid checkpoint file " + filename);
            checkpoint->clear();
        }
    }

    if (MPIHelper::getInstance().isWorker())
        checkpoint->setFileName("");

    _log_file = Params::getInstance().out_prefix;
    _log_file += ".log";
    startLogFile(append_log);
    time_t start_time;

    if (append_log) {
        cout << endl << "******************************************************"
             << endl << "CHECKPOINT: Resuming analysis from " << filename << endl << endl;
    }

    MPIHelper::getInstance().syncRandomSeed();
    
    signal(SIGABRT, &funcAbort);
    signal(SIGFPE, &funcAbort);
    signal(SIGILL, &funcAbort);
    signal(SIGSEGV, &funcAbort);
#if !defined WIN32 && !defined _WIN32 && !defined __WIN32__ && !defined WIN64
    signal(SIGBUS, &funcAbort);
#endif
    printCopyright(cout);

    /*
    double x=1e-100;
    double y=1e-101;
    if (x > y) cout << "ok!" << endl;
    else cout << "shit!" << endl;
    */
    //FILE *pfile = popen("hostname","r");
    char hostname[100];
#if defined WIN32 || defined _WIN32 || defined __WIN32__ || defined WIN64
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    gethostname(hostname, sizeof(hostname));
    WSACleanup();
#else
    gethostname(hostname, sizeof(hostname));
#endif
    //fgets(hostname, sizeof(hostname), pfile);
    //pclose(pfile);

    instruction_set = instrset_detect();
#if defined(BINARY32) || defined(__NOAVX__)
    instruction_set = min(instruction_set, (int)LK_SSE42);
#endif
    if (instruction_set < LK_SSE2) outError("Your CPU does not support SSE2!");
    bool has_fma3 = (instruction_set >= LK_AVX) && hasFMA3();

#ifdef __FMA__
    bool has_fma =  has_fma3;
    if (!has_fma) {
        outError("Your CPU does not support FMA instruction, quiting now...");
    }
#endif

    cout << "Host:    " << hostname << " (";
    switch (instruction_set) {
    case 0: cout << "x86, "; break;
    case 1: cout << "SSE, "; break;
    case 2: cout << "SSE2, "; break;
    case 3: cout << "SSE3, "; break;
    case 4: cout << "SSSE3, "; break;
    case 5: cout << "SSE4.1, "; break;
    case 6: cout << "SSE4.2, "; break;
    case 7: cout << "AVX, "; break;
    case 8: cout << "AVX2, "; break;
    default: cout << "AVX512, "; break;
    }
    if (has_fma3) cout << "FMA3, ";
//    if (has_fma4) cout << "FMA4, ";
//#if defined __APPLE__ || defined __MACH__
    cout << (int)(((getMemorySize()/1024.0)/1024)/1024) << " GB RAM)" << endl;
//#else
//    cout << (int)(((getMemorySize()/1000.0)/1000)/1000) << " GB RAM)" << endl;
//#endif

    cout << "Command:";
    int i;
    for (i = 0; i < argc; i++)
        cout << " " << argv[i];
    cout << endl;

    checkpoint->get("iqtree.seed", Params::getInstance().ran_seed);
    cout << "Seed:    " << Params::getInstance().ran_seed <<  " ";
    init_random(Params::getInstance().ran_seed + MPIHelper::getInstance().getProcessID(), true);

    time(&start_time);
    cout << "Time:    " << ctime(&start_time);

    // increase instruction set level with FMA
    if (has_fma3 && instruction_set < LK_AVX_FMA)
        instruction_set = LK_AVX_FMA;

    Params::getInstance().SSE = min(Params::getInstance().SSE, (LikelihoodKernel)instruction_set);

    cout << "Kernel:  ";

    if (Params::getInstance().lk_safe_scaling) {
        cout << "Safe ";
    }

    if (Params::getInstance().pll) {
#ifdef __AVX__
        cout << "PLL-AVX";
#else
        cout << "PLL-SSE3";
#endif
    } else {
        if (Params::getInstance().SSE >= LK_AVX512)
            cout << "AVX-512";
        else if (Params::getInstance().SSE >= LK_AVX_FMA) {
            cout << "AVX+FMA";
        } else if (Params::getInstance().SSE >= LK_AVX) {
            cout << "AVX";
        } else if (Params::getInstance().SSE >= LK_SSE2){
            cout << "SSE2";
        } else
            cout << "x86";
    }

#ifdef _OPENMP
    if (Params::getInstance().num_threads >= 1) {
        omp_set_num_threads(Params::getInstance().num_threads);
        Params::getInstance().num_threads = omp_get_max_threads();
    }
//    int max_threads = omp_get_max_threads();
    int max_procs = countPhysicalCPUCores();
    cout << " - ";
    if (Params::getInstance().num_threads > 0)
        cout << Params::getInstance().num_threads  << " threads";
    else
        cout << "auto-detect threads";
    cout << " (" << max_procs << " CPU cores detected)";
    if (Params::getInstance().num_threads  > max_procs) {
        cout << endl;
        outError("You have specified more threads than CPU cores available");
    }
    omp_set_nested(false); // don't allow nested OpenMP parallelism
#else
    if (Params::getInstance().num_threads != 1) {
        cout << endl << endl;
        outError("Number of threads must be 1 for sequential version.");
    }
#endif

#ifdef _IQTREE_MPI
    cout << endl << "MPI:     " << MPIHelper::getInstance().getNumProcesses() << " processes";
#endif
    
    int num_procs = countPhysicalCPUCores();
#ifdef _OPENMP
    if (num_procs > 1 && Params::getInstance().num_threads == 1 && !Params::getInstance().alisim_active) {
        cout << endl << endl << "HINT: Use -nt option to specify number of threads because your CPU has " << num_procs << " cores!";
        cout << endl << "HINT: -nt AUTO will automatically determine the best number of threads to use.";
    }
#else
    if (num_procs > 1)
        cout << endl << endl << "NOTE: Consider using the multicore version because your CPU has " << num_procs << " cores!";
#endif

    //cout << "sizeof(int)=" << sizeof(int) << endl;
    cout << endl << endl;
    
    // show msgs which are delayed to show
    cout << Params::getInstance().delay_msgs;

    cout.precision(3);
    cout.setf(ios::fixed);
    
    // checkpoint general run information
    checkpoint->startStruct("iqtree");
    string command;
    
    if (CKP_RESTORE_STRING(command)) {
        // compare command between saved and current commands
        stringstream ss(command);
        string str;
        bool mismatch = false;
        for (i = 1; i < argc; i++) {
            if (!(ss >> str)) {
                outWarning("Number of command-line arguments differs from checkpoint");
                mismatch = true;
                break;
            }
            if (str != argv[i]) {
                outWarning((string)"Command-line argument `" + argv[i] + "` differs from checkpoint `" + str + "`");
                mismatch = true;
            }
        }
        if (mismatch) {
            outWarning("Command-line differs from checkpoint!");
        }
        command = "";
    }
    
    for (i = 1; i < argc; i++)
        command += string(" ") + argv[i];
    CKP_SAVE(command);
    int seed = Params::getInstance().ran_seed;
    CKP_SAVE(seed);
    CKP_SAVE(start_time);

    // check for incompatible version
    string version;
    stringstream sversion;
    sversion << iqtree_VERSION_MAJOR << "." << iqtree_VERSION_MINOR << iqtree_VERSION_PATCH;
    version = sversion.str();
    CKP_SAVE(version);
    checkpoint->endStruct();
    
    // load distributions from built-in file and user-specified file
    // load distributions from built-in file
    read_distributions();
    // load distributions from user-specified file
    if (Params::getInstance().alisim_distribution_definitions)
    {
        cout<<"Reading user-specified distributions/lists of random numbers."<<endl;
        read_distributions(Params::getInstance().alisim_distribution_definitions);
    }

    if (MPIHelper::getInstance().getNumProcesses() > 1) {
        if (Params::getInstance().alisim_active) {
            runAliSim(Params::getInstance(), checkpoint);
        } else if (Params::getInstance().aln_file || Params::getInstance().partition_file) {
            runPhyloAnalysis(Params::getInstance(), checkpoint);
        } else {
            outError("Please use one MPI process! The feature you wanted does not need parallelization.");
        }
    } else
    // call the main function
    if (Params::getInstance().alisim_active) {
        runAliSim(Params::getInstance(), checkpoint);
    } else if (Params::getInstance().tree_gen != NONE && Params::getInstance().start_tree!=STT_RANDOM_TREE) {
        generateRandomTree(Params::getInstance());
    } else if (Params::getInstance().do_pars_multistate) {
        doParsMultiState(Params::getInstance());
    } else if (Params::getInstance().rf_dist_mode != 0) {
        computeRFDist(Params::getInstance());
    } else if (Params::getInstance().test_input != TEST_NONE) {
        Params::getInstance().intype = detectInputFile(Params::getInstance().user_file);
        testInputFile(Params::getInstance());
    } else if (Params::getInstance().run_mode == PRINT_TAXA) {
        printTaxa(Params::getInstance());
    } else if (Params::getInstance().run_mode == PRINT_AREA) {
        printAreaList(Params::getInstance());
    } else if (Params::getInstance().run_mode == SCALE_BRANCH_LEN || Params::getInstance().run_mode == SCALE_NODE_NAME) {
        scaleBranchLength(Params::getInstance());
    } else if (Params::getInstance().run_mode == PD_DISTRIBUTION) {
        calcDistribution(Params::getInstance());
    } else if (Params::getInstance().run_mode == STATS){ /**MINH ANH: for some statistics on the input tree*/
        branchStats(Params::getInstance()); // MA
    } else if (Params::getInstance().branch_cluster > 0) {
        calcTreeCluster(Params::getInstance());
    } else if (Params::getInstance().ncbi_taxid) {
        processNCBITree(Params::getInstance());
    } else if (Params::getInstance().user_file && Params::getInstance().eco_dag_file) { /**ECOpd analysis*/
        processECOpd(Params::getInstance());
    } else if (Params::getInstance().gen_all_NNI){
        PhyloTree *tree = new PhyloTree();
        tree->readTree(Params::getInstance().user_file, Params::getInstance().is_rooted);
        tree->gen_all_nni_trees();
    } else if (Params::getInstance().terrace_analysis) { /**Olga: Terrace analysis*/
        runterraceanalysis(Params::getInstance());
    } else if ((Params::getInstance().aln_file || Params::getInstance().partition_file) &&
               Params::getInstance().consensus_type != CT_ASSIGN_SUPPORT_EXTENDED)
    {
        if ((Params::getInstance().siteLL_file || Params::getInstance().second_align) && !Params::getInstance().gbo_replicates)
        {
            if (Params::getInstance().siteLL_file)
                guidedBootstrap(Params::getInstance());
            if (Params::getInstance().second_align)
                computeMulProb(Params::getInstance());
        } else {
            runPhyloAnalysis(Params::getInstance(), checkpoint);
        }
//    } else if (Params::getInstance().ngs_file || Params::getInstance().ngs_mapped_reads) {
//        runNGSAnalysis(Params::getInstance());
//    } else if (Params::getInstance().pdtaxa_file && Params::getInstance().gene_scale_factor >=0.0 && Params::getInstance().gene_pvalue_file) {
//        runGSSAnalysis(Params::getInstance());
    } else if (Params::getInstance().consensus_type != CT_NONE) {
        MExtTree tree;
        switch (Params::getInstance().consensus_type) {
            case CT_CONSENSUS_TREE:
                computeConsensusTree(Params::getInstance().user_file, Params::getInstance().tree_burnin, Params::getInstance().tree_max_count, Params::getInstance().split_threshold,
                    Params::getInstance().split_weight_threshold, Params::getInstance().out_file, Params::getInstance().out_prefix, Params::getInstance().tree_weight_file, &Params::getInstance());
                break;
            case CT_CONSENSUS_NETWORK:
                computeConsensusNetwork(Params::getInstance().user_file, Params::getInstance().tree_burnin, Params::getInstance().tree_max_count, Params::getInstance().split_threshold,
                    Params::getInstance().split_weight_summary, Params::getInstance().split_weight_threshold, Params::getInstance().out_file, Params::getInstance().out_prefix, Params::getInstance().tree_weight_file);
                break;
            case CT_ASSIGN_SUPPORT:
                assignBootstrapSupport(Params::getInstance().user_file, Params::getInstance().tree_burnin, Params::getInstance().tree_max_count, 
                    Params::getInstance().second_tree, Params::getInstance().is_rooted, Params::getInstance().out_file,
                    Params::getInstance().out_prefix, tree, Params::getInstance().tree_weight_file, &Params::getInstance());
                break;
            case CT_ASSIGN_SUPPORT_EXTENDED:
                assignBranchSupportNew(Params::getInstance());
                break;
            case CT_NONE: break;
            /**MINH ANH: for some comparison*/
            case COMPARE: compare(Params::getInstance()); break; //MA
            case CT_ROOTSTRAP:
                runRootstrap(Params::getInstance()); break;
        }
    } else if (Params::getInstance().split_threshold_str) {
        // for Ricardo: keep those splits from input tree above given support threshold
        collapseLowBranchSupport(Params::getInstance().user_file, Params::getInstance().split_threshold_str);
    } else {
        Params::getInstance().intype = detectInputFile(Params::getInstance().user_file);
        if (Params::getInstance().intype == IN_NEWICK && Params::getInstance().pdtaxa_file && Params::getInstance().tree_gen == NONE) {
            if (Params::getInstance().budget_file) {
                //if (Params::getInstance().budget < 0) Params::getInstance().run_mode = PD_USER_SET;
            } else {
                if (Params::getInstance().sub_size < 1 && Params::getInstance().pd_proportion == 0.0)
                    Params::getInstance().run_mode = PD_USER_SET;
            }
            // input is a tree, check if it is a reserve selection -> convert to splits
            if (Params::getInstance().run_mode != PD_USER_SET) Params::getInstance().multi_tree = true;
        }


        if (Params::getInstance().intype == IN_NEWICK && !Params::getInstance().find_all && Params::getInstance().budget_file == NULL &&
            Params::getInstance().find_pd_min == false && Params::getInstance().calc_pdgain == false &&
            Params::getInstance().run_mode != LINEAR_PROGRAMMING && Params::getInstance().multi_tree == false)
            runPDTree(Params::getInstance());
        else if (Params::getInstance().intype == IN_NEXUS || Params::getInstance().intype == IN_NEWICK) {
            if (Params::getInstance().run_mode == LINEAR_PROGRAMMING && Params::getInstance().find_pd_min)
                outError("Current linear programming does not support finding minimal PD sets!");
            if (Params::getInstance().find_all && Params::getInstance().run_mode == LINEAR_PROGRAMMING)
                Params::getInstance().binary_programming = true;
            runPDSplit(Params::getInstance());
        } else {
            outError("Unknown file input format");
        }
    }

    time(&start_time);
    cout << "Date and Time: " << ctime(&start_time);
    try{
    delete checkpoint;
    }catch(int err_num){}

    finish_random();
    
    return EXIT_SUCCESS;
}
