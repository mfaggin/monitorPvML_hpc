#ifndef PostProcessBDT_h
#define PostProcessBDT_h

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TStyle.h"

namespace ML {
    
    enum eCases {
        kBdtScore=0, kBdtScoreNormalized, kIsEvSel,
        kPosX, kPosY, kPosZ,
        kCovXX, kCovXY, kCovXZ, kCovYY, kCovYZ, kCovZZ,
        kNumContrib, kNumTracksAll, kNumtracksFiltered, kPvChi2,
        kFt0PosZ, kFt0Asignal, kFt0Csignal, kFt0Msignal, kV0Asignal,
        kCollTimeRes, kNRecoPvPerMcColl, kIsPvHighestContribForMcColl, kIsDuplicate,
        /* TODO: add all 2D cases */
        kNCases
    };

    class PostProcessBDT {

        public:
            PostProcessBDT();   // dummy constructor
            PostProcessBDT(std::string str_input, std::string str_case);
            ~PostProcessBDT();

            
            /// setters
            void setIsMC(bool flag) {isMC = flag;}
            void setInputFile(std::string flag) {str_file_input = flag;}
            void setCaseApplication(std::string flag) {str_case_application = flag;}
            void setBdtCut (float cut) {bdt_cut = cut;}
            void setClasses (std::vector<std::string> input_classes, std::vector<std::string> input_classes_cuts) {
                classes = input_classes;
                cuts = input_classes_cuts;
            }
            void setEnablePlotting() {
                enablePlotting = true;
                std::cout << ">>> plotting at screen enabled" << std::endl;
            }
            void unsetProcessCase(int flag) {map_process_case[flag] = false;}
            void setProcessCase(int flag) {map_process_case[flag] = true;}
            void setProcessAllCases() {for(int i=0; i<static_cast<int>(kNCases); i++)    map_process_case[i] = true;}
    
            /// getters
            bool        getIsMC() {return isMC;}
            std::string getInputFile() {return str_file_input;}
            std::string getCaseApplication() {return str_case_application;}
            float       getBdtCut() {return bdt_cut;}
            void        getTTreeFromFile();
            std::vector<TCanvas*> getCanvases() {return vec_canvases;}

            /// analysis functions
            void saveCanvas(TCanvas& can, std::string name);
            void process_labelled(int argc, char **argv);
            void plot1D(std::string name_var, std::vector<TH1D*>& vHistos, std::string str_my_case, std::string strCanvasSuff, std::string final_name, bool normalize=true, bool logy=false);
            
            /// others
            void print_data_members() {
                std::cout << "\n>>> print all data members: " << std::endl;
                std::cout << "isMC: " << isMC << std::endl;
                std::cout << "str_file_input: " << str_file_input << std::endl;
                std::cout << "str_case_application: " << str_case_application << std::endl;
                std::cout << "bdt_cut: " << bdt_cut << std::endl;
                std::cout << "classes: ";
                for(auto c : classes) std::cout << c << " ";
                std::cout << std::endl;
                std::cout << "cuts: " << str_case_application << std::endl;
                for(auto c : cuts) std::cout << c << " ";
                std::cout << std::endl;
                std::cout << "inputTree: " << inputTree << std::endl;
                std::cout << "str_bdt_cut: " << str_bdt_cut << std::endl;

                return;
            }

        private:

            bool isMC;                               // true: application on MC, i.e. labelled output; false: unlabelled output
            std::string str_file_input;              // input file, i.e. output of the application process
            std::string str_case_application;        // application case (eg: LHC22s_pass5)
            
            float bdt_cut;                           // cut on BDT output score
            std::vector<std::string> classes = {};   // classes to be considered in the labelled case
            std::vector<std::string> cuts = {};      // cuts to determined the classes in the labelled case
    
            TTree* inputTree = nullptr;              //! tree from application
            std::vector<TCanvas*> vec_canvases = {}; //

            std::map<int, bool> map_process_case; // map to enable/disable the postprocessing of variables/distributions

            /// miscellanea
            std::vector<int> colors = {kBlue+2, kRed+2};
            std::vector<int> markers = {kFullCircle, kFullCircle};
            std::string str_bdt_cut;
            std::string getStringBdtCut(){
                std::string strBDTcut = " ";
                if(bdt_cut > 0) {
                    /// apply the BDT selection
                    //std::string strBDTcut = std::to_string(bdtCut);
                    std::ostringstream os;
                    os.precision(2);
                    os << std::fixed << bdt_cut;
                    strBDTcut = std::string(" model_output > ") + os.str();
                    for(std::string& str : cuts)    str = str + std::string(" && ") + strBDTcut;
                    std::cout << ">>> Applying BDT cut " << strBDTcut <<  std::endl;
                }

                return strBDTcut;
            }
            bool enablePlotting = false;        // enable plotting at screen of TCanvases
    };
}

#endif