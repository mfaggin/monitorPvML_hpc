#include "PostProcessBDT.h"

int main(int argc, char **argv) {

    //______________________________
    //___ Settings to be changed ___
    ML::PostProcessBDT postprocess;
    postprocess.setInputFile("../appliedBDT_LHC22l1b2_all_397_399_403_414.root");
    postprocess.setIsMC(true);
    postprocess.setCaseApplication("LHC22l1b2_runs_397_399_403_414");
    postprocess.setBdtCut(0);
    postprocess.setClasses({"single_PV", "duplicated_PV"}, {"fIsDuplicate < 1", "fIsDuplicate > 0"});
    postprocess.setEnablePlotting();
    postprocess.setProcessAllCases(); // enable the processing of all cases
    //postprocess.setProcessCase(ML::kBdtScore); // enable the processing of a single case
    //______________________________
    //______________________________
    
    postprocess.getTTreeFromFile();
    postprocess.print_data_members();

    postprocess.process_labelled(argc, argv);

    

    return 0;
}