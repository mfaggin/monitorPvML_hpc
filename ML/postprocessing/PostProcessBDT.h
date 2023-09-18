#ifndef PostProcessBDT_h
#define PostProcessBDT_h

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TStyle.h"

namespace ML {
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
        
            bool isMC;                          // true: application on MC, i.e. labelled output; false: unlabelled output
            std::string str_file_input;         // input file, i.e. output of the application process
            std::string str_case_application;   // application case (eg: LHC22s_pass5)
            
            float bdt_cut;                      // cut on BDT output score
            std::vector<std::string> classes = {};   // classes to be considered in the labelled case
            std::vector<std::string> cuts = {};      // cuts to determined the classes in the labelled case
    
            TTree* inputTree = nullptr;         //! tree from application
            std::vector<TCanvas*> vec_canvases = {};

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
    };
}

#endif