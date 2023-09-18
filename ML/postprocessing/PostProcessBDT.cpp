#include "PostProcessBDT.h"

//_________________________________________________________________________
/// Dummy constructor
ML::PostProcessBDT::PostProcessBDT() :
    isMC(false), str_file_input(""), str_case_application(""), bdt_cut(0), classes{}, cuts{}, inputTree(nullptr), vec_canvases{}
    {
        /// default constructor
        str_bdt_cut = this->getStringBdtCut();
    }
//_________________________________________________________________________
/// constructor
ML::PostProcessBDT::PostProcessBDT(std::string str_input, std::string str_case) :
    isMC(false), bdt_cut(0), classes{}, cuts{}, inputTree(nullptr), vec_canvases{}
    {
        str_file_input = str_input;
        str_case_application = str_case;  
        str_bdt_cut = this->getStringBdtCut(); 
    }
//_________________________________________________________________________
/// destructor
ML::PostProcessBDT::~PostProcessBDT() {
    std::cout << "\nDestroing post processing object..." << std::endl;
};
//
//
//_________________________________________________________________________
void ML::PostProcessBDT::getTTreeFromFile() {
    std::cout << "\n=== ML::PostProcessBDT::getTTreeFromFile() ===" << std::endl;

    TFile* file = TFile::Open(str_file_input.c_str(), "read");
    std::cout << "input file: " << file << std::endl;
    std::cout << "            " << str_file_input << std::endl;

    inputTree = ((TTree*) file->Get("dpgCollsBigML"))->CloneTree();
    inputTree->SetDirectory(0);   /// This MUST be done together with "CloneTree", otherwise the TTree is set to 0 (issues with basket writing)
    std::cout << "inputTree: " << inputTree << std::endl;
    delete file;
    
    return;
}
//
//
//_________________________________________________________________________
void ML::PostProcessBDT::saveCanvas(TCanvas& can, std::string name) {
    can.SaveAs(Form("%s.pdf", name.c_str()));
    can.Update();
    can.Update();
    can.SaveAs(Form("%s.png", name.c_str()));
}
//
//
//_________________________________________________________________________
void ML::PostProcessBDT::plot1D(std::string name_var, std::vector<TH1D*>& vHistos, std::string str_my_case, std::string strCanvasSuff, std::string final_name,
                                bool normalize, bool logy)
{
    gStyle->SetCanvasPreferGL(kTRUE);
    std::cout << "\n=== ML::PostProcessBDT::plot1D() for " << name_var << " ===" << std::endl;

    if ( cuts.size() != classes.size()) {
        std::cout << "[plot1D]: input vectors with different sizes. Skipping the case." << std::endl;
        return;
    }

    /// get histograms for each class
    for(int i=0; i<static_cast<int>(classes.size()); i++) {
        const std::string cut = cuts[i];
        inputTree->Draw(Form("%s>>%s", name_var.c_str(), vHistos[i]->GetName()), cut.c_str(), "goff");
    }

    /// draw
    TCanvas can(Form("can_%s%s", name_var.c_str(), strCanvasSuff.c_str()), str_case_application.c_str(), 800, 650);
    TLegend leg(0.65, 0.7, 0.9, 0.85);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);
    leg.SetTextFont(42);
    leg.SetHeader(str_case_application.c_str());
    for(int i=0; i<static_cast<int>(vHistos.size()); i++) {
        can.cd();
        vHistos[i]->SetMarkerStyle(markers[i]);
        vHistos[i]->SetMarkerColor(colors[i]);
        vHistos[i]->SetMarkerSize(1);
        vHistos[i]->SetLineColor(colors[i]);
        vHistos[i]->SetFillColorAlpha(colors[i], 0.25);
        vHistos[i]->SetStats(kFALSE);
        vHistos[i]->GetXaxis()->SetTitle(name_var.c_str());
        if(normalize){
            vHistos[i]->GetYaxis()->SetTitle("normalized counts");
            vHistos[i]->Scale(1./vHistos[i]->Integral());
            if(logy) vHistos[i]->GetYaxis()->SetRangeUser(1e-5, 1);
        } else {
            vHistos[i]->GetYaxis()->SetTitle("counts");
        }
        vHistos[i]->DrawClone(i==0?"histo":"histosame");
        vHistos[i]->DrawClone("pesame");

        const std::string myclass = classes[i];
        leg.AddEntry(vHistos[i], myclass.c_str(), "pf");
    }

    if(logy) gPad->SetLogy();
    gPad->SetTicks();
    leg.Draw();
    gPad->SetLeftMargin(0.15);
    vec_canvases.emplace_back(dynamic_cast<TCanvas*>(can.Clone()));

    saveCanvas(can, final_name.c_str());

    return;
}
//
//
//_________________________________________________________________________
void ML::PostProcessBDT::process_labelled(int argc, char **argv) {

    TApplication app("app", &argc, argv);

    /// BDT score
    std::vector< TH1D* > histos_MLscore = {};
    std::vector< TH1D* > histos_MLscore_norm = {};
    std::vector< TH1D* > histos_posX = {};
    for(int i_class=0; i_class<static_cast<int>(classes.size()); i_class++) {

        const std::string name_class = classes[i_class];

        histos_MLscore.emplace_back( new TH1D(Form("MLscore_%s", name_class.c_str()), Form("BDT score %s", str_bdt_cut.c_str()), 110, -0.05, 1.05) );
        histos_MLscore_norm.emplace_back( new TH1D(Form("MLscoreNormalized_%s", name_class.c_str()), Form("BDT score normalized %s", str_bdt_cut.c_str()), 110, -0.05, 1.05) );
        histos_posX.emplace_back(new TH1D(Form("posX_%s", name_class.c_str()), Form("PV posX %s", str_bdt_cut.c_str()), 400, -0.2, 0.2));
    }
    plot1D("model_output", histos_MLscore, "BDT_score", "", "1D_model_output", false, true);
    plot1D("model_output", histos_MLscore_norm, "BDT_score normalized", "_normalized", "1D_model_output_normalized", true, true);
    plot1D("fPosX", histos_posX, "Primary vertex posX", "", "1D_posX");


    for(auto& c : vec_canvases) {
        c->Draw();
    }
    app.Run();

    return;
}