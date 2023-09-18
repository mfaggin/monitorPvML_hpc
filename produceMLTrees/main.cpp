#include "headerFilterTree.h"

int main(int argc, char** argv){

    /*if(argc==1 || (argc==2 && (std::string(argv[1])=="-h" || std::string(argv[1])=="--help"))){
        std::cout << "===> Specify the following parameters: " << std::endl;
        std::cout << " [print debug std::cout (as integer)]" << std::endl;
        std::cout << " [activate Filter_MC_TTree (as integer)]" << std::endl;
        std::cout << " [activate Filter_background_TTree (as integer)]" << std::endl;
        std::cout << " [activate Filter_application_TTree (as integer)]" << std::endl;
        std::cout << " [minpt] [maxpt]" << std::endl;
        std::cout << std::endl;
        return 0;
    }
    if(argc<7)  {
        std::cout << "Not enough arguments!" << std::endl;
        return 0;
    }
    
    bool print_debug_cout = atoi(argv[1]);

    // filter the signal TTree
    if(atoi(argv[2]))   Filter_MC_TTree( print_debug_cout, atoi(argv[5]), atoi(argv[6]) );

    // filter the bacgkround TTree
    if(atoi(argv[3]))   Filter_background_TTree( print_debug_cout, atoi(argv[5]), atoi(argv[6]) );

    // filter the application TTree
    if(atoi(argv[4]))   Filter_application_TTree( print_debug_cout );*/

    // pp 13.6 TeV, MC anchored to LHC22f_pass4
    //std::cout << "##### Tree preparation on MC LHC23d1e (single PV)" << std::endl;
    //filterTree("inputs_LHC23d1e_single.txt", "LHC23d1e_single", true);
    //
    //std::cout << "##### Tree preparation on MC LHC23d1e (duplicated PV)" << std::endl;
    //filterTree("inputs_LHC23d1e_duplicate.txt", "LHC23d1e_duplicate", true);
    //
    //std::cout << "##### Tree preparation on MC LHC23d1f" << std::endl;
    //filterTree("inputs_LHC23d1f_from2023Jul13.txt", "LHC23d1f", true);

    // Pb-Pb 5.36 TeV, MC anchored to LHC22s_pass5
    //std::cout << "##### Tree preparation on MC LHC22l1b2 (single PV)" << std::endl;
    //filterTree("inputs_LHC22l1b2_single.txt", "LHC22l1b2_single", true);
    //
    //std::cout << "##### Tree preparation on MC LHC22l1b2 (duplicated PV)" << std::endl;
    //filterTree("inputs_LHC22l1b2_duplicated.txt", "LHC22l1b2_duplicated", true);
    //
    
    std::cout << "##### Tree preparation on MC LHC22l1b2 (single PV) for TRAINING only run 418" << std::endl;
    filterTree("inputs_LHC22l1b2_single_418.txt", "LHC22l1b2_single_418", true);
    //
    std::cout << "##### Tree preparation on MC LHC22l1b2 (duplicated PV) for TRAINING only run 418" << std::endl;
    filterTree("inputs_LHC22l1b2_duplicated_418.txt", "LHC22l1b2_duplicated_418", true);
    //
    std::cout << "##### Tree preparation on MC LHC22l1b2 (all PV) for APPLICATION" << std::endl;
    filterTree("inputs_LHC22l1b2_all_397_399_403_414.txt", "LHC22l1b2_all_397_399_403_414", true);
    

    // Pb-Pb 5.36 TeV, MC unanchored
    //
    //std::cout << "##### Tree preparation on MC LHC22i1 (single PV)" << std::endl;
    //filterTree("inputs_LHC22i1_single.txt", "LHC22i1_single", true);
    //
    //std::cout << "##### Tree preparation on MC LHC22i1 (duplicated PV)" << std::endl;
    //filterTree("inputs_LHC22i1_duplicated.txt", "LHC22i1_duplicated", true);
    //
    //std::cout << "##### Tree preparation on MC LHC22i1 (all PV)" << std::endl;
    //filterTree("inputs_LHC22i1_all.txt", "LHC22i1_all", true);

    /// trial with 1 file (debug)
    //std::cout << "##### Tree preparation on MC LHC23d1e" << std::endl;
    //filterTree("inputs_LHC23d1e_1file.txt", "LHC23d1e_1file", true, true, true);

    return 0;
}