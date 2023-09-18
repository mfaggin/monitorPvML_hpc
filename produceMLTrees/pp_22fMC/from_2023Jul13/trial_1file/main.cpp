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


/*
    std::cout << "##### Tree preparation on MC LHC23d1e" << std::endl;
    filterTree("inputs_LHC23d1e.txt", "LHC23d1e", true);

    std::cout << "##### Tree preparation on MC LHC23d1f" << std::endl;
    filterTree("inputs_LHC23d1f.txt", "LHC23d1f", true);
*/
    std::cout << "##### Tree preparation on MC LHC23d1e" << std::endl;
    filterTree("inputs_LHC23d1e_1file.txt", "LHC23d1e_1file", true);

    return 0;
}