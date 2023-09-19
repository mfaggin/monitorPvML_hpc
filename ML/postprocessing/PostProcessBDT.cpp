#include "PostProcessBDT.h"

//_________________________________________________________________________
/// Dummy constructor
ML::PostProcessBDT::PostProcessBDT() :
    isMC(false), str_file_input(""), str_case_application(""), bdt_cut(0), classes{}, cuts{}, inputTree(nullptr), vec_canvases{}
    {
        /// default constructor
        str_bdt_cut = this->getStringBdtCut();
        for(int i=0; i<static_cast<int>(kNCases); i++)    map_process_case[i] = false;
    }
//_________________________________________________________________________
/// constructor
ML::PostProcessBDT::PostProcessBDT(std::string str_input, std::string str_case) :
    isMC(false), bdt_cut(0), classes{}, cuts{}, inputTree(nullptr), vec_canvases{}
    {
        str_file_input = str_input;
        str_case_application = str_case;
        str_bdt_cut = this->getStringBdtCut();
        for(int i=0; i<static_cast<int>(kNCases); i++)    map_process_case[i] = false;
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
    TLegend leg(0.55, 0.65, 0.9, 0.85);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);
    leg.SetTextFont(42);
    leg.SetTextSize(0.030);
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

    std::cout << "=== ML::PostProcessBDT::process_labelled() ===" << std::endl;

    TApplication app("app", &argc, argv);

    ////////////////////////////////////////////
    /// TO DO: IMPLEMENT EFF., PRECISION, RECALL
    std::cout << "\tTO DO: implement eff., precision, recall calculation" << std::endl;
    sleep(3);
    ////////////////////////////////////////////

    std::vector< TH1D* > histos_MLscore = {};
    std::vector< TH1D* > histos_MLscore_norm = {};
    std::vector< TH1D* > histos_IsEvSel = {};
    std::vector< TH1D* > histos_posX = {};
    std::vector< TH1D* > histos_posY = {};
    std::vector< TH1D* > histos_posZ = {};
    std::vector< TH1D* > histos_CovXX = {};
    std::vector< TH1D* > histos_CovXY = {};
    std::vector< TH1D* > histos_CovXZ = {};
    std::vector< TH1D* > histos_CovYY = {};
    std::vector< TH1D* > histos_CovYZ = {};
    std::vector< TH1D* > histos_CovZZ = {};
    std::vector< TH1D* > histos_NumContrib = {};
    std::vector< TH1D* > histos_NumTracksAll = {};
    std::vector< TH1D* > histos_NumTracksFiltered = {};
    std::vector< TH1D* > histos_Chi2 = {};
    std::vector< TH1D* > histos_Ft0PosZ = {};
    std::vector< TH1D* > histos_Ft0PosZ_central = {};
    std::vector< TH1D* > histos_SignalFT0A = {};
    std::vector< TH1D* > histos_SignalFT0C = {};
    std::vector< TH1D* > histos_SignalFT0M = {};
    std::vector< TH1D* > histos_SignalV0A = {};
    std::vector< TH1D* > histos_CollisionTimeRes = {};
    std::vector< TH1D* > histos_RecoPVsPerMcColl = {};
    std::vector< TH1D* > histos_IsPvHighestContribForMcColl = {};
    std::vector< TH1D* > histos_IsDuplicate = {};
    for(int i_class=0; i_class<static_cast<int>(classes.size()); i_class++) {

        const std::string name_class = classes[i_class];

        histos_MLscore.emplace_back( new TH1D(Form("MLscore_%s", name_class.c_str()), Form("BDT score %s", str_bdt_cut.c_str()), 110, -0.05, 1.05) );
        histos_MLscore_norm.emplace_back( new TH1D(Form("MLscoreNormalized_%s", name_class.c_str()), Form("BDT score normalized %s", str_bdt_cut.c_str()), 110, -0.05, 1.05) );
        ///
        histos_IsEvSel.emplace_back(new TH1D(Form("IsEvSel_%s", name_class.c_str()), Form("is event selected %s", str_bdt_cut.c_str()), 4, -1, 3));
        ///
        histos_posX.emplace_back(new TH1D(Form("posX_%s", name_class.c_str()), Form("PV posX %s", str_bdt_cut.c_str()), 400, -0.2, 0.2));
        histos_posY.emplace_back(new TH1D(Form("posY_%s", name_class.c_str()), Form("PV posY %s", str_bdt_cut.c_str()), 400, -0.2, 0.2));
        histos_posZ.emplace_back(new TH1D(Form("posZ_%s", name_class.c_str()), Form("PV posZ %s", str_bdt_cut.c_str()), 80, -20, 20));
        ///
        histos_CovXX.emplace_back(new TH1D(Form("CovXX_%s", name_class.c_str()), Form("PV CovXX %s", str_bdt_cut.c_str()), 500, 0, 0.05e-03) );
        histos_CovXY.emplace_back(new TH1D(Form("CovXY_%s", name_class.c_str()), Form("PV CovXY %s", str_bdt_cut.c_str()), 500, -0.025e-03, 0.025e-03) );
        histos_CovXZ.emplace_back(new TH1D(Form("CovXZ_%s", name_class.c_str()), Form("PV CovXZ %s", str_bdt_cut.c_str()), 500, -0.1e-03, 0.1e-03) );
        histos_CovYY.emplace_back(new TH1D(Form("CovYY_%s", name_class.c_str()), Form("PV CovYY %s", str_bdt_cut.c_str()), 500, 0, 0.05e-03) );
        histos_CovYZ.emplace_back(new TH1D(Form("CovYZ_%s", name_class.c_str()), Form("PV CovYZ %s", str_bdt_cut.c_str()), 500, -0.1e-03, 0.1e-03) );
        histos_CovZZ.emplace_back(new TH1D(Form("CovZZ_%s", name_class.c_str()), Form("PV CovZZ %s", str_bdt_cut.c_str()), 500, 0, 0.002) );
        ///
        histos_NumContrib.emplace_back(new TH1D(Form("NumContrib_%s", name_class.c_str()), Form("PV contributors %s", str_bdt_cut.c_str()), 150, 0, 150));
        histos_NumTracksAll.emplace_back(new TH1D(Form("NumTracksAll_%s", name_class.c_str()), Form("All tracks %s", str_bdt_cut.c_str()), 400, 0, 400));
        histos_NumTracksFiltered.emplace_back(new TH1D(Form("NumTracksFiltered_%s", name_class.c_str()), Form("Filtered tracks %s", str_bdt_cut.c_str()), 100, 0, 100));
        ///
        histos_Chi2.emplace_back(new TH1D(Form("Chi2_%s", name_class.c_str()), Form("PV chi2 %s", str_bdt_cut.c_str()), 400, 0, 400));
        ///
        histos_Ft0PosZ.emplace_back(new TH1D(Form("Ft0PosZ_%s", name_class.c_str()), Form("PV Ft0PosZ %s", str_bdt_cut.c_str()), 10, -1000, 1000));
        histos_Ft0PosZ_central.emplace_back(new TH1D(Form("Ft0PosZ_central_%s", name_class.c_str()), Form("PV Ft0PosZ (no bad values) %s", str_bdt_cut.c_str()), 10, -1000, 1000));
        histos_SignalFT0A.emplace_back(new TH1D(Form("SignalFT0A_%s", name_class.c_str()), Form("PV SignalFT0A %s", str_bdt_cut.c_str()), 500, 0, 5000));
        histos_SignalFT0C.emplace_back(new TH1D(Form("SignalFT0C_%s", name_class.c_str()), Form("PV SignalFT0C %s", str_bdt_cut.c_str()), 150, 0, 1500));
        histos_SignalFT0M.emplace_back(new TH1D(Form("SignalFT0M_%s", name_class.c_str()), Form("PV SignalFT0M %s", str_bdt_cut.c_str()), 500, 0, 5000));
        histos_SignalV0A.emplace_back(new TH1D(Form("SignalV0A_%s", name_class.c_str()), Form("PV SignalV0A %s", str_bdt_cut.c_str()), 800, 0, 8000));
        ///
        histos_CollisionTimeRes.emplace_back(new TH1D(Form("CollisionTimeRes_%s", name_class.c_str()), Form("Collision time resolution %s", str_bdt_cut.c_str()), 200, 0, 2000));
        histos_RecoPVsPerMcColl.emplace_back(new TH1D(Form("RecoPVsPerMcColl_%s", name_class.c_str()), Form("# reco PVs per MC collision %s", str_bdt_cut.c_str()), 5, 0, 5));
        histos_IsPvHighestContribForMcColl.emplace_back(new TH1D(Form("IsPvHighestContribForMcColl_%s", name_class.c_str()), Form("is PV with highest number of contributors %s", str_bdt_cut.c_str()), 5, 0, 5));
        histos_IsDuplicate.emplace_back(new TH1D(Form("IsDuplicate_%s", name_class.c_str()), Form("is duplicate %s", str_bdt_cut.c_str()), 5, 0, 5));
    }
    if(map_process_case[ML::kBdtScore])             plot1D("model_output", histos_MLscore, "BDT_score", "", "1D_model_output", false, true);
    if(map_process_case[ML::kBdtScoreNormalized])   plot1D("model_output", histos_MLscore_norm, "BDT_score normalized", "_normalized", "1D_model_output_normalized", true, true);
    ///
    if(map_process_case[ML::kIsEvSel])              plot1D("fIsEventSelected", histos_IsEvSel, "BDT_score normalized", "_normalized", "1D_model_output_normalized", true, true);
    ///
    if(map_process_case[ML::kPosX])     plot1D("fPosX", histos_posX, "Primary vertex posX", "", "1D_posX");
    if(map_process_case[ML::kPosY])     plot1D("fPosY", histos_posY, "Primary vertex posY", "", "1D_posY");
    if(map_process_case[ML::kPosZ])     plot1D("fPosZ", histos_posZ, "Primary vertex posZ", "", "1D_posZ");
    ///
    if(map_process_case[ML::kCovXX])    plot1D("fCovXX", histos_CovXX, "PV cov matrix element XX", "", "1D_covXX", true, true);
    if(map_process_case[ML::kCovXY])    plot1D("fCovXY", histos_CovXY, "PV cov matrix element XX", "", "1D_covXY", true, true);
    if(map_process_case[ML::kCovXZ])    plot1D("fCovXZ", histos_CovXZ, "PV cov matrix element XX", "", "1D_covXZ", true, true);
    if(map_process_case[ML::kCovYY])    plot1D("fCovYY", histos_CovYY, "PV cov matrix element XX", "", "1D_covYY", true, true);
    if(map_process_case[ML::kCovYZ])    plot1D("fCovYZ", histos_CovYZ, "PV cov matrix element XX", "", "1D_covYZ", true, true);
    if(map_process_case[ML::kCovZZ])    plot1D("fCovZZ", histos_CovZZ, "PV cov matrix element XX", "", "1D_covZZ", true, true);
    ///
    if(map_process_case[ML::kNumContrib])           plot1D("fNumContrib", histos_NumContrib, "PV contributors", "", "1D_numContrib", true, true);
    if(map_process_case[ML::kNumTracksAll])         plot1D("fNumTracksAll", histos_NumTracksAll, "All tracks", "", "1D_allTracks", true, true);
    if(map_process_case[ML::kNumtracksFiltered])    plot1D("fNumTracksFiltered", histos_NumTracksFiltered, "Filtered tracks", "", "1D_filteredTracks", true, true);
    ///
    if(map_process_case[kPvChi2])   plot1D("fChi2", histos_Chi2, "PV Chi2", "", "1D_Chi2", true, true);
    ///
    if(map_process_case[kFt0PosZ]) {
        plot1D("fFt0PosZ", histos_Ft0PosZ, "PV Ft0PosZ", "", "1D_Ft0PosZ", true, true);
        plot1D("fFt0PosZ", histos_Ft0PosZ_central, "PV Ft0PosZ (no bad values)", "_central", "1D_Ft0PosZ_central", true, false);
    }
    ///
    if(map_process_case[kFt0Asignal])   plot1D("fSignalFT0A", histos_SignalFT0A, "FT0A signal", "", "1D_signalFT0A", true, true);
    if(map_process_case[kFt0Csignal])   plot1D("fSignalFT0C", histos_SignalFT0C, "FT0C signal", "", "1D_signalFT0C", true, true);
    if(map_process_case[kFt0Msignal])   plot1D("fSignalFT0M", histos_SignalFT0M, "FT0M signal", "", "1D_signalFT0M", true, true);
    if(map_process_case[kV0Asignal])    plot1D("fSignalV0A", histos_SignalV0A, "V0A signal", "", "1D_signalV0A", true, true);
    ///
    if(map_process_case[kCollTimeRes])                  plot1D("fCollisionTimeRes", histos_CollisionTimeRes, "Collision time resolution", "", "1D_collisionTimeRes", true, true);
    if(map_process_case[kNRecoPvPerMcColl])             plot1D("fRecoPVsPerMcColl", histos_RecoPVsPerMcColl, "# reco PVs per MC collision", "", "1D_control_RecoPVsPerMcColl", true, true);
    if(map_process_case[kIsPvHighestContribForMcColl])  plot1D("fIsPvHighestContribForMcColl", histos_IsPvHighestContribForMcColl, "is PV with highest number of contributors", "", "1D_control_IsPvHighestContribForMcColl", true, false);
    if(map_process_case[kIsDuplicate])                  plot1D("fIsDuplicate", histos_IsDuplicate, "is duplicate", "", "1D_control_IsDuplicate", true, false);

    if(enablePlotting) {
        for(auto& c : vec_canvases) {
            c->Draw();
            c->Update();
        }
        app.Run();
    }

    return;
}