#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

#include <string>
#include <vector>
#include <iostream>
#include <numeric>
#include <math.h>
#include <sstream>

std::vector<int> colors = {kBlue+2, kRed+2};
std::vector<int> markers = {kFullCircle, kFullCircle};

bool delete_canvases = false;

////////////////////
////////////////////
TCanvas* plot1D(TTree* tree, std::string name, std::vector<std::string> vCuts, std::vector<std::string> vClasses, std::vector<TH1D*>& vHistos, 
                std::string strMyCase, std::string strCanvasSuff, std::string strCaseAppl, bool normalize=true, bool logy=false);
TCanvas* plot2D(TTree* tree, std::string name1, std::string name2, std::vector<TH2D*> vHistos, std::vector<std::string> vCuts, std::vector<std::string> vClasses,
                std::string strMyCase, std::string strCanvasSuff, std::string strCaseAppl);
TCanvas* plotEffRejPrecisionRecall(std::vector<std::string> vClasses, std::vector<int> vPositive, std::vector<TH1D*>& vHistos,
                                   std::string name, std::string strCanvasSuff, std::string strMyCase, std::string strCaseAppl);
////////////////////
////////////////////
void saveCanvas(TCanvas* can, std::string name) {
    can->SaveAs(Form("%s.pdf", name.c_str()));
    can->Update();
    can->Update();
    can->SaveAs(Form("%s.png", name.c_str()));
    //can->SaveAs(Form("%s.root", name.c_str()));
    if(delete_canvases) delete can;
}
////////////////////
////////////////////
void postprocessML_labelled(std::string strFileInput="../appliedBDT_LHC22i1_all.root", std::string strCaseAppl="LHC22i1", double bdtCut=-1., bool del_can=false) {

    delete_canvases = del_can;
    gStyle->SetCanvasPreferGL(kTRUE);

    /// categories to be considered
    std::vector<std::string> classes = {"single PV", "duplicated PV"};
    std::vector<std::string> cuts = {"fIsDuplicate < 1", "fIsDuplicate > 0"};
    std::string strBDTcut = " ";
    if(bdtCut > 0) {
        /// apply the BDT selection
        //std::string strBDTcut = std::to_string(bdtCut);
        std::ostringstream os;
        os.precision(2);
        os << std::fixed << bdtCut;
        strBDTcut = std::string(" model_output > ") + os.str();
        for(std::string& str : cuts)    str = str + std::string(" && ") + strBDTcut;
        std::cout << ">>> Applying BDT cut " << strBDTcut <<  std::endl;
    }

    /// input file TTree after the model application
    TFile* fileInput = TFile::Open(strFileInput.c_str());
    TTree* tree = dynamic_cast<TTree*>(fileInput->Get("dpgCollsBigML"));

    /////////////////////////
    /// 1-D distributions ///
    /////////////////////////

    // BDT score 
    std::vector<TH1D*> histos_MLscore = {new TH1D("MLscore_singlePv", Form("BDT score %s", strBDTcut.c_str()), 110, -0.05, 1.05), new TH1D("MLscore_duplicatePv", Form("BDT score %s", strBDTcut.c_str()), 110, -0.05, 1.05)};
    TCanvas* canBDT = plot1D(tree, "model_output", cuts, classes, histos_MLscore, "BDT score", "", strCaseAppl, false, true);
    TCanvas* canBDT_EffRejPrecisionRecall = plotEffRejPrecisionRecall(classes, std::vector<int>{0,1}, histos_MLscore, "model_output", "", strBDTcut.c_str(), strCaseAppl);
    saveCanvas(canBDT, "1D_model_output");
    saveCanvas(canBDT_EffRejPrecisionRecall, "1D_model_output_EffRejPrecisionRecall");

    // BDT score normalized
    std::vector<TH1D*> histos_MLscore_norm = {new TH1D("MLscoreNormalized_singlePv", Form("BDT score %s", strBDTcut.c_str()), 110, -0.05, 1.05), new TH1D("MLscoreNormalized_duplicatePv", Form("BDT score %s", strBDTcut.c_str()), 110, -0.05, 1.05)};
    TCanvas* canBDT_normalized = plot1D(tree, "model_output", cuts, classes, histos_MLscore_norm, "BDT score normalized", "_normalized", strCaseAppl, true, true);
    // The precision curve strongly depends on the number of counts of both classes.
    // It has no sense to do it with the normalized counts. Skipping (the others curve do not depend on normalization).
    //plotEffRejPrecisionRecall(classes, std::vector<int>{0,1}, histos_MLscore_norm, "model_output", "_normalized", "BDT score normalized eff., rej., precision, recall", strCaseAppl);
    saveCanvas(canBDT_normalized, "1D_model_output_normalized");

    // isEventSelected
    std::vector<TH1D*> histos_IsEvSel = {new TH1D("IsEvSel_singlePv", Form("is event selected %s", strBDTcut.c_str()), 4, -1, 3), new TH1D("IsEvSel_duplicatePv", Form("is event selected %s", strBDTcut.c_str()), 4, -1, 3)};
    TCanvas* canIsEvSel = plot1D(tree, "fIsEventSelected", cuts, classes, histos_IsEvSel, "is event selected", "", strCaseAppl, true, true);
    saveCanvas(canIsEvSel, "1D_control_isEventSelected");

    // PV coordinates
    std::vector<TH1D*> histos_posX = {new TH1D("posX_singlePv", Form("PV posX %s", strBDTcut.c_str()), 400, -0.2, 0.2), new TH1D("posX_duplicatePv", Form("PV posX %s", strBDTcut.c_str()), 400, -0.2, 0.2)};
    std::vector<TH1D*> histos_posY = {new TH1D("posY_singlePv", Form("PV posY %s", strBDTcut.c_str()), 400, -0.2, 0.2), new TH1D("posY_duplicatePv", Form("PV posY %s", strBDTcut.c_str()), 400, -0.2, 0.2)};
    std::vector<TH1D*> histos_posZ = {new TH1D("posZ_singlePv", Form("PV posZ %s", strBDTcut.c_str()), 80, -20, 20)   , new TH1D("posZ_duplicatePv", Form("PV posZ %s", strBDTcut.c_str()), 80, -20, 20)};
    TCanvas* canPosX = plot1D(tree, "fPosX", cuts, classes, histos_posX, "Primary vertex posX", "", strCaseAppl);
    TCanvas* canPosY = plot1D(tree, "fPosY", cuts, classes, histos_posY, "Primary vertex posY", "", strCaseAppl);
    TCanvas* canPosZ = plot1D(tree, "fPosZ", cuts, classes, histos_posZ, "Primary vertex posZ", "", strCaseAppl);
    saveCanvas(canPosX, "1D_posX");
    saveCanvas(canPosY, "1D_posY");
    saveCanvas(canPosZ, "1D_posZ");

    // PV covariance matrix
    std::vector<TH1D*> histos_CovXX = {new TH1D("CovXX_singlePv", Form("PV CovXX %s", strBDTcut.c_str()), 500, 0, 0.05e-03)             , new TH1D("CovXX_duplicatePv", Form("PV CovXX %s", strBDTcut.c_str()), 500, 0, 0.05e-03)};
    std::vector<TH1D*> histos_CovXY = {new TH1D("CovXY_singlePv", Form("PV CovXY %s", strBDTcut.c_str()), 500, -0.025e-03, 0.025e-03)   , new TH1D("CovXY_duplicatePv", Form("PV CovXY %s", strBDTcut.c_str()), 500, -0.025e-03, 0.025e-03)};
    std::vector<TH1D*> histos_CovXZ = {new TH1D("CovXZ_singlePv", Form("PV CovXZ %s", strBDTcut.c_str()), 500, -0.1e-03, 0.1e-03)       , new TH1D("CovXZ_duplicatePv", Form("PV CovXZ %s", strBDTcut.c_str()), 500, -0.1e-03, 0.1e-03)};
    std::vector<TH1D*> histos_CovYY = {new TH1D("CovYY_singlePv", Form("PV CovYY %s", strBDTcut.c_str()), 500, 0, 0.05e-03)             , new TH1D("CovYY_duplicatePv", Form("PV CovYY %s", strBDTcut.c_str()), 500, 0, 0.05e-03)};
    std::vector<TH1D*> histos_CovYZ = {new TH1D("CovYZ_singlePv", Form("PV CovYZ %s", strBDTcut.c_str()), 500, -0.1e-03, 0.1e-03)       , new TH1D("CovYZ_duplicatePv", Form("PV CovYZ %s", strBDTcut.c_str()), 500, -0.1e-03, 0.1e-03)};
    std::vector<TH1D*> histos_CovZZ = {new TH1D("CovZZ_singlePv", Form("PV CovZZ %s", strBDTcut.c_str()), 400, 0, 0.002)                , new TH1D("CovZZ_duplicatePv", Form("PV CovZZ %s", strBDTcut.c_str()), 500, 0, 0.002)};
    TCanvas* canCovXX = plot1D(tree, "fCovXX", cuts, classes, histos_CovXX, "PV cov matrix element XX", "", strCaseAppl, true, true);
    TCanvas* canCovXY = plot1D(tree, "fCovXY", cuts, classes, histos_CovXY, "PV cov matrix element XY", "", strCaseAppl, true, true);
    TCanvas* canCovXZ = plot1D(tree, "fCovXZ", cuts, classes, histos_CovXZ, "PV cov matrix element XZ", "", strCaseAppl, true, true);
    TCanvas* canCovYY = plot1D(tree, "fCovYY", cuts, classes, histos_CovYY, "PV cov matrix element YY", "", strCaseAppl, true, true);
    TCanvas* canCovYZ = plot1D(tree, "fCovYZ", cuts, classes, histos_CovYZ, "PV cov matrix element YZ", "", strCaseAppl, true, true);
    TCanvas* canCovZZ = plot1D(tree, "fCovZZ", cuts, classes, histos_CovZZ, "PV cov matrix element ZZ", "", strCaseAppl, true, true);
    saveCanvas(canCovXX, "1D_covXX");
    saveCanvas(canCovXY, "1D_covXY");
    saveCanvas(canCovXZ, "1D_covXZ");
    saveCanvas(canCovYY, "1D_covYY");
    saveCanvas(canCovYZ, "1D_covYZ");
    saveCanvas(canCovZZ, "1D_covZZ");

    // PV contributors
    std::vector<TH1D*> histos_NumContrib = {new TH1D("NumContrib_singlePv", Form("PV contributors %s", strBDTcut.c_str()), 150, 0, 150), new TH1D("NumContrib_duplicatePv", Form("PV contributors %s", strBDTcut.c_str()), 150, 0, 150)};
    TCanvas* canNumContrib = plot1D(tree, "fNumContrib", cuts, classes, histos_NumContrib, "PV contributors", "", strCaseAppl, true, true);
    saveCanvas(canNumContrib, "1D_numContrib");

    // number of tracks
    std::vector<TH1D*> histos_NumTracksAll = {new TH1D("NumTracksAll_singlePv", Form("All tracks %s", strBDTcut.c_str()), 400, 0, 400), new TH1D("NumTracksAll_duplicatePv", Form("All tracks %s", strBDTcut.c_str()), 400, 0, 400)};
    std::vector<TH1D*> histos_NumTracksFiltered = {new TH1D("NumTracksFiltered_singlePv", Form("Filtered tracks %s", strBDTcut.c_str()), 100, 0, 100), new TH1D("NumTracksFiltered_duplicatePv", Form("Filtered tracks %s", strBDTcut.c_str()), 100, 0, 100)};
    TCanvas* canNumTracksAll = plot1D(tree, "fNumTracksAll", cuts, classes, histos_NumTracksAll, "All tracks", "", strCaseAppl, true, true);
    TCanvas* canNumTracksFiltered = plot1D(tree, "fNumTracksFiltered", cuts, classes, histos_NumTracksFiltered, "All tracks", "", strCaseAppl, true, true);
    saveCanvas(canNumTracksAll, "1D_allTracks");
    saveCanvas(canNumTracksFiltered, "1D_filteredTracks");

    // PV chi2
    std::vector<TH1D*> histos_Chi2 = {new TH1D("Chi2_singlePv", Form("PV chi2 %s", strBDTcut.c_str()), 400, 0, 400), new TH1D("Chi2_duplicatePv", Form("PV chi2 %s", strBDTcut.c_str()), 400, 0, 400)};
    TCanvas* canChi2 = plot1D(tree, "fChi2", cuts, classes, histos_Chi2, "PV chi2", "", strCaseAppl, true, true);
    saveCanvas(canChi2, "1D_Chi2");

    // FT0 posZ
    std::vector<TH1D*> histos_Ft0PosZ = {new TH1D("Ft0PosZ_singlePv", Form("PV Ft0PosZ %s", strBDTcut.c_str()), 10, -1000, 1000), new TH1D("Ft0PosZ_duplicatePv", Form("PV Ft0PosZ %s", strBDTcut.c_str()), 10, -1000, 1000)};
    std::vector<TH1D*> histos_Ft0PosZ_central = {new TH1D("Ft0PosZ_central_singlePv", Form("PV Ft0PosZ (no bad values) %s", strBDTcut.c_str()), 80, -20, 20), new TH1D("Ft0PosZ_central_duplicatePv", Form("PV Ft0PosZ (no bad values) %s", strBDTcut.c_str()), 80, -20, 20)};
    TCanvas* canFt0PosZ = plot1D(tree, "fFt0PosZ", cuts, classes, histos_Ft0PosZ, "PV Ft0PosZ (no bad values)", "", strCaseAppl, true, true);
    TCanvas* canFt0PosZ_central = plot1D(tree, "fFt0PosZ", cuts, classes, histos_Ft0PosZ_central, "PV Ft0PosZ (no bad values)", "_central", strCaseAppl, true, false);
    saveCanvas(canFt0PosZ, "1D_Ft0PosZ");
    saveCanvas(canFt0PosZ_central, "1D_Ft0PosZ_central");

    // FIT signals
    std::vector<TH1D*> histos_SignalFT0A = {new TH1D("SignalFT0A_singlePv", Form("PV SignalFT0A %s", strBDTcut.c_str()), 500, 0, 5000), new TH1D("SignalFT0A_duplicatePv", Form("PV SignalFT0A %s", strBDTcut.c_str()), 500, 0, 5000)};
    std::vector<TH1D*> histos_SignalFT0C = {new TH1D("SignalFT0C_singlePv", Form("PV SignalFT0C %s", strBDTcut.c_str()), 150, 0, 1500), new TH1D("SignalFT0C_duplicatePv", Form("PV SignalFT0C %s", strBDTcut.c_str()), 150, 0, 1500)};
    std::vector<TH1D*> histos_SignalFT0M = {new TH1D("SignalFT0M_singlePv", Form("PV SignalFT0M %s", strBDTcut.c_str()), 500, 0, 5000), new TH1D("SignalFT0M_duplicatePv", Form("PV SignalFT0M %s", strBDTcut.c_str()), 500, 0, 5000)};
    std::vector<TH1D*> histos_SignalV0A  = {new TH1D("SignalV0A_singlePv" , Form("PV SignalV0A %s", strBDTcut.c_str()), 800, 0, 8000) , new TH1D("SignalV0A_duplicatePv" , Form("PV SignalV0A %s", strBDTcut.c_str()), 800, 0, 8000)};
    TCanvas* canSignalFT0A = plot1D(tree, "fSignalFT0A", cuts, classes, histos_SignalFT0A, "FT0A signal", "", strCaseAppl, true, true);
    TCanvas* canSignalFT0C = plot1D(tree, "fSignalFT0C", cuts, classes, histos_SignalFT0C, "FT0C signal", "", strCaseAppl, true, true);
    TCanvas* canSignalFT0M = plot1D(tree, "fSignalFT0M", cuts, classes, histos_SignalFT0M, "FT0M signal", "", strCaseAppl, true, true);
    TCanvas* canSignalV0A = plot1D(tree, "fSignalV0A", cuts, classes, histos_SignalV0A, "V0A signal", "", strCaseAppl, true, true);
    saveCanvas(canSignalFT0A, "1D_signalFT0A");
    saveCanvas(canSignalFT0C, "1D_signalFT0C");
    saveCanvas(canSignalFT0M, "1D_signalFT0M");
    saveCanvas(canSignalV0A, "1D_signalV0A");

    // collision time resolution
    std::vector<TH1D*> histos_CollisionTimeRes = {new TH1D("CollisionTimeRes_singlePv", Form("Collision time resolution %s", strBDTcut.c_str()), 200, 0, 2000), new TH1D("CollisionTimeRes_duplicatePv", Form("Collision time resolution %s", strBDTcut.c_str()), 200, 0, 2000)};
    TCanvas* canCollisionTimeRes = plot1D(tree, "fCollisionTimeRes", cuts, classes, histos_CollisionTimeRes, "Collision time resolution", "", strCaseAppl, true, true);
    saveCanvas(canCollisionTimeRes, "1D_collisionTimeRes");

    // control plot - # reco PV per MC collision
    std::vector<TH1D*> histos_RecoPVsPerMcColl = {new TH1D("RecoPVsPerMcColl_singlePv", Form("# reco PVs per MC collision %s", strBDTcut.c_str()), 5, 0, 5), new TH1D("RecoPVsPerMcColl_duplicatePv", Form("# reco PVs per MC collision %s", strBDTcut.c_str()), 5, 0, 5)};
    TCanvas* canRecoPVsPerMcColl = plot1D(tree, "fRecoPVsPerMcColl", cuts, classes, histos_RecoPVsPerMcColl, "# reco PVs per MC collision", "", strCaseAppl, true, false);
    saveCanvas(canRecoPVsPerMcColl, "1D_control_RecoPVsPerMcColl");

    // control plot - is PV with highest mumber of contributors for the considered MC collision
    std::vector<TH1D*> histos_IsPvHighestContribForMcColl = {new TH1D("IsPvHighestContribForMcColl_singlePv", Form("is PV with highest number of contributors %s", strBDTcut.c_str()), 5, 0, 5), new TH1D("IsPvHighestContribForMcColl_duplicatePv", Form("is PV with highest number of contributors %s", strBDTcut.c_str()), 2, 0, 2)};
    TCanvas* canIsPvHighestContribForMcColl = plot1D(tree, "fIsPvHighestContribForMcColl", cuts, classes, histos_IsPvHighestContribForMcColl, "is PV with highest number of contributors", "", strCaseAppl, true, false);
    saveCanvas(canIsPvHighestContribForMcColl, "1D_control_IsPvHighestContribForMcColl");

    // control plot - is duplicate
    std::vector<TH1D*> histos_IsDuplicate = {new TH1D("IsDuplicate_singlePv", Form("is duplicate %s", strBDTcut.c_str()), 5, 0, 5), new TH1D("IsDuplicate_duplicatePv", Form("is duplicate %s", strBDTcut.c_str()), 2, 0, 2)};
    TCanvas* canIsDuplicate = plot1D(tree, "fIsDuplicate", cuts, classes, histos_IsDuplicate, "is duplicate", "", strCaseAppl, true, false);
    saveCanvas(canIsDuplicate, "1D_control_IsDuplicate");


    /////////////////////////
    /// 2-D distributions ///
    /////////////////////////

    // posX vs posY
    std::vector<TH2D*> histos2D_PosXvsPosY = {new TH2D("PosXvsPosY_singlePv", Form("posX vs. posY %s", strBDTcut.c_str()), 400, -0.2, 0.2, 400, -0.2, 0.2), new TH2D("PosXvsPosY_duplicatePv", Form("posX vs. posY %s", strBDTcut.c_str()), 400, -0.2, 0.2, 400, -0.2, 0.2)};
    TCanvas* canPosXvPosY = plot2D(tree, "fPosX", "fPosY", histos2D_PosXvsPosY, cuts, classes, "posX vs posY", "", strCaseAppl);
    saveCanvas(canPosXvPosY, "2D_posX_vs_posY");

    // posX vs posXMC
    std::vector<TH2D*> histos2D_PosXvsPosXMC = {new TH2D("PosXvsPosXMC_singlePv", Form("posX vs. posXMC %s", strBDTcut.c_str()), 400, -0.2, 0.2, 400, -0.2, 0.2), new TH2D("PosXvsPosXMC_duplicatePv", Form("posX vs. posXMC %s", strBDTcut.c_str()), 400, -0.2, 0.2, 400, -0.2, 0.2)};
    TCanvas* canPosXvPosXMC = plot2D(tree, "fPosX", "fPosXMC", histos2D_PosXvsPosXMC, cuts, classes, "posX vs posXMC", "", strCaseAppl);
    saveCanvas(canPosXvPosXMC, "2D_posX_vs_PosXMC");

    // posY vs posYMC
    std::vector<TH2D*> histos2D_PosYvsPosYMC = {new TH2D("PosYvsPosYMC_singlePv", Form("posY vs. posYMC %s", strBDTcut.c_str()), 400, -0.2, 0.2, 400, -0.2, 0.2), new TH2D("PosYvsPosYMC_duplicatePv", Form("posY vs. posYMC %s", strBDTcut.c_str()), 400, -0.2, 0.2, 400, -0.2, 0.2)};
    TCanvas* canPosYvPosYMC = plot2D(tree, "fPosY", "fPosYMC", histos2D_PosYvsPosYMC, cuts, classes, "posY vs posYMC", "", strCaseAppl);
    saveCanvas(canPosYvPosYMC, "2D_posY_vs_PosYMC");

    // posZ vs posZMC
    std::vector<TH2D*> histos2D_PosZvsPosZMC = {new TH2D("PosZvsPosZMC_singlePv", Form("posZ vs. posZMC %s", strBDTcut.c_str()), 80, -20, 20, 80, -20, 20), new TH2D("PosZvsPosZMC_duplicatePv", Form("posZ vs. posZMC %s", strBDTcut.c_str()), 80, -20, 20, 80, -20, 20)};
    TCanvas* canPosZvPosZMC = plot2D(tree, "fPosZ", "fPosZMC", histos2D_PosZvsPosZMC, cuts, classes, "posZ vs posZMC", "", strCaseAppl);
    saveCanvas(canPosZvPosZMC, "2D_posZ_vs_PosZMC");

    // PosZ vs. Ft0PosZ
    std::vector<TH2D*> histos2D_PosZvsFt0PosZ = {new TH2D("PosZvsFt0PosZ_singlePv", Form("PosZ vs. Ft0PosZ %s", strBDTcut.c_str()), 80, -20, 20, 10, -1000, 1000), new TH2D("PosZvsFt0PosZ_duplicatePv", Form("PosZ vs. Ft0PosZ %s", strBDTcut.c_str()), 80, -20, 20, 10, -1000, 1000)};
    TCanvas* canPosZvFt0PosZ = plot2D(tree, "fPosZ", "fFt0PosZ", histos2D_PosZvsFt0PosZ, cuts, classes, "PosZ vs Ft0PosZ", "", strCaseAppl);
    saveCanvas(canPosZvFt0PosZ, "2D_PosZ_vs_Ft0PosZ");

    // PosZ vs. Ft0PosZ (no bad values)
    std::vector<TH2D*> histos2D_PosZvsFt0PosZ_central = {new TH2D("PosZvsFt0PosZ_central_singlePv", Form("PosZ vs. Ft0PosZ (no bad values) %s", strBDTcut.c_str()), 80, -20, 20, 80, -20, 20), new TH2D("PosZvsFt0PosZ_central_duplicatePv", Form("PosZ vs. Ft0PosZ (no bad values) %s", strBDTcut.c_str()), 80, -20, 20, 80, -20, 20)};
    TCanvas* canPosZvFt0PosZ_central = plot2D(tree, "fPosZ", "fFt0PosZ", histos2D_PosZvsFt0PosZ_central, cuts, classes, "PosZ vs Ft0PosZ (no bad values)", "_central", strCaseAppl);
    saveCanvas(canPosZvFt0PosZ_central, "2D_PosZ_vs_Ft0PosZ_central");

    // CovXX vs CovYY
    std::vector<TH2D*> histos2D_CovXXvsCovYY = {new TH2D("CovXXvsCovYY_singlePv", Form("CovXX vs. CovYY %s", strBDTcut.c_str()), 500, 0, 0.05e-03, 500, 0, 0.05e-03), new TH2D("CovXXvsCovYY_duplicatePv", Form("CovXX vs. CovYY %s", strBDTcut.c_str()), 500, 0, 0.05e-03, 500, 0, 0.05e-03)};
    TCanvas* canCovXXvCovYY = plot2D(tree, "fCovXX", "fCovYY", histos2D_CovXXvsCovYY, cuts, classes, "CovXX vs CovYY", "", strCaseAppl);
    saveCanvas(canCovXXvCovYY, "2D_CovXX_vs_CovYY");

    // CovXX vs CovZZ
    std::vector<TH2D*> histos2D_CovXXvsCovZZ = {new TH2D("CovXXvsCovZZ_singlePv", Form("CovXX vs. CovZZ %s", strBDTcut.c_str()), 500, 0, 0.05e-03, 500, 0, 0.05e-03), new TH2D("CovXXvsCovZZ_duplicatePv", Form("CovXX vs. CovZZ %s", strBDTcut.c_str()), 500, 0, 0.05e-03, 500, 0, 0.05e-03)};
    TCanvas* canCovXXvCovZZ = plot2D(tree, "fCovXX", "fCovZZ", histos2D_CovXXvsCovZZ, cuts, classes, "CovXX vs CovZZ", "", strCaseAppl);
    saveCanvas(canCovXXvCovZZ, "2D_CovXX_vs_CovZZ");

    // CovYY vs CovZZ
    std::vector<TH2D*> histos2D_CovYYvsCovZZ = {new TH2D("CovYYvsCovZZ_singlePv", Form("CovYY vs. CovZZ %s", strBDTcut.c_str()), 500, 0, 0.05e-03, 500, 0, 0.05e-03), new TH2D("CovYYvsCovZZ_duplicatePv", Form("CovYY vs. CovZZ %s", strBDTcut.c_str()), 500, 0, 0.05e-03, 500, 0, 0.05e-03)};
    TCanvas* canCovYYvCovZZ = plot2D(tree, "fCovYY", "fCovZZ", histos2D_CovYYvsCovZZ, cuts, classes, "CovYY vs CovZZ", "", strCaseAppl);
    saveCanvas(canCovYYvCovZZ, "2D_CovYY_vs_CovZZ");

    // numContrib vs. numTracksAll
    std::vector<TH2D*> histos2D_numContribvsnumTracksAll = {new TH2D("numContribvsnumTracksAll_singlePv", Form("numContrib vs. numTracksAll %s", strBDTcut.c_str()), 150, 0, 150, 400, 0, 400), new TH2D("numContribvsnumTracksAll_duplicatePv", Form("numContrib vs. numTracksAll %s", strBDTcut.c_str()), 150, 0, 150, 400, 0, 400)};
    TCanvas* cannumContribvnumTracksAll = plot2D(tree, "fNumContrib", "fNumTracksAll", histos2D_numContribvsnumTracksAll, cuts, classes, "numContrib vs numTracksAll", "", strCaseAppl);
    saveCanvas(cannumContribvnumTracksAll, "2D_numContrib_vs_numTracksAll");

    // numContrib vs. numTracksFiltered
    std::vector<TH2D*> histos2D_numContribvsnumTracksFiltered = {new TH2D("numContribvsnumTracksFiltered_singlePv", Form("numContrib vs. numTracksFiltered %s", strBDTcut.c_str()), 150, 0, 150, 100, 0, 100), new TH2D("numContribvsnumTracksFiltered_duplicatePv", Form("numContrib vs. numTracksFiltered %s", strBDTcut.c_str()), 150, 0, 150, 100, 0, 100)};
    TCanvas* cannumContribvnumTracksFiltered = plot2D(tree, "fNumContrib", "fNumTracksFiltered", histos2D_numContribvsnumTracksFiltered, cuts, classes, "numContrib vs numTracksFiltered", "", strCaseAppl);
    saveCanvas(cannumContribvnumTracksFiltered, "2D_numContrib_vs_numTracksAll");

    // numTracksAll vs. numTracksFiltered
    std::vector<TH2D*> histos2D_numTracksAllvsnumTracksFiltered = {new TH2D("numTracksAllvsnumTracksFiltered_singlePv", Form("numTracksAll vs. numTracksFiltered %s", strBDTcut.c_str()), 400, 0, 400, 100, 0, 100), new TH2D("numTracksAllvsnumTracksFiltered_duplicatePv", Form("numTracksAll vs. numTracksFiltered %s", strBDTcut.c_str()), 400, 0, 400, 100, 0, 100)};
    TCanvas* cannumTracksAllvnumTracksFiltered = plot2D(tree, "fNumTracksAll", "fNumTracksFiltered", histos2D_numTracksAllvsnumTracksFiltered, cuts, classes, "numTracksAll vs numTracksFiltered", "", strCaseAppl);
    saveCanvas(cannumTracksAllvnumTracksFiltered, "2D_numTracksAll_vs_numTracksAll");
    
    // numContrib vs. chi2
    std::vector<TH2D*> histos2D_numContribvsChi2 = {new TH2D("numContribvsChi2_singlePv", Form("numContrib vs. Chi2 %s", strBDTcut.c_str()), 150, 0, 150, 400, 0, 400), new TH2D("numContribvsChi2_duplicatePv", Form("numContrib vs. Chi2 %s", strBDTcut.c_str()), 150, 0, 150, 400, 0, 400)};
    TCanvas* cannumContribvChi2 = plot2D(tree, "fNumContrib", "fChi2", histos2D_numContribvsChi2, cuts, classes, "numContrib vs Chi2", "", strCaseAppl);
    saveCanvas(cannumContribvChi2, "2D_numContrib_vs_Chi2");

    // numContrib vs. SignalFT0A
    std::vector<TH2D*> histos2D_numContribvsSignalFT0A = {new TH2D("numContribvsSignalFT0A_singlePv", Form("numContrib vs. SignalFT0A %s", strBDTcut.c_str()), 150, 0, 150, 500, 0, 5000), new TH2D("numContribvsSignalFT0A_duplicatePv", Form("numContrib vs. SignalFT0A %s", strBDTcut.c_str()), 150, 0, 150, 500, 0, 5000)};
    TCanvas* cannumContribvSignalFT0A = plot2D(tree, "fNumContrib", "fSignalFT0A", histos2D_numContribvsSignalFT0A, cuts, classes, "numContrib vs SignalFT0A", "", strCaseAppl);
    saveCanvas(cannumContribvSignalFT0A, "2D_numContrib_vs_SignalFT0A");

    // numContrib vs. SignalFT0C
    std::vector<TH2D*> histos2D_numContribvsSignalFT0C = {new TH2D("numContribvsSignalFT0C_singlePv", Form("numContrib vs. SignalFT0C %s", strBDTcut.c_str()), 150, 0, 150, 150, 0, 1500), new TH2D("numContribvsSignalFT0C_duplicatePv", Form("numContrib vs. SignalFT0C %s", strBDTcut.c_str()), 150, 0, 150, 150, 0, 1500)};
    TCanvas* cannumContribvSignalFT0C = plot2D(tree, "fNumContrib", "fSignalFT0C", histos2D_numContribvsSignalFT0C, cuts, classes, "numContrib vs SignalFT0C", "", strCaseAppl);
    saveCanvas(cannumContribvSignalFT0C, "2D_numContrib_vs_SignalFT0C");

    // numContrib vs. SignalFT0M
    std::vector<TH2D*> histos2D_numContribvsSignalFT0M = {new TH2D("numContribvsSignalFT0M_singlePv", Form("numContrib vs. SignalFT0M %s", strBDTcut.c_str()), 150, 0, 150, 500, 0, 5000), new TH2D("numContribvsSignalFT0M_duplicatePv", Form("numContrib vs. SignalFT0M %s", strBDTcut.c_str()), 150, 0, 150, 500, 0, 5000)};
    TCanvas* cannumContribvSignalFT0M = plot2D(tree, "fNumContrib", "fSignalFT0M", histos2D_numContribvsSignalFT0M, cuts, classes, "numContrib vs SignalFT0M", "", strCaseAppl);
    saveCanvas(cannumContribvSignalFT0M, "2D_numContrib_vs_SignalFT0M");

    // numContrib vs. SignalV0A
    std::vector<TH2D*> histos2D_numContribvsSignalV0A = {new TH2D("numContribvsSignalV0A_singlePv", Form("numContrib vs. SignalV0A %s", strBDTcut.c_str()), 150, 0, 150, 800, 0, 8000), new TH2D("numContribvsSignalV0A_duplicatePv", Form("numContrib vs. SignalV0A %s", strBDTcut.c_str()), 150, 0, 150, 800, 0, 8000)};
    TCanvas* cannumContribvSignalV0A = plot2D(tree, "fNumContrib", "fSignalV0A", histos2D_numContribvsSignalV0A, cuts, classes, "numContrib vs SignalV0A", "", strCaseAppl);
    saveCanvas(cannumContribvSignalV0A, "2D_numContrib_vs_SignalV0A");

    // SignalFT0C vs. SignalFT0A
    std::vector<TH2D*> histos2D_SignalFT0CvsSignalFT0A = {new TH2D("SignalFT0CvsSignalFT0A_singlePv", Form("SignalFT0C vs. SignalFT0A %s", strBDTcut.c_str()), 150, 0, 1500, 500, 0, 5000), new TH2D("SignalFT0CvsSignalFT0A_duplicatePv", Form("SignalFT0C vs. SignalFT0A %s", strBDTcut.c_str()), 150, 0, 1500, 500, 0, 5000)};
    TCanvas* canSignalFT0CvSignalFT0A = plot2D(tree, "fSignalFT0C", "fSignalFT0A", histos2D_SignalFT0CvsSignalFT0A, cuts, classes, "SignalFT0C vs SignalFT0A", "", strCaseAppl);
    saveCanvas(canSignalFT0CvSignalFT0A, "2D_SignalFT0C_vs_SignalFT0A");

    // SignalFT0A vs. SignalFT0M
    std::vector<TH2D*> histos2D_SignalFT0AvsSignalFT0M = {new TH2D("SignalFT0AvsSignalFT0M_singlePv", Form("SignalFT0A vs. SignalFT0M %s", strBDTcut.c_str()), 150, 0, 1500, 500, 0, 5000), new TH2D("SignalFT0AvsSignalFT0M_duplicatePv", Form("SignalFT0A vs. SignalFT0M %s", strBDTcut.c_str()), 150, 0, 1500, 500, 0, 5000)};
    TCanvas* canSignalFT0AvSignalFT0M = plot2D(tree, "fSignalFT0A", "fSignalFT0M", histos2D_SignalFT0AvsSignalFT0M, cuts, classes, "SignalFT0A vs SignalFT0M", "", strCaseAppl);
    saveCanvas(canSignalFT0AvSignalFT0M, "2D_SignalFT0A_vs_SignalFT0M");
    
    // SignalFT0C vs. SignalFT0M
    std::vector<TH2D*> histos2D_SignalFT0CvsSignalFT0M = {new TH2D("SignalFT0CvsSignalFT0M_singlePv", Form("SignalFT0C vs. SignalFT0M %s", strBDTcut.c_str()), 150, 0, 1500, 500, 0, 5000), new TH2D("SignalFT0CvsSignalFT0M_duplicatePv", Form("SignalFT0C vs. SignalFT0M %s", strBDTcut.c_str()), 150, 0, 1500, 500, 0, 5000)};
    TCanvas* canSignalFT0CvSignalFT0M = plot2D(tree, "fSignalFT0C", "fSignalFT0M", histos2D_SignalFT0CvsSignalFT0M, cuts, classes, "SignalFT0C vs SignalFT0M", "", strCaseAppl);
    saveCanvas(canSignalFT0CvSignalFT0M, "2D_SignalFT0C_vs_SignalFT0M");

    // SignalV0A vs. SignalFT0A
    std::vector<TH2D*> histos2D_SignalV0AvsSignalFT0A = {new TH2D("SignalV0AvsSignalFT0A_singlePv", Form("SignalV0A vs. SignalFT0A %s", strBDTcut.c_str()), 800, 0, 8000, 500, 0, 5000), new TH2D("SignalV0AvsSignalFT0A_duplicatePv", Form("SignalV0A vs. SignalFT0A %s", strBDTcut.c_str()), 800, 0, 8000, 500, 0, 5000)};
    TCanvas* canSignalV0AvSignalFT0A = plot2D(tree, "fSignalV0A", "fSignalFT0A", histos2D_SignalV0AvsSignalFT0A, cuts, classes, "SignalV0A vs SignalFT0A", "", strCaseAppl);
    saveCanvas(canSignalV0AvSignalFT0A, "2D_SignalV0A_vs_SignalFT0A");

    // SignalV0A vs. SignalFT0C
    std::vector<TH2D*> histos2D_SignalV0AvsSignalFT0C = {new TH2D("SignalV0AvsSignalFT0C_singlePv", Form("SignalV0A vs. SignalFT0C %s", strBDTcut.c_str()), 800, 0, 8000, 150, 0, 1500), new TH2D("SignalV0AvsSignalFT0C_duplicatePv", Form("SignalV0A vs. SignalFT0C %s", strBDTcut.c_str()), 800, 0, 8000, 150, 0, 1500)};
    TCanvas* canSignalV0AvSignalFT0C = plot2D(tree, "fSignalV0A", "fSignalFT0C", histos2D_SignalV0AvsSignalFT0C, cuts, classes, "SignalV0A vs SignalFT0C", "", strCaseAppl);
    saveCanvas(canSignalV0AvSignalFT0C, "2D_SignalV0A_vs_SignalFT0C");

    // SignalV0A vs. SignalFT0M
    std::vector<TH2D*> histos2D_SignalV0AvsSignalFT0M = {new TH2D("SignalV0AvsSignalFT0M_singlePv", Form("SignalV0A vs. SignalFT0M %s", strBDTcut.c_str()), 800, 0, 8000, 500, 0, 5000), new TH2D("SignalV0AvsSignalFT0M_duplicatePv", Form("SignalV0A vs. SignalFT0M %s", strBDTcut.c_str()), 800, 0, 8000, 500, 0, 5000)};
    TCanvas* canSignalV0AvSignalFT0M = plot2D(tree, "fSignalV0A", "fSignalFT0M", histos2D_SignalV0AvsSignalFT0M, cuts, classes, "SignalV0A vs SignalFT0M", "", strCaseAppl);
    saveCanvas(canSignalV0AvSignalFT0M, "2D_SignalV0A_vs_SignalFT0M");

    // CollisionTimeRes vs. numTracksAll
    std::vector<TH2D*> histos2D_CollisionTimeResvsnumTracksAll = {new TH2D("CollisionTimeResvsnumTracksAll_singlePv", Form("CollisionTimeRes vs. numTracksAll %s", strBDTcut.c_str()), 200, 0, 2000, 400, 0, 400), new TH2D("CollisionTimeResvsnumTracksAll_duplicatePv", Form("CollisionTimeRes vs. numTracksAll %s", strBDTcut.c_str()), 200, 0, 2000, 400, 0, 400)};
    TCanvas* canCollisionTimeResvnumTracksAll = plot2D(tree, "fCollisionTimeRes", "fNumTracksAll", histos2D_CollisionTimeResvsnumTracksAll, cuts, classes, "CollisionTimeRes vs numTracksAll", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvnumTracksAll, "2D_CollisionTimeRes_vs_numTracksAll");

    // CollisionTimeRes vs. numTracksFiltered
    std::vector<TH2D*> histos2D_CollisionTimeResvsnumTracksFiltered = {new TH2D("CollisionTimeResvsnumTracksFiltered_singlePv", Form("CollisionTimeRes vs. numTracksFiltered %s", strBDTcut.c_str()), 200, 0, 2000, 150, 0, 150), new TH2D("CollisionTimeResvsnumTracksFiltered_duplicatePv", Form("CollisionTimeRes vs. numTracksFiltered %s", strBDTcut.c_str()), 200, 0, 2000, 150, 0, 150)};
    TCanvas* canCollisionTimeResvnumTracksFiltered = plot2D(tree, "fCollisionTimeRes", "fNumTracksFiltered", histos2D_CollisionTimeResvsnumTracksFiltered, cuts, classes, "CollisionTimeRes vs numTracksFiltered", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvnumTracksFiltered, "2D_CollisionTimeRes_vs_numTracksFiltered");

    // CollisionTimeRes vs. numContrib
    std::vector<TH2D*> histos2D_CollisionTimeResvsnumContrib = {new TH2D("CollisionTimeResvsnumContrib_singlePv", Form("CollisionTimeRes vs. numContrib %s", strBDTcut.c_str()), 200, 0, 2000, 150, 0, 150), new TH2D("CollisionTimeResvsnumContrib_duplicatePv", Form("CollisionTimeRes vs. numContrib %s", strBDTcut.c_str()), 200, 0, 2000, 150, 0, 150)};
    TCanvas* canCollisionTimeResvnumContrib = plot2D(tree, "fCollisionTimeRes", "fNumContrib", histos2D_CollisionTimeResvsnumContrib, cuts, classes, "CollisionTimeRes vs numContrib", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvnumContrib, "2D_CollisionTimeRes_vs_numContrib");
    
    // CollisionTimeRes vs. Chi2
    std::vector<TH2D*> histos2D_CollisionTimeResvsChi2 = {new TH2D("CollisionTimeResvsChi2_singlePv", Form("CollisionTimeRes vs. Chi2 %s", strBDTcut.c_str()), 200, 0, 2000, 400, 0, 400), new TH2D("CollisionTimeResvsChi2_duplicatePv", Form("CollisionTimeRes vs. Chi2 %s", strBDTcut.c_str()), 200, 0, 2000, 400, 0, 400)};
    TCanvas* canCollisionTimeResvChi2 = plot2D(tree, "fCollisionTimeRes", "fChi2", histos2D_CollisionTimeResvsChi2, cuts, classes, "CollisionTimeRes vs Chi2", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvChi2, "2D_CollisionTimeRes_vs_Chi2");

    // CollisionTimeRes vs. Ft0PosZ
    std::vector<TH2D*> histos2D_CollisionTimeResvsFt0PosZ = {new TH2D("CollisionTimeResvsFt0PosZ_singlePv", Form("CollisionTimeRes vs. Ft0PosZ %s", strBDTcut.c_str()), 200, 0, 2000, 10, -1000, 1000), new TH2D("CollisionTimeResvsFt0PosZ_duplicatePv", Form("CollisionTimeRes vs. Ft0PosZ %s", strBDTcut.c_str()), 200, 0, 2000, 10, -1000, 1000)};
    TCanvas* canCollisionTimeResvFt0PosZ = plot2D(tree, "fCollisionTimeRes", "fFt0PosZ", histos2D_CollisionTimeResvsFt0PosZ, cuts, classes, "CollisionTimeRes vs Ft0PosZ", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvFt0PosZ, "2D_CollisionTimeRes_vs_Ft0PosZ");

    // CollisionTimeRes vs. Ft0PosZ (no bad values)
    std::vector<TH2D*> histos2D_CollisionTimeResvsFt0PosZ_central = {new TH2D("CollisionTimeResvsFt0PosZ_central_singlePv", Form("CollisionTimeRes vs. Ft0PosZ (no bad values) %s", strBDTcut.c_str()), 200, 0, 2000, 80, -20, 20), new TH2D("CollisionTimeResvsFt0PosZ_central_duplicatePv", Form("CollisionTimeRes vs. Ft0PosZ (no bad values) %s", strBDTcut.c_str()), 200, 0, 2000, 80, -20, 20)};
    TCanvas* canCollisionTimeResvFt0PosZ_central = plot2D(tree, "fCollisionTimeRes", "fFt0PosZ", histos2D_CollisionTimeResvsFt0PosZ_central, cuts, classes, "CollisionTimeRes vs Ft0PosZ (no bad values)", "_central", strCaseAppl);
    saveCanvas(canCollisionTimeResvFt0PosZ_central, "2D_CollisionTimeRes_vs_Ft0PosZ_central");

    // CollisionTimeRes vs. SignalFT0A
    std::vector<TH2D*> histos2D_CollisionTimeResvsSignalFT0A = {new TH2D("CollisionTimeResvsSignalFT0A_singlePv", Form("CollisionTimeRes vs. SignalFT0A %s", strBDTcut.c_str()), 200, 0, 2000, 500, 0, 5000), new TH2D("CollisionTimeResvsSignalFT0A_duplicatePv", Form("CollisionTimeRes vs. SignalFT0A %s", strBDTcut.c_str()), 200, 0, 2000, 500, 0, 5000)};
    TCanvas* canCollisionTimeResvSignalFT0A = plot2D(tree, "fCollisionTimeRes", "fSignalFT0A", histos2D_CollisionTimeResvsSignalFT0A, cuts, classes, "CollisionTimeRes vs SignalFT0A", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvSignalFT0A, "2D_CollisionTimeRes_vs_SignalFT0A");

    // CollisionTimeRes vs. SignalFT0C
    std::vector<TH2D*> histos2D_CollisionTimeResvsSignalFT0C = {new TH2D("CollisionTimeResvsSignalFT0C_singlePv", Form("CollisionTimeRes vs. SignalFT0C %s", strBDTcut.c_str()), 200, 0, 2000, 150, 0, 1500), new TH2D("CollisionTimeResvsSignalFT0C_duplicatePv", Form("CollisionTimeRes vs. SignalFT0C %s", strBDTcut.c_str()), 200, 0, 2000, 150, 0, 1500)};
    TCanvas* canCollisionTimeResvSignalFT0C = plot2D(tree, "fCollisionTimeRes", "fSignalFT0C", histos2D_CollisionTimeResvsSignalFT0C, cuts, classes, "CollisionTimeRes vs SignalFT0C", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvSignalFT0C, "2D_CollisionTimeRes_vs_SignalFT0C");

    // CollisionTimeRes vs. SignalFT0M
    std::vector<TH2D*> histos2D_CollisionTimeResvsSignalFT0M = {new TH2D("CollisionTimeResvsSignalFT0M_singlePv", Form("CollisionTimeRes vs. SignalFT0M %s", strBDTcut.c_str()), 200, 0, 2000, 500, 0, 5000), new TH2D("CollisionTimeResvsSignalFT0M_duplicatePv", Form("CollisionTimeRes vs. SignalFT0M %s", strBDTcut.c_str()), 200, 0, 2000, 500, 0, 5000)};
    TCanvas* canCollisionTimeResvSignalFT0M = plot2D(tree, "fCollisionTimeRes", "fSignalFT0M", histos2D_CollisionTimeResvsSignalFT0M, cuts, classes, "CollisionTimeRes vs SignalFT0M", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvSignalFT0M, "2D_CollisionTimeRes_vs_SignalFT0M");

    // CollisionTimeRes vs. SignalV0A
    std::vector<TH2D*> histos2D_CollisionTimeResvsSignalV0A = {new TH2D("CollisionTimeResvsSignalV0A_singlePv", Form("CollisionTimeRes vs. SignalV0A %s", strBDTcut.c_str()), 200, 0, 2000, 800, 0, 8000), new TH2D("CollisionTimeResvsSignalV0A_duplicatePv", Form("CollisionTimeRes vs. SignalV0A %s", strBDTcut.c_str()), 200, 0, 2000, 800, 0, 8000)};
    TCanvas* canCollisionTimeResvSignalV0A = plot2D(tree, "fCollisionTimeRes", "fSignalV0A", histos2D_CollisionTimeResvsSignalV0A, cuts, classes, "CollisionTimeRes vs SignalV0A", "", strCaseAppl);
    saveCanvas(canCollisionTimeResvSignalV0A, "2D_CollisionTimeRes_vs_SignalV0A");

    /// TODO: other 2D correlation plots?
    /// [...]

    return;
}

////////////////////
////////////////////
/// 1D projections from TTree
TCanvas* plot1D(TTree* tree, std::string name, std::vector<std::string> vCuts, std::vector<std::string> vClasses, std::vector<TH1D*>& vHistos, 
                std::string strMyCase, std::string strCanvasSuff, std::string strCaseAppl, bool normalize, bool logy) {

    if ( vCuts.size() != vClasses.size() || vClasses.size() != vHistos.size() ) {
        std::cout << "[plot1D]: input vectors with different sizes. Skipping the case " << name << "." << std::endl;
        return nullptr;
    }

    /// get histograms for each class
    for(int i=0; i<vClasses.size(); i++) {
        const std::string cut = vCuts[i];
        tree->Draw(Form("%s>>%s", name.c_str(), vHistos[i]->GetName()), cut.c_str(), "goff");
    }

    /// draw
    TCanvas* can = new TCanvas(Form("can_%s%s", name.c_str(), strCanvasSuff.c_str()), strMyCase.c_str(), 800, 650);
    TLegend* leg = new TLegend(0.65, 0.7, 0.9, 0.85);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetHeader(strCaseAppl.c_str());
    for(int i=0; i<vHistos.size(); i++) {
        can->cd();
        vHistos[i]->SetMarkerStyle(markers[i]);
        vHistos[i]->SetMarkerColor(colors[i]);
        vHistos[i]->SetMarkerSize(1);
        vHistos[i]->SetLineColor(colors[i]);
        vHistos[i]->SetFillColorAlpha(colors[i], 0.25);
        vHistos[i]->SetStats(kFALSE);
        vHistos[i]->Draw(i==0?"histo":"histosame");
        vHistos[i]->Draw("pesame");
        vHistos[i]->GetXaxis()->SetTitle(name.c_str());
        if(normalize){
            vHistos[i]->GetYaxis()->SetTitle("normalized counts");
            vHistos[i]->Scale(1./vHistos[i]->Integral());
            if(logy) vHistos[i]->GetYaxis()->SetRangeUser(1e-5, 1);
        } else {
            vHistos[i]->GetYaxis()->SetTitle("counts");
        }

        const std::string myclass = vClasses[i];
        leg->AddEntry(vHistos[i], myclass.c_str(), "pf");
    }
    if(logy) gPad->SetLogy();
    gPad->SetTicks();
    leg->Draw();
    gPad->SetLeftMargin(0.15);

    return can;
}
////////////////////
////////////////////
/// Produce efficiency, rejection, precision, and recall curves
TCanvas* plotEffRejPrecisionRecall(std::vector<std::string> vClasses, std::vector<int> vPositive, std::vector<TH1D*>& vHistos,
                                   std::string name, std::string strCanvasSuff, std::string strMyCase, std::string strCaseAppl) {

    if ( vClasses.size() != vHistos.size() ) {
        std::cout << "[plotEffRejPrecisionRecall]: input vectors with different sizes. Return." << std::endl;
        return nullptr;
    }

    /// final histograms
    std::vector<TH1D*> vHistosEff{}, vHistosRej{};
    for(auto& h : vHistos) {
        vHistosEff.emplace_back( dynamic_cast<TH1D*>(h->Clone(Form("%s_eff", h->GetName()))) );
        vHistosRej.emplace_back( dynamic_cast<TH1D*>(h->Clone(Form("%s_rej", h->GetName()))) );
        vHistosEff.back()->SetTitle(strMyCase.c_str());
        vHistosRej.back()->SetTitle(strMyCase.c_str());
    }

    /// total integral
    std::vector<double> integral = {};
    for(auto& h : vHistos)  integral.push_back( h->Integral() );

    /// fill histograms
    /// loop over the bins, to calculate the variables
    for(int bin=1; bin<=vHistos[0]->GetNbinsX(); bin++) { 

        const double center = vHistos[0]->GetBinCenter(bin);
        if(center < 0 || center > 1)    continue;

        /// loop over the input histograms, to calculate the variables for all categories
        for(int iHisto=0; iHisto<vClasses.size(); iHisto++) {
            const double aboveCut = vHistos[iHisto]->Integral(bin, vHistos[iHisto]->GetNbinsX());
            const double belowCut = integral[iHisto] - aboveCut;

            const double efficiency = aboveCut / integral[iHisto];
            const double rejection = belowCut / integral[iHisto];
            vHistosEff[iHisto]->SetBinContent(bin, efficiency);
            vHistosRej[iHisto]->SetBinContent(bin, rejection);
            vHistosEff[iHisto]->SetBinError(bin, 0);
            vHistosRej[iHisto]->SetBinError(bin, 0);
        }
    }

    /// plot
    TCanvas* can = new TCanvas(Form("can_%s%s_effRejPrecRecall", name.c_str(), strCanvasSuff.c_str()), strMyCase.c_str(), 800, 650);
    TLegend* leg = new TLegend(0.2, 0.15, 0.9, 0.30);
    leg->SetNColumns(2);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetHeader(strCaseAppl.c_str());
    
    for(int i=0; i<vHistosEff.size(); i++) {
        can->cd();

        /// efficiency
        vHistosEff[i]->SetFillStyle(0);
        vHistosEff[i]->SetLineColor(colors[i]);
        vHistosEff[i]->SetLineStyle(1);
        vHistosEff[i]->SetLineWidth(2);
        vHistosEff[i]->SetStats(kFALSE);
        //vHistosEff[i]->SetTitle("");
        vHistosEff[i]->Draw(i==0?"l":"lsame");
        if(i==0)    vHistosEff[i]->GetYaxis()->SetRangeUser(0, 1.1);
        vHistosEff[i]->GetXaxis()->SetTitle(name.c_str());
        vHistosEff[i]->GetYaxis()->SetTitle("");

        /// rejection
        vHistosRej[i]->SetFillStyle(0);
        vHistosRej[i]->SetLineColor(colors[i]);
        vHistosRej[i]->SetLineStyle(7);
        vHistosRej[i]->SetLineWidth(2);
        vHistosRej[i]->SetStats(kFALSE);
        //vHistosRej[i]->SetTitle("");
        vHistosRej[i]->Draw("lsame");
        if(i==0)    vHistosRej[i]->GetYaxis()->SetRangeUser(0, 1.1);
        vHistosRej[i]->GetXaxis()->SetTitle(name.c_str());
        vHistosRej[i]->GetYaxis()->SetTitle("");

        const std::string myclass = vClasses[i];
        leg->AddEntry(vHistosEff[i], Form("%s efficiency", myclass.c_str()), "l");
        leg->AddEntry(vHistosRej[i], Form("%s rejection", myclass.c_str()), "l");
    }
    gPad->SetTicks();
    leg->Draw();

    /// precision and recall
    if(vPositive.size()!=2) {
        std::cout << "[plotEffRejPrecisionRecall]: precision and recall curves not possible for a number of classes different than 2. Skipping it." << std::endl;
        return can;
    }
    if(std::accumulate(vPositive.begin(), vPositive.end(), 0) != 1) {
        std::cout << "[plotEffRejPrecisionRecall]: precision and recall curves not possible if none or both classes are flagged as positive/negative. Skipping it." << std::endl;
        return can;
    }

    int iPositive = -1;
    int iNegative = -1;
    if(vPositive[0] == 1 && vPositive[1] == 0) {
        iPositive = 0;
        iNegative = 1;
    } else {
        iPositive = 1;
        iNegative = 0;
    }
    std::cout << "[plotEffRejPrecisionRecall]: positive class " << iPositive << ", negative class " << iNegative << std::endl;

    TH1D* hPrecision = dynamic_cast<TH1D*>(vHistosEff[0]->Clone(Form("precision_%s%s", name.c_str(), strCanvasSuff.c_str())));
    TH1D* hRecall = dynamic_cast<TH1D*>(vHistosEff[0]->Clone(Form("recall_%s%s", name.c_str(), strCanvasSuff.c_str())));
    for(int bin=1; bin<=vHistos[0]->GetNbinsX(); bin++) {

        const double center = vHistos[0]->GetBinCenter(bin);
        if(center < 0 || center > 1)    continue;

        const double true_positive = vHistos[iPositive]->Integral(bin, vHistos[iPositive]->GetNbinsX());
        const double false_negative = vHistos[iPositive]->Integral() - true_positive; 
        const double false_positive = vHistos[iNegative]->Integral(bin, vHistos[iNegative]->GetNbinsX());
        const double true_negative = vHistos[iNegative]->Integral() - false_positive;

        const double precision = true_positive/(true_positive + false_positive); /// it's a purity: Tp/(Tp+Fp)
        const double recall = true_positive/(true_positive + false_negative); /// it's an efficiency: Tp/(Tp+Fn)
        //std::cout << ">>> true_positive bin " << bin << " = " << true_positive << std::endl;
        //std::cout << "    false_positive bin " << bin << " = " << false_positive << std::endl;
        //std::cout << "    precision bin " << bin << " = " << precision << std::endl;
        //std::cout << "    center bin " << bin << " = " << vHistos[iPositive]->GetBinCenter(bin) << std::endl;
        if(isnan(precision) || isnan(recall)) continue;
        hPrecision->SetBinContent(bin, precision);
        hRecall->SetBinContent(bin, recall); 
    }
    hPrecision->SetLineColor(kGreen+1);
    hPrecision->SetLineStyle(1);
    hPrecision->SetLineWidth(2);
    hPrecision->Draw("lsame");
    hRecall->SetLineColor(kGreen+1);
    hRecall->SetLineStyle(7);
    hRecall->SetLineWidth(2);
    hRecall->Draw("lsame");
    leg->AddEntry(hPrecision, "Precision: Tp/(Tp+Fp)", "l");
    leg->AddEntry(hRecall, "Recall: Tp/(Tp+Fn)", "l");

    return can;
}
////////////////////
////////////////////
/// 2D projections from TTree
TCanvas* plot2D(TTree* tree, std::string name1, std::string name2, std::vector<TH2D*> vHistos, std::vector<std::string> vCuts, std::vector<std::string> vClasses,
                std::string strMyCase, std::string strCanvasSuff, std::string strCaseAppl)
{
    if ( vCuts.size() != vClasses.size()) {
        std::cout << "[plot1D]: input vectors with different sizes. Skipping the case." << std::endl;
        return nullptr;
    }

    gStyle->SetPalette(kRainBow);

    /// get histograms for each class
    for(int i=0; i<vClasses.size(); i++) {
        const std::string cut = vCuts[i];
        const std::string myClass = vClasses[i];
        tree->Draw(Form("%s:%s>>%s", name2.c_str(), name1.c_str(), vHistos[i]->GetName()), cut.c_str(), "goff");
    }

    /// draw
    TCanvas* can = new TCanvas(Form("can_%s%s%s", name1.c_str(), name2.c_str(), strCanvasSuff.c_str()), strMyCase.c_str(), 1200, 600);
    int rows = vHistos.size()==2? 1 : 2;
    int cols = 2;
    can->Divide(cols, rows);
    for(int i=0; i<vHistos.size(); i++) {
        can->cd(i+1);
        vHistos[i]->SetTitle(Form("%s - %s",vClasses[i].c_str(), strCaseAppl.c_str()));
        vHistos[i]->Draw("colz");
        vHistos[i]->Scale(1./vHistos[i]->Integral());
        vHistos[i]->GetXaxis()->SetTitle(name1.c_str());
        vHistos[i]->GetYaxis()->SetTitle(name2.c_str());
        gPad->SetLeftMargin(0.15);
        gPad->SetRightMargin(0.125);
        gPad->SetLogz();
    }

    return can;
}