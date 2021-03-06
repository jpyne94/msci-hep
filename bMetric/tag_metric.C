#define tag_metric_cxx
#include "tag_metric.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

void tag_metric::Loop()
{
	//if (fChain == 0) return;



	

	first_pass = true;

	// gDirectory is the working root file.
	// 
	// Get its name as a string for renaming
	// file to be created.
	dir_name = gDirectory->GetName();

	//Cut off .root suffix.
	prefix = dir_name.substr(0, dir_name.size() - 5);

	// Get rid of processtree.
	// gDirectory->Delete("processtree");

	//Set new output file.
	outputfile = new TFile((TString) prefix + "_proc.root", "RECREATE");

	// Blank output tree.
	outputtree = new TNtuple("outputtree","Jet Level Tree",
	"jet_pt:jet_truthflav:jet_mv2c20:jet_mv2c10:jet_mv2c00:"
	"jet_sv0_sig3d:jet_sv1_sig3d:jet_ip3d_llr:jet_jf_llr:"
	"jet_jfcombnn_llr:jet_sv1ip3d:jet_ip2d_llr");

	

	// Turn off all branches; switch on those wanted.
	fChain->SetBranchStatus("*",0);

	fChain->SetBranchStatus("jet_pt",1);
	fChain->SetBranchStatus("jet_truthflav",1);
	fChain->SetBranchStatus("jet_mv2c20",1);
	fChain->SetBranchStatus("jet_mv2c10",1);
	fChain->SetBranchStatus("jet_mv2c00",1);
	fChain->SetBranchStatus("jet_sv0_sig3d",1);
	fChain->SetBranchStatus("jet_sv1_sig3d",1);
	fChain->SetBranchStatus("jet_ip3d_llr",1);
	fChain->SetBranchStatus("jet_jf_llr",1);
	fChain->SetBranchStatus("jet_jfcombnn_llr",1);
	fChain->SetBranchStatus("jet_sv1ip3d",1);
	fChain->SetBranchStatus("jet_ip2d_llr",1);
	




	Long64_t nentries = fChain->GetEntriesFast();

	Long64_t nbytes = 0, nb = 0;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
			Long64_t ientry = LoadTree(jentry);
		if (ientry < 0) break;
		nb = fChain->GetEntry(jentry);
     	nbytes += nb;

     	int i = 0;

		// Iteration for each jet using iterator for-loop.
		// Remember, CINT C++ does not support C++11 features.
		for (vector<int>::iterator it = (*jet_truthflav).begin();
			it != (*jet_truthflav).end(); ++it) {

			// Dereference the arrays (* binding) THEN index.



			pt = (*jet_pt)[i];
			truthflav = (*jet_truthflav)[i];
			mv2c20 = (*jet_mv2c20)[i];
			mv2c10 = (*jet_mv2c10)[i];
			mv2c00 = (*jet_mv2c00)[i];
			sv0_sig3d = (*jet_sv0_sig3d)[i];
			mv2c20 = (*jet_mv2c20)[i];
			mv2c10 = (*jet_mv2c10)[i];
			mv2c00 = (*jet_mv2c00)[i];
			ip3d_llr = (*jet_ip3d_llr)[i];
			sv1_sig3d = (*jet_sv1_sig3d)[i];
			jf_llr = (*jet_jf_llr)[i];
			jfcombnn_llr = (*jet_jfcombnn_llr)[i];
			sv1ip3d = (*jet_sv1ip3d)[i];
			ip2d_llr = (*jet_ip2d_llr)[i];



			// Fill da tree.
			outputtree->Fill(pt,truthflav,mv2c20,mv2c10,mv2c00,
				sv0_sig3d,sv1_sig3d,ip3d_llr,jf_llr,jfcombnn_llr,sv1ip3d,ip2d_llr);



			i++;
		}   	
    }


    // Write outputtree in memory to outputfile as key.
    outputfile->Write();

    // Delete all objects left in memory.
    outputfile->Close();


}
