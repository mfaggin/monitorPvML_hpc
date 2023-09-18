#include "TChain.h"
#include "TDirectoryFile.h"
#include "TList.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TFileMerger.h"

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <algorithm>

void filterTree(std::string inputsNames, std::string myCase, bool isMC);