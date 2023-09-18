#include "headerFilterTree.h"

void filterTree(std::string inputsNames, std::string myCase, bool isMC) {

    // take the time
    std::cout << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::time_t start_time = std::chrono::high_resolution_clock::to_time_t(start);

    /// getting out the files
    std::fstream inputFiles;
    inputFiles.open(inputsNames.c_str(), std::ios::in);
    if(!inputFiles.is_open()) {
        std::cout << "ERROR: file " << inputsNames << " not open!" << std::endl;
        return;
    }
    std::vector<std::string> files = {};
    std::string s;
    while (getline(inputFiles, s)) {
        files.push_back(s);
    }
    std::cout << ">>> analysed files:" << std::endl;
    for(auto file : files) {
        std::cout << "\t" << file << std::endl;
    }
    std::cout << std::endl;

    // final output file
    TFile fout(Form("tree%s.root", myCase.c_str()),"recreate");

    // new TTree to be filled and finally saved
    TTree finalTree("dpgCollsBigML", "collision table for ML");
    int isEvSel, runNumber, numContrib, numTracksAll, numTracksFiltered, counterCollision,
        counterDF, collIdMC, isFakeColl, recoPvPerMcColl, isPvHighestContribForMcColl;
    ULong64_t globalBcInRun;
    float posX, posY, posZ, covXX, covXY, covXZ, covYY, covYZ, covZZ, chi2PV,
          ft0posZ, signalFT0A, signalFT0C, signalFT0M, signalV0A,
          collTime, collTimeRes, posXMC, posYMC, posZMC, collTimeMC;
    int isDuplicate;
    finalTree.Branch("fIsEventSelected", &isEvSel, "fIsEventSelected/I");
    finalTree.Branch("fRunNumber", &runNumber, "fRunNumber/I");
    finalTree.Branch("fPosX", &posX, "fPosX/F");
    finalTree.Branch("fPosY", &posY, "fPosY/F");
    finalTree.Branch("fPosZ", &posZ, "fPosZ/F");
    finalTree.Branch("fCovXX", &covXX, "fCovXX/F");
    finalTree.Branch("fCovXY", &covXY, "fCovXY/F");
    finalTree.Branch("fCovXZ", &covXZ, "fCovXZ/F");
    finalTree.Branch("fCovYY", &covYY, "fCovYY/F");
    finalTree.Branch("fCovYZ", &covYZ, "fCovYZ/F");
    finalTree.Branch("fCovZZ", &covZZ, "fCovZZ/F");
    finalTree.Branch("fNumContrib", &numContrib, "fNumContrib/I");
    finalTree.Branch("fNumTracksAll", &numTracksAll, "fNumTracksAll/I");
    finalTree.Branch("fNumTracksFiltered", &numTracksFiltered, "fNumTracksFiltered/I");
    finalTree.Branch("fChi2", &chi2PV, "fChi2/F");
    finalTree.Branch("fGlobalBcInRun", &globalBcInRun, "fGlobalBcInRun/l");
    finalTree.Branch("fFt0PosZ", &ft0posZ, "fFt0PosZ/F");
    finalTree.Branch("fSignalFT0A", &signalFT0A, "fSignalFT0A/F");
    finalTree.Branch("fSignalFT0C", &signalFT0C, "fSignalFT0C/F");
    finalTree.Branch("fSignalFT0M", &signalFT0M, "fSignalFT0M/F");
    finalTree.Branch("fSignalV0A", &signalV0A, "fSignalV0A/F");
    finalTree.Branch("fCollisionTime", &collTime, "fCollisionTime/F");
    finalTree.Branch("fCollisionTimeRes", &collTimeRes, "fCollisionTimeRes/F");
    //finalTree.Branch("fDpgCounterCollision", &counterCollision, "fDpgCounterCollision/I");
    //finalTree.Branch("fDpgCounterDF", &counterDF, "fDpgCounterDF/I");
    finalTree.Branch("fCollIDMC", &collIdMC, "fCollIDMC/I");
    finalTree.Branch("fPosXMC", &posXMC, "fPosXMC/F");
    finalTree.Branch("fPosYMC", &posYMC, "fPosYMC/F");
    finalTree.Branch("fPosZMC", &posZMC, "fPosZMC/F");
    finalTree.Branch("fCollisionTimeMC", &collTimeMC, "fCollisionTimeMC/F");
    finalTree.Branch("fIsFakeCollision", &isFakeColl, "fIsFakeCollision/I");
    finalTree.Branch("fRecoPVsPerMcColl", &recoPvPerMcColl, "fRecoPVsPerMcColl/I");
    finalTree.Branch("fIsPvHighestContribForMcColl", &isPvHighestContribForMcColl, "fIsPvHighestContribForMcColl/I");
    finalTree.Branch("fIsDuplicate", &isDuplicate, "fIsDuplicate/I");
    
    /// looping over all the files, and fill the ML tree as desired
    for(int i=0; i<static_cast<int>(files.size()); i++) {
        const std::string filename(files.at(i));
        std::cout << "  >>> working on file " << filename << std::endl;
        TFile file(filename.c_str(), "READ");

        /// get the list of DFs in the current file
        TList* listDF = dynamic_cast<TList*>(file.GetListOfKeys());
        //std::unique_ptr<TList> listDF(dynamic_cast<TList*>(file.GetListOfKeys()));
        listDF->Print();
        
        /// looping over directories == DFs
        for(int iDF=0; iDF < listDF->GetEntries(); iDF++) {
            // directory (DF)
            std::string strDir(listDF->At(iDF)->GetName());
            std::unique_ptr<TDirectoryFile> dir {(TDirectoryFile*) file.Get(strDir.c_str()) };
            if(strDir.substr(0,3).compare("DF_")) {
                std::cout << "\tdirectory " << strDir << " skipped" << std::endl;
                continue;
            }
            //std::cout << "\t\tdirectory " << dir->GetName() << std::endl;

            ///
            /// !!! TEMPORARY !!!
            ///
            if(strDir.compare("DF_2234495402005329")) {
                std::cout << "\tdirectory " << strDir << " skipped (!!! TEMPORARY !!!)" << std::endl;
                continue;
            }
            ///
            /// !!! TEMPORARY !!!
            ///

            /// get out the tree from the related DFs
            std::unique_ptr<TTree> inputTree { (TTree*) dir->Get("O2dpgcollsbig") };
            if(!inputTree.get()) {
                std::cout << "!!! TTree not found !!!" << std::endl;
                continue;
            }
            //std::cout << "\t\tTree " << inputTree.get() << std::endl;
            int isEvSel_input, runNumber_input, numContrib_input, numTracksAll_input, numTracksFiltered_input, counterCollision_input,
                counterDF_input, collIdMC_input, isFakeColl_input, recoPvPerMcColl_input, isPvHighestContribForMcColl_input;
            ULong64_t globalBcInRun_input;
            float posX_input, posY_input, posZ_input, covXX_input, covXY_input, covXZ_input, covYY_input, covYZ_input, covZZ_input, chi2PV_input,
                  ft0posZ_input, signalFT0A_input, signalFT0C_input, signalFT0M_input, signalV0A_input,
                  collTime_input, collTimeRes_input, posXMC_input, posYMC_input, posZMC_input, collTimeMC_input;
            inputTree->SetBranchAddress("fIsEventSelected", &isEvSel_input);
            inputTree->SetBranchAddress("fRunNumber", &runNumber_input);
            inputTree->SetBranchAddress("fPosX", &posX_input);
            inputTree->SetBranchAddress("fPosY", &posY_input);
            inputTree->SetBranchAddress("fPosZ", &posZ_input);
            inputTree->SetBranchAddress("fCovXX", &covXX_input);
            inputTree->SetBranchAddress("fCovXY", &covXY_input);
            inputTree->SetBranchAddress("fCovXZ", &covXZ_input);
            inputTree->SetBranchAddress("fCovYY", &covYY_input);
            inputTree->SetBranchAddress("fCovYZ", &covYZ_input);
            inputTree->SetBranchAddress("fCovZZ", &covZZ_input);
            inputTree->SetBranchAddress("fNumContrib", &numContrib_input);
            inputTree->SetBranchAddress("fNumTracksAll", &numTracksAll_input);
            inputTree->SetBranchAddress("fNumTracksFiltered", &numTracksFiltered_input);
            inputTree->SetBranchAddress("fChi2", &chi2PV_input);
            inputTree->SetBranchAddress("fGlobalBcInRun", &globalBcInRun_input);
            inputTree->SetBranchAddress("fFt0PosZ", &ft0posZ_input);
            inputTree->SetBranchAddress("fSignalFT0A", &signalFT0A_input);
            inputTree->SetBranchAddress("fSignalFT0C", &signalFT0C_input);
            inputTree->SetBranchAddress("fSignalFT0M", &signalFT0M_input);
            inputTree->SetBranchAddress("fSignalV0A", &signalV0A_input);
            inputTree->SetBranchAddress("fCollisionTime", &collTime_input);
            inputTree->SetBranchAddress("fCollisionTimeRes", &collTimeRes_input);
            inputTree->SetBranchAddress("fDpgCounterCollision", &counterCollision_input);
            inputTree->SetBranchAddress("fDpgCounterDF", &counterDF_input);
            inputTree->SetBranchAddress("fCollIDMC", &collIdMC_input);
            inputTree->SetBranchAddress("fPosXMC", &posXMC_input);
            inputTree->SetBranchAddress("fPosYMC", &posYMC_input);
            inputTree->SetBranchAddress("fPosZMC", &posZMC_input);
            inputTree->SetBranchAddress("fCollisionTimeMC", &collTimeMC_input);
            inputTree->SetBranchAddress("fIsFakeCollision", &isFakeColl_input);
            inputTree->SetBranchAddress("fRecoPVsPerMcColl", &recoPvPerMcColl_input);
            inputTree->SetBranchAddress("fIsPvHighestContribForMcColl", &isPvHighestContribForMcColl_input);

            /// buffer vectors needed to understand if a collision is duplicated or not
            std::vector<int> vecRunNumber = {};
            std::vector<ULong64_t> vecGlobBcInRun = {};
            std::vector<int> vecCollIdMC = {};
            for(Long64_t i=0; i < inputTree->GetEntries(); i++) {

                /// retrieve the values from the tree for the current entrys
                inputTree->GetEntry(i);

                /// store the values in the buffer vectors
                vecRunNumber.push_back(runNumber_input);
                vecGlobBcInRun.push_back(globalBcInRun_input);
                vecCollIdMC.push_back(collIdMC_input);
                /// actually no need to count the DF, since each directory corresponds to one DF
            }

            std::cout << "vecGlobBcInRun = ";
            for(int j=0; j < vecGlobBcInRun.size(); j++) {
                std::cout << vecGlobBcInRun.at(j) << " ";
                if(j==(vecGlobBcInRun.size()-1))    std::cout << std::endl;
            }

            /// loop over tree entries again, find out the duplicates and fill the output tree
            for(Long64_t i=0; i < inputTree->GetEntries(); i++) {

                /// retrieve the values from the tree for the current entrys
                inputTree->GetEntry(i);

                /// Determine if the current one is a duplicated collision
                /// Done checking if at least another collision has the same MC one within the same run
                /// By definition, this counts within a DF (we are looping over dirs == DFs)
                const int currentRun = runNumber_input;
                const ULong64_t currentGlobBcInRun = globalBcInRun_input;
                const int currentCollIdMC = collIdMC_input;
                int isCurrentDuplicate = 0;
                const int countRunNumber = std::count(vecRunNumber.begin(), vecRunNumber.end(), currentRun);
                const int countGlobBcInRun = std::count(vecGlobBcInRun.begin(), vecGlobBcInRun.end(), currentGlobBcInRun);
                const int countCollIdMc = std::count(vecCollIdMC.begin(), vecCollIdMC.end(), currentCollIdMC);
                std::cout << "          currentGlobBcInRun=" << currentGlobBcInRun << std::endl;
                std::cout << "             currentCollIdMC=" << currentCollIdMC << std::endl;
                std::cout << "          >>> countRunNumber=" << countRunNumber << std::endl;
                std::cout << "          >>> countGlobBcInRun=" << countGlobBcInRun << std::endl;
                std::cout << "          >>> countCollIdMc=" << countCollIdMc << std::endl;
                if( countRunNumber > 1 && countGlobBcInRun > 1 && ( !isMC || (isMC && ( countCollIdMc > 1 ) ) ) ) {
                    /// This is a collision that shares the same BC with (at least) another collision
                    /// (i.e. BC with the same gloabId in the same run)
                    /// and, in MC, that shares the same MC collision with (at least) another collision in the same DF
                    /// (i.e. same collisionId, which is unique within each DF)
                    isCurrentDuplicate = 1;
                }

                /// Fill the tree
                isEvSel = isEvSel_input;
                runNumber = runNumber_input;
                posX = posX_input;
                posY = posY_input;
                posZ = posZ_input;
                covXX = covXX_input;
                covXY = covXY_input;
                covXZ = covXZ_input;
                covYY = covYY_input;
                covYZ = covYZ_input;
                covZZ = covZZ_input;
                numContrib = numContrib_input;
                numTracksAll = numTracksAll_input;
                numTracksFiltered = numTracksFiltered_input;
                chi2PV = chi2PV_input;
                globalBcInRun = globalBcInRun_input;
                ft0posZ = ft0posZ_input;
                signalFT0A = signalFT0A_input;
                signalFT0C = signalFT0C_input;
                signalFT0M = signalFT0M_input;
                signalV0A = signalV0A_input;
                collTime = collTime_input;
                collTimeRes = collTimeRes_input;
                counterCollision = counterCollision_input;
                counterDF = counterDF_input;
                collIdMC = collIdMC_input;
                posXMC = posXMC_input;
                posYMC = posYMC_input;
                posZMC = posZMC_input;
                collTimeMC = collTimeMC_input;
                isFakeColl = isFakeColl_input;
                recoPvPerMcColl = recoPvPerMcColl_input;
                isPvHighestContribForMcColl = isPvHighestContribForMcColl_input;
                isDuplicate = isCurrentDuplicate;

                /// fill the output tree
                finalTree.Fill();
            }
        } /// end loop over directories == DFs

    } /// end loop over all the files

    /// save the tree
    fout.cd();
    finalTree.Write();
    fout.Close();

    // end of execution - take the time
    std::cout << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "==> Start of execution at " << std::ctime(&start_time);// << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    std::time_t stop_time = std::chrono::high_resolution_clock::to_time_t(stop);
    std::cout << "==> End of execution at " << std::ctime(&stop_time);// << std::endl;
    auto dur = std::chrono::duration_cast<std::chrono::duration<double>>(stop-start);   // in seconds
    std::cout << "==> Duration: " << ((int) dur.count())/3600 << "h " << ((int) dur.count()%3600)/60 << "m " << ((int) dur.count())%60 << "s" << std::endl;
    std::cout << "==============================================" << std::endl << std::endl;

    return;
}