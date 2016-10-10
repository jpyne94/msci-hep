#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TMVA/Factory.h>

int main(int argc, char * argv[]) {  

// Create ouput file, factory object and open the input file

  int jetnum = std::atoi(argv[1]);

  bool lowpt = ( std::atoi(argv[2]) == 1 );

//  bool tracks = false;
  bool tracks = false;
  bool useMET = false;

  std::cout << "Low pt " << lowpt << std::endl;
  std::cout << "Jetnum " << jetnum << std::endl;

  for(int i=0;i<2;i++){

    TString output_file = (jetnum==2) ? "_2jet" : "_3jet";
    TString output_file_pt = (lowpt) ? "_lowpt" : "";
    TString output_file_met = (useMET) ? "_MET" : "";
    TString kFold = (i==0) ? "_even" : "_odd";
    TString trackstr = (tracks) ? "_tracks" : "";

    TFile* outputFile = TFile::Open( "TMVA"+output_file+output_file_pt+output_file_met+kFold+trackstr+".root", "RECREATE" );
    TMVA::Factory* factory = new TMVA::Factory("tmvaTest"+output_file+output_file_pt+output_file_met+trackstr+kFold, outputFile, "");
//  TFile* trainingFile = new TFile("./../../../PGS/outputGeneral_fit_ntuple.mc15_13TeV.410000.DAOD_FTAG2_rescaling_train.root");
//  TFile* testFile = new TFile("./../../../PGS/outputGeneral_fit_ntuple.mc15_13TeV.410000.DAOD_FTAG2_rescaling_test.root");
//  TFile* trainingFile = new TFile("./../../../PGS/outputGeneral_fit_all_rescaling_train.root");
//  TFile* testFile = new TFile("./../../../PGS/outputGeneral_fit_all_rescaling_test.root");


    TString signal = "tree_55";
    TString background_A = "tree_51";
    TString background_B = "tree_11";

    if(jetnum==3){
      signal = "tree_551";
      background_A = "tree_511";
      background_B = "tree_111";
    }

    TChain* sig = new TChain(signal);

    std::vector<std::string> signalFiles;
    signalFiles.push_back("");
    /*
    signalFiles.push_back("SysMCAtNLO");
    signalFiles.push_back("SysPowhegHerwig");
    signalFiles.push_back("SysttbarRadHi");
    signalFiles.push_back("SysttbarRadLo");
    */
    for(int r=0;r<signalFiles.size();r++){
      int entriesToGet = 30000;
      if(tracks) sig->Add(("/unix/atlas3/abell/tmvaTraining/PGS_tracks/outputGeneral_fit_top"+signalFiles.at(r)+"_rescaling.root").c_str());
      else sig->Add(("../PGS/outputGeneral_fit_top"+signalFiles.at(r)+"_rescaling.root").c_str());
//      sig->Add(("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_top"+signalFiles.at(r)+"_rescaling.root").c_str(), entriesToGet);
//      std::cout << sig->GetEntries() <<std::endl;
    }
//    TFile* inputFileSignal = new TFile("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_top_all_rescaling.root");
    TFile* inputFile; // = new TFile("/unix/atlasvhbb/abell/mc15data7/PGS/outputGeneral_fit_all_rescaling.root");
    if(tracks) inputFile = TFile::Open("/unix/atlas3/abell/tmvaTraining/PGS_tracks/outputGeneral_fit_all_rescaling.root");
    else inputFile = TFile::Open("../PGS/outputGeneral_fit_all_rescaling.root");

// get the TTree objects from the input files

//    TTree* sig = (TTree*)inputFileSignal->Get(signal);
    TTree* bkg_A = (TTree*)inputFile->Get(background_A);
    TTree* bkg_B = (TTree*)inputFile->Get(background_B);

    outputFile->cd();
// global event weights (see below for setting event-wise weights)

    double sigWeight = 1.0;
    double bkgWeight = 1.0;

//  factory->AddSpectator("weight", 'F');

    TCut mycut_train = (i==0) ? "isEven" : "!isEven";
    TCut mycut_test = (i!=0) ? "isEven" : "!isEven";

    factory->AddTree(sig, "Signal", sigWeight, mycut_train, "train");
    factory->AddTree(sig, "Signal", sigWeight, mycut_test, "test");

    factory->AddTree(bkg_A, "Background", bkgWeight, mycut_train, "train");
    factory->AddTree(bkg_A, "Background", bkgWeight, mycut_test, "test");
    factory->AddTree(bkg_B, "Background", bkgWeight, mycut_train, "train");
    factory->AddTree(bkg_B, "Background", bkgWeight, mycut_test, "test");

// Define the input variables that shall be used for the MVA training
// (the variables used in the expression must exist in the original TTree).

    factory->SetSignalWeightExpression( "weight" );
    factory->SetBackgroundWeightExpression( "weight" );

    factory->AddVariable("imbalance", 'F');
    factory->AddVariable("leading_pt", 'F');
    factory->AddVariable("sub_leading_pt", 'F');
//     factory->AddVariable("forwardjets", 'I');
    if(jetnum==3) factory->AddVariable("sub_sub_leading_pt", 'F');
    factory->AddVariable("delta_phi", 'F');
    factory->AddVariable("delta_r", 'F');
    factory->AddVariable("mlj", 'F');
    if(useMET) factory->AddVariable("train_met", 'F');

    factory->AddVariable("minl1j", 'F');
    factory->AddVariable("minl2j", 'F');

//  factory->AddVariable("train_met", 'F');

    TCut mycut = "";
    TCut lowpT = (tracks) ? "0.02" : "0.02";

    if(tracks){
      if(lowpt){
	mycut = "sub_leading_pt < 0.02  && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
	if(jetnum==3) mycut = "sub_leading_pt < 0.02 || sub_sub_leading_pt < 0.02 && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
      }
      else {
	mycut = "sub_leading_pt > 0.02 && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
	if(jetnum==3) mycut = "sub_leading_pt > 0.02 && sub_sub_leading_pt > 0.02  && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
      }
    }else{
      if(lowpt){
	mycut = "sub_leading_pt < 0.03  && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
	if(jetnum==3) mycut = "sub_leading_pt < 0.03 || sub_sub_leading_pt < 0.03 && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
      }
      else {
	mycut = "sub_leading_pt > 0.03 && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
	if(jetnum==3) mycut = "sub_leading_pt > 0.03 && sub_sub_leading_pt > 0.03  && (leptontype==0 || (etmiss>60e3 && (mll<80e3 || mll>100e3) && mll>50e3))";
      }
    }

  
// Tell the factory how to use the training and testing events
    factory->PrepareTrainingAndTestTree( mycut, "NormMode=None:!V" );

// Book MVA methods (see TMVA manual).    

    factory->BookMethod(TMVA::Types::kBDT, "BDT_ABbase","IgnoreNegWeightsInTraining:!H:!V:NTrees=200:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.10:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");
    factory->BookMethod(TMVA::Types::kBDT, "BDT_GBMbase","IgnoreNegWeightsInTraining:!H:!V:NTrees=200:MaxDepth=4:BoostType=Grad:Shrinkage=0.10:SeparationType=GiniIndex:nCuts=100:PruneMethod=NoPruning");

// Train, test and evaluate all methods

    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();    

// Save the output and finish up

    outputFile->Close();
    std::cout << "==> wrote root file TMVA.root" << std::endl;
    std::cout << "==> TMVAnalysis is done!" << std::endl; 

    delete factory;
  }
  return 0;

}
