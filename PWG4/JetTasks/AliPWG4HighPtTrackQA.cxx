/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//-----------------------------------------------------------------------
// This class stores QA variables as function of pT for different type
// of tracks and track selection criteria
// Output: Histograms for different set of cuts
//-----------------------------------------------------------------------
// Author : Marta Verweij - UU
//-----------------------------------------------------------------------

#ifndef ALIPWG4HIGHPTTRACKQA_CXX
#define ALIPWG4HIGHPTTRACKQA_CXX

#include "AliPWG4HighPtTrackQA.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TList.h"
#include "TFile.h"
#include "TChain.h"
#include "TH3F.h"
#include "TKey.h"
#include "TSystem.h"
#include "TBits.h"

#include "AliAnalysisManager.h"
#include "AliESDInputHandler.h"
#include "AliMCEvent.h"
#include "AliMCEventHandler.h"
#include "AliStack.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"
#include "AliExternalTrackParam.h"
#include "AliLog.h"
#include "AliGenPythiaEventHeader.h"
#include "AliGenCocktailEventHeader.h"
#include "AliCentrality.h"
#include "AliAODVertex.h"
#include "AliAODEvent.h"
//#include "AliAnalysisHelperJetTasks.h"

using namespace std; //required for resolving the 'cout' symbol

ClassImp(AliPWG4HighPtTrackQA)

AliPWG4HighPtTrackQA::AliPWG4HighPtTrackQA()
: AliAnalysisTaskSE(), 
  fDataType(kESD),
  fEvent(0x0),
  fESD(0x0),
  fVtx(0x0),
  fTrackCuts(0), 
  fTrackType(0),
  fFilterMask(0),
  fSigmaConstrainedMax(-1.),
  fPtMax(100.),
  fIsPbPb(0),
  fCentClass(10),
  fNVariables(18),
  fVariables(0x0),
  fAvgTrials(1),
  fNEventAll(0),
  fNEventSel(0),
  fNEventReject(0),
  fh1Centrality(0x0),
  fh1Xsec(0),
  fh1Trials(0),
  fh1PtHard(0),
  fh1PtHardTrials(0),
  fh1NTracksAll(0x0),
  fh1NTracksReject(0x0),
  fh1NTracksSel(0x0),
  fPtAll(0),  
  fPtSel(0),    
  fPtPhi(0x0),
  fPtEta(0x0),
  fPtDCA2D(0x0),
  fPtDCAZ(0x0),
  fPtNClustersTPC(0x0),
  fPtNPointITS(0x0),
  fPtChi2C(0x0),
  fPtNSigmaToVertex(0x0),
  fPtRelUncertainty1Pt(0x0),
  fPtUncertainty1Pt(0x0),
  fPtChi2PerClusterTPC(0x0),
  fPtNCrossedRows(0x0),
  fPtNCrossedRowsNClusF(0x0),
  fPtNCrRNCrRNClusF(0x0),
  fPtSigmaY2(0x0),
  fPtSigmaZ2(0x0),
  fPtSigmaSnp2(0x0),
  fPtSigmaTgl2(0x0),
  fPtSigma1Pt2(0x0),
  fProfPtSigmaY2(0x0),
  fProfPtSigmaZ2(0x0),
  fProfPtSigmaSnp2(0x0),
  fProfPtSigmaTgl2(0x0),
  fProfPtSigma1Pt2(0x0),
  fProfPtSigma1Pt(0x0),
  fProfPtPtSigma1Pt(0x0),
  fSystTrackCuts(0x0),
  fHistList(0)
{
  SetNVariables(18);

  fPtBinEdges[0][0] = 10.;
  fPtBinEdges[0][1] = 1.;
  fPtBinEdges[1][0] = 20.;
  fPtBinEdges[1][1] = 2.;
  fPtBinEdges[2][0] = 50.;
  fPtBinEdges[2][1] = 5.;

}
//________________________________________________________________________
AliPWG4HighPtTrackQA::AliPWG4HighPtTrackQA(const char *name): 
  AliAnalysisTaskSE(name), 
  fDataType(kESD),
  fEvent(0x0),
  fESD(0x0),
  fVtx(0x0),
  fTrackCuts(),
  fTrackType(0),
  fFilterMask(0),
  fSigmaConstrainedMax(-1.),
  fPtMax(100.),
  fIsPbPb(0),
  fCentClass(10),
  fNVariables(18),
  fVariables(0x0),
  fAvgTrials(1),
  fNEventAll(0),
  fNEventSel(0),
  fNEventReject(0),
  fh1Centrality(0x0),
  fh1Xsec(0),
  fh1Trials(0),
  fh1PtHard(0),
  fh1PtHardTrials(0),
  fh1NTracksAll(0x0),
  fh1NTracksReject(0x0),
  fh1NTracksSel(0x0),
  fPtAll(0),
  fPtSel(0),
  fPtPhi(0x0),
  fPtEta(0x0),
  fPtDCA2D(0x0),
  fPtDCAZ(0x0),
  fPtNClustersTPC(0x0),
  fPtNPointITS(0x0),
  fPtChi2C(0x0),
  fPtNSigmaToVertex(0x0),
  fPtRelUncertainty1Pt(0x0),
  fPtUncertainty1Pt(0x0),
  fPtChi2PerClusterTPC(0x0),
  fPtNCrossedRows(0x0),
  fPtNCrossedRowsNClusF(0x0),
  fPtNCrRNCrRNClusF(0x0),
  fPtSigmaY2(0x0),
  fPtSigmaZ2(0x0),
  fPtSigmaSnp2(0x0),
  fPtSigmaTgl2(0x0),
  fPtSigma1Pt2(0x0),
  fProfPtSigmaY2(0x0),
  fProfPtSigmaZ2(0x0),
  fProfPtSigmaSnp2(0x0),
  fProfPtSigmaTgl2(0x0),
  fProfPtSigma1Pt2(0x0),
  fProfPtSigma1Pt(0x0),
  fProfPtPtSigma1Pt(0x0),
  fSystTrackCuts(0x0),
  fHistList(0)
{
  //
  // Constructor. Initialization of Inputs and Outputs
  //
  AliDebug(2,Form("AliPWG4HighPtTrackQA Calling Constructor"));

  SetNVariables(18);

  fPtBinEdges[0][0] = 10.;
  fPtBinEdges[0][1] = 1.;
  fPtBinEdges[1][0] = 20.;
  fPtBinEdges[1][1] = 2.;
  fPtBinEdges[2][0] = 50.;
  fPtBinEdges[2][1] = 5.;

  // Input slot #0 works with a TChain ESD
  DefineInput(0, TChain::Class());
  // Output slot #1 write into a TList
  DefineOutput(1, TList::Class());
}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::SetPtBinEdges(Int_t region, Double_t ptmax, Double_t ptBinWidth) {

  if(region<3) {
    fPtBinEdges[region][0] = ptmax;
    fPtBinEdges[region][1] = ptBinWidth;
  }
  else {
    AliError("Only 3 regions alowed. Use region 0/1/2\n");
    return;
  }

}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::UserCreateOutputObjects() {
  //Create output objects
  AliDebug(2,Form(">> AliPWG4HighPtTrackQA::UserCreateOutputObjects \n"));

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE); 

  OpenFile(1);
  fHistList = new TList();
  fHistList->SetOwner(kTRUE);
  
  Float_t fgkPtMin = 0.;
  Float_t fgkPtMax = fPtMax;

  //  Float_t ptBinEdges[2][2];
  // ptBinEdges[0][0] = 10.;
  // ptBinEdges[0][1] = 1.;
  // ptBinEdges[1][0] = 20.;
  // ptBinEdges[1][1] = 2.;
  // Float_t binWidth3 = 5.;
  // if(fPtMax>100.) {
  //   ptBinEdges[0][0] = 100.;
  //   ptBinEdges[0][1] = 5.;
  //   ptBinEdges[1][0] = 300.;
  //   ptBinEdges[1][1] = 10.;
  //   binWidth3 = 20.;
  // }

  //fPtBinEdges[region][0] = ptmax of region ; fPtBinEdges[region][1] = binWidth of region
  const Float_t ptmin1 =  fgkPtMin;
  const Float_t ptmax1 =  fPtBinEdges[0][0];
  const Float_t ptmin2 =  ptmax1 ;
  const Float_t ptmax2 =  fPtBinEdges[1][0];
  const Float_t ptmin3 =  ptmax2 ;
  const Float_t ptmax3 =  fgkPtMax;
  const Int_t nbin11 = (int)((ptmax1-ptmin1)/fPtBinEdges[0][1]);
  const Int_t nbin12 = (int)((ptmax2-ptmin2)/fPtBinEdges[1][1])+nbin11;
  const Int_t nbin13 = (int)((ptmax3-ptmin3)/fPtBinEdges[2][2])+nbin12;
  Int_t fgkNPtBins=nbin13;
  //Create array with low edges of each bin
  Double_t *binsPt=new Double_t[fgkNPtBins+1];
  for(Int_t i=0; i<=fgkNPtBins; i++) {
    if(i<=nbin11) binsPt[i]=(Double_t)ptmin1 + (ptmax1-ptmin1)/nbin11*(Double_t)i ;
    if(i<=nbin12 && i>nbin11) binsPt[i]=(Double_t)ptmin2 + (ptmax2-ptmin2)/(nbin12-nbin11)*((Double_t)i-(Double_t)nbin11) ;
    if(i<=nbin13 && i>nbin12) binsPt[i]=(Double_t)ptmin3 + (ptmax3-ptmin3)/(nbin13-nbin12)*((Double_t)i-(Double_t)nbin12) ;
  }

  Int_t fgkNPhiBins = 18*6;
  Float_t kMinPhi   = 0.;
  Float_t kMaxPhi   = 2.*TMath::Pi();
  Double_t *binsPhi = new Double_t[fgkNPhiBins+1];
  for(Int_t i=0; i<=fgkNPhiBins; i++) binsPhi[i]=(Double_t)kMinPhi + (kMaxPhi-kMinPhi)/fgkNPhiBins*(Double_t)i ;

  Int_t fgkNEtaBins=20;
  Float_t fgkEtaMin = -1.;
  Float_t fgkEtaMax = 1.;
  Double_t *binsEta=new Double_t[fgkNEtaBins+1];
  for(Int_t i=0; i<=fgkNEtaBins; i++) binsEta[i]=(Double_t)fgkEtaMin + (fgkEtaMax-fgkEtaMin)/fgkNEtaBins*(Double_t)i ;

  Int_t fgkNNClustersTPCBins=80;
  Float_t fgkNClustersTPCMin = 0.5;
  Float_t fgkNClustersTPCMax = 160.5;
  Double_t *binsNClustersTPC=new Double_t[fgkNNClustersTPCBins+1];
  for(Int_t i=0; i<=fgkNNClustersTPCBins; i++) binsNClustersTPC[i]=(Double_t)fgkNClustersTPCMin + (fgkNClustersTPCMax-fgkNClustersTPCMin)/fgkNNClustersTPCBins*(Double_t)i ;

  Int_t fgkNDCA2DBins=80;
  Float_t fgkDCA2DMin = -0.2;
  Float_t fgkDCA2DMax = 0.2;
  if(fTrackType==1 || fTrackType==2 || fTrackType==4) {
    fgkDCA2DMin = -2.;
    fgkDCA2DMax = 2.;
  }
  Double_t *binsDCA2D=new Double_t[fgkNDCA2DBins+1];
  for(Int_t i=0; i<=fgkNDCA2DBins; i++) binsDCA2D[i]=(Double_t)fgkDCA2DMin + (fgkDCA2DMax-fgkDCA2DMin)/fgkNDCA2DBins*(Double_t)i ;

  Int_t fgkNDCAZBins=80;
  Float_t fgkDCAZMin = -2.;
  Float_t fgkDCAZMax = 2.;
  if(fTrackType==1 || fTrackType==2 || fTrackType==4) {
    fgkDCAZMin = -5.;
    fgkDCAZMax = 5.;
  }
  Double_t *binsDCAZ=new Double_t[fgkNDCAZBins+1];
  for(Int_t i=0; i<=fgkNDCAZBins; i++) binsDCAZ[i]=(Double_t)fgkDCAZMin + (fgkDCAZMax-fgkDCAZMin)/fgkNDCAZBins*(Double_t)i ;

  Int_t fgkNNPointITSBins=9;
  Float_t fgkNPointITSMin = -0.5;
  Float_t fgkNPointITSMax = 8.5;
  Double_t *binsNPointITS=new Double_t[fgkNNPointITSBins+1];
  for(Int_t i=0; i<=fgkNNPointITSBins; i++) binsNPointITS[i]=(Double_t)fgkNPointITSMin + (fgkNPointITSMax-fgkNPointITSMin)/fgkNNPointITSBins*(Double_t)i ;

  Int_t fgkNNSigmaToVertexBins=40;
  Float_t fgkNSigmaToVertexMin = 0.;
  Float_t fgkNSigmaToVertexMax = 8.;
  Double_t *binsNSigmaToVertex=new Double_t[fgkNNSigmaToVertexBins+1];
  for(Int_t i=0; i<=fgkNNSigmaToVertexBins; i++) binsNSigmaToVertex[i]=(Double_t)fgkNSigmaToVertexMin + (fgkNSigmaToVertexMax-fgkNSigmaToVertexMin)/fgkNNSigmaToVertexBins*(Double_t)i ;

  Int_t fgkNChi2CBins=20;
  Float_t fgkChi2CMin = 0.;
  Float_t fgkChi2CMax = 100.;
  Double_t *binsChi2C=new Double_t[fgkNChi2CBins+1];
  for(Int_t i=0; i<=fgkNChi2CBins; i++) binsChi2C[i]=(Double_t)fgkChi2CMin + (fgkChi2CMax-fgkChi2CMin)/fgkNChi2CBins*(Double_t)i ;

  Int_t fgkNRel1PtUncertaintyBins=30;
  Float_t fgkRel1PtUncertaintyMin = 0.;
  Float_t fgkRel1PtUncertaintyMax = 0.3;
  if(fTrackType==1 || fTrackType==2 || fTrackType==4) 
    fgkRel1PtUncertaintyMax = 0.5; 
  Double_t *binsRel1PtUncertainty=new Double_t[fgkNRel1PtUncertaintyBins+1];
  for(Int_t i=0; i<=fgkNRel1PtUncertaintyBins; i++) binsRel1PtUncertainty[i]=(Double_t)fgkRel1PtUncertaintyMin + (fgkRel1PtUncertaintyMax-fgkRel1PtUncertaintyMin)/fgkNRel1PtUncertaintyBins*(Double_t)i ;

  Int_t fgkNUncertainty1PtBins = 30;
  Float_t fgkUncertainty1PtMin = 0.;
  Float_t fgkUncertainty1PtMax = 0.1;
  if(fTrackType==1 || fTrackType==2 || fTrackType==4) 
    fgkUncertainty1PtMax = 0.2; 
  Double_t *binsUncertainty1Pt=new Double_t[fgkNUncertainty1PtBins+1];
  for(Int_t i=0; i<=fgkNUncertainty1PtBins; i++) binsUncertainty1Pt[i]=(Double_t)fgkUncertainty1PtMin + (fgkUncertainty1PtMax-fgkUncertainty1PtMin)/fgkNUncertainty1PtBins*(Double_t)i ;

  Float_t fgkChi2PerClusMin = 0.;
  Float_t fgkChi2PerClusMax = 4.;
  Int_t fgkNChi2PerClusBins = (int)(fgkChi2PerClusMax*10.);
  Double_t *binsChi2PerClus=new Double_t[fgkNChi2PerClusBins+1];
  for(Int_t i=0; i<=fgkNChi2PerClusBins; i++) binsChi2PerClus[i]=(Double_t)fgkChi2PerClusMin + (fgkChi2PerClusMax-fgkChi2PerClusMin)/fgkNChi2PerClusBins*(Double_t)i ;

  Int_t fgkNCrossedRowsNClusFBins  = 50;
  Float_t fgkNCrossedRowsNClusFMin = 0.;
  Float_t fgkNCrossedRowsNClusFMax = 1.;
  Double_t *binsNCrossedRowsNClusF=new Double_t[fgkNCrossedRowsNClusFBins+1];
  for(Int_t i=0; i<=fgkNCrossedRowsNClusFBins; i++) binsNCrossedRowsNClusF[i]=(Double_t)fgkNCrossedRowsNClusFMin + (fgkNCrossedRowsNClusFMax-fgkNCrossedRowsNClusFMin)/fgkNCrossedRowsNClusFBins*(Double_t)i ;

  Float_t fgk1PtMin = 0.;
  Float_t fgk1PtMax = 6.;
  Float_t binEdge1Pt1 = 1.;
  Float_t binWidth1Pt1 = 0.05;
  Int_t fgkN1PtBins1 = (int)((binEdge1Pt1-fgk1PtMin)/binWidth1Pt1);
  Float_t binWidth1Pt2 = 0.1;
  Int_t fgkN1PtBins2 = (int)((fgk1PtMax-binEdge1Pt1)/binWidth1Pt2);
  Int_t fgkN1PtBins = fgkN1PtBins1+fgkN1PtBins2;
  Double_t *bins1Pt=new Double_t[fgkN1PtBins+1];

  for(Int_t i=0; i<=fgkN1PtBins; i++) {
    if(i<=fgkN1PtBins1) 
      bins1Pt[i]=(Double_t)fgk1PtMin + (Double_t)(binEdge1Pt1-fgk1PtMin)/(Double_t)fgkN1PtBins1*(Double_t)i;
    if(i<=fgkN1PtBins && i>fgkN1PtBins1)
      bins1Pt[i]=(Double_t)binEdge1Pt1 + (Double_t)(fgk1PtMax-binEdge1Pt1)/(Double_t)fgkN1PtBins2*(Double_t)(i-fgkN1PtBins1);
  }

  Int_t fgkNSigmaY2Bins = 50;
  Float_t fgkSigmaY2Min = 0.;
  Float_t fgkSigmaY2Max = 1.;
  if(fTrackType==1) fgkSigmaY2Max = 4.;
  if(fTrackType==2 || fTrackType==4) fgkSigmaY2Max = 0.1;
  Double_t *binsSigmaY2=new Double_t[fgkNSigmaY2Bins+1];
  for(Int_t i=0; i<=fgkNSigmaY2Bins; i++) binsSigmaY2[i]=(Double_t)fgkSigmaY2Min + (fgkSigmaY2Max-fgkSigmaY2Min)/fgkNSigmaY2Bins*(Double_t)i ;

  Int_t fgkNSigmaZ2Bins = 50;
  Float_t fgkSigmaZ2Min = 0.;
  Float_t fgkSigmaZ2Max = 0.4;
  Double_t *binsSigmaZ2=new Double_t[fgkNSigmaZ2Bins+1];
  for(Int_t i=0; i<=fgkNSigmaZ2Bins; i++) binsSigmaZ2[i]=(Double_t)fgkSigmaZ2Min + (fgkSigmaZ2Max-fgkSigmaZ2Min)/fgkNSigmaZ2Bins*(Double_t)i ;

  Int_t fgkNSigmaSnp2Bins = 50;
  Float_t fgkSigmaSnp2Min = 0.;
  Float_t fgkSigmaSnp2Max = 0.05;
  if(fTrackType==1) fgkSigmaSnp2Max = 0.2;
  if(fTrackType==2 || fTrackType==4) fgkSigmaSnp2Max = 0.1;
  Double_t *binsSigmaSnp2=new Double_t[fgkNSigmaSnp2Bins+1];
  for(Int_t i=0; i<=fgkNSigmaSnp2Bins; i++) binsSigmaSnp2[i]=(Double_t)fgkSigmaSnp2Min + (fgkSigmaSnp2Max-fgkSigmaSnp2Min)/fgkNSigmaSnp2Bins*(Double_t)i ;

  Int_t fgkNSigmaTgl2Bins = 50;
  Float_t fgkSigmaTgl2Min = 0.;
  Float_t fgkSigmaTgl2Max = 0.1;
  if(fTrackType==1) fgkSigmaTgl2Max = 0.2;
  if(fTrackType==2 || fTrackType==4) fgkSigmaTgl2Max = 0.1;
  Double_t *binsSigmaTgl2=new Double_t[fgkNSigmaTgl2Bins+1];
  for(Int_t i=0; i<=fgkNSigmaTgl2Bins; i++) binsSigmaTgl2[i]=(Double_t)fgkSigmaTgl2Min + (fgkSigmaTgl2Max-fgkSigmaTgl2Min)/fgkNSigmaTgl2Bins*(Double_t)i ;

  Int_t fgkNSigma1Pt2Bins = 50;
  Float_t fgkSigma1Pt2Min = 0.;
  Float_t fgkSigma1Pt2Max = 1.;
  Double_t *binsSigma1Pt2=new Double_t[fgkNSigma1Pt2Bins+1];
  for(Int_t i=0; i<=fgkNSigma1Pt2Bins; i++) binsSigma1Pt2[i]=(Double_t)fgkSigma1Pt2Min + (fgkSigma1Pt2Max-fgkSigma1Pt2Min)/fgkNSigma1Pt2Bins*(Double_t)i ;


  fNEventAll = new TH1F("fNEventAll","NEventAll",1,-0.5,0.5);
  fHistList->Add(fNEventAll);
  fNEventSel = new TH1F("fNEventSel","NEvent Selected for analysis",1,-0.5,0.5);
  fHistList->Add(fNEventSel);
  fNEventReject = new TH1F("fNEventReject","Reason events are rejectected for analysis",20,0,20);
  //Set labels
  fNEventReject->Fill("noESD",0);
  fNEventReject->Fill("Trigger",0);
  fNEventReject->Fill("NTracks<2",0);
  fNEventReject->Fill("noVTX",0);
  fNEventReject->Fill("VtxStatus",0);
  fNEventReject->Fill("NCont<2",0);
  fNEventReject->Fill("ZVTX>10",0);
  fNEventReject->Fill("cent",0);
  fNEventReject->Fill("cent>90",0);
  fHistList->Add(fNEventReject);

  fh1Centrality = new TH1F("fh1Centrality","fh1Centrality; Centrality %",100,0,100);
  fHistList->Add(fh1Centrality);

  fh1Xsec = new TProfile("fh1Xsec","xsec from pyxsec.root",1,0,1);
  fh1Xsec->GetXaxis()->SetBinLabel(1,"<#sigma>");
  fHistList->Add(fh1Xsec);

  fh1Trials = new TH1F("fh1Trials","trials root file",1,0,1);
  fh1Trials->GetXaxis()->SetBinLabel(1,"#sum{ntrials}");
  fHistList->Add(fh1Trials);

  fh1PtHard       = new TH1F("fh1PtHard","PYTHIA Pt hard;p_{T,hard}",350,-.5,349.5);
  fHistList->Add(fh1PtHard);
  fh1PtHardTrials = new TH1F("fh1PtHardTrials","PYTHIA Pt hard weight with trials;p_{T,hard}",350,-.5,349.5);
  fHistList->Add(fh1PtHardTrials);

  fh1NTracksAll = new TH1F("fh1NTracksAll","fh1NTracksAll",1,-0.5,0.5);
  fHistList->Add(fh1NTracksAll);

  fh1NTracksReject = new TH1F("fh1NTracksReject","fh1NTracksReject",1,-0.5,0.5);
  fh1NTracksReject->Fill("noESDtrack",0);
  fh1NTracksReject->Fill("noTPCInner",0);
  fh1NTracksReject->Fill("FillTPC",0);
  fh1NTracksReject->Fill("noTPConly",0);
  fh1NTracksReject->Fill("relate",0);
  fh1NTracksReject->Fill("trackCuts",0);
  fh1NTracksReject->Fill("laser",0);
  fh1NTracksReject->Fill("chi2",0);
  fHistList->Add(fh1NTracksReject);

  fh1NTracksSel = new TH1F("fh1NTracksSel","fh1NTracksSel",1,-0.5,0.5);
  fHistList->Add(fh1NTracksSel);

  fSystTrackCuts = new TH1F("fSystTrackCuts","fSystTrackCuts",1,-0.5,0.5);
  fSystTrackCuts->Fill("noCut",0);
  fSystTrackCuts->Fill("eta",0);
  fSystTrackCuts->Fill("0.15<pT<1e10",0);
  fSystTrackCuts->Fill("kink",0);
  fSystTrackCuts->Fill("NClusterTPC",0);
  fSystTrackCuts->Fill("Chi2PerNClusTPC",0);
  fSystTrackCuts->Fill("DCA2D",0);
  fHistList->Add(fSystTrackCuts);


  fPtAll = new TH1F("fPtAll","PtAll",fgkNPtBins, binsPt);
  fHistList->Add(fPtAll);
  fPtSel = new TH1F("fPtSel","PtSel",fgkNPtBins, binsPt);
  fHistList->Add(fPtSel);

  fPtPhi = new TH2F("fPtPhi","fPtPhi",fgkNPtBins,binsPt,fgkNPhiBins,binsPhi);
  fHistList->Add(fPtPhi);
 
  fPtEta = new TH2F("fPtEta","fPtEta",fgkNPtBins,binsPt,fgkNEtaBins,binsEta);
  fHistList->Add(fPtEta);
 
  fPtDCA2D = new TH2F("fPtDCA2D","fPtDCA2D",fgkNPtBins,binsPt,fgkNDCA2DBins,binsDCA2D);
  fHistList->Add(fPtDCA2D);
 
  fPtDCAZ = new TH2F("fPtDCAZ","fPtDCAZ",fgkNPtBins,binsPt,fgkNDCAZBins,binsDCAZ);
  fHistList->Add(fPtDCAZ);
 
  fPtNClustersTPC = new TH2F("fPtNClustersTPC","fPtNClustersTPC",fgkNPtBins,binsPt,fgkNNClustersTPCBins,binsNClustersTPC);
  fHistList->Add(fPtNClustersTPC);
 
  fPtNPointITS = new TH2F("fPtNPointITS","fPtNPointITS",fgkNPtBins,binsPt,fgkNNPointITSBins,binsNPointITS);
  fHistList->Add(fPtNPointITS);
 
  fPtChi2C = new TH2F("fPtChi2C","fPtChi2C",fgkNPtBins,binsPt,fgkNChi2CBins,binsChi2C);
  fHistList->Add(fPtChi2C);
 
  fPtNSigmaToVertex = new TH2F("fPtNSigmaToVertex","fPtNSigmaToVertex",fgkNPtBins,binsPt,fgkNNSigmaToVertexBins,binsNSigmaToVertex);
  fHistList->Add(fPtNSigmaToVertex);

  fPtRelUncertainty1Pt = new TH2F("fPtRelUncertainty1Pt","fPtRelUncertainty1Pt",fgkNPtBins,binsPt,fgkNRel1PtUncertaintyBins,binsRel1PtUncertainty);
  fHistList->Add(fPtRelUncertainty1Pt);

  fPtUncertainty1Pt = new TH2F("fPtUncertainty1Pt","fPtUncertainty1Pt",fgkNPtBins,binsPt,fgkNUncertainty1PtBins,binsUncertainty1Pt);
  fHistList->Add(fPtUncertainty1Pt);
 
  fPtChi2PerClusterTPC = new TH2F("fPtChi2PerClusterTPC","fPtChi2PerClusterTPC",fgkNPtBins,binsPt,fgkNChi2PerClusBins,binsChi2PerClus);
  fHistList->Add(fPtChi2PerClusterTPC);

  fPtNCrossedRows = new TH2F("fPtNCrossedRows","fPtNCrossedRows",fgkNPtBins,binsPt,fgkNNClustersTPCBins,binsNClustersTPC);
  fHistList->Add(fPtNCrossedRows);

  fPtNCrossedRowsNClusF = new TH2F("fPtNCrossedRowsNClusF","fPtNCrossedRowsNClusF",fgkNPtBins,binsPt,fgkNCrossedRowsNClusFBins,binsNCrossedRowsNClusF);
  fHistList->Add(fPtNCrossedRowsNClusF);
 
  fPtNCrRNCrRNClusF = new TH3F("fPtNCrRNCrRNClusF","fPtNCrRNCrRNClusF",fgkNPtBins,binsPt,fgkNNClustersTPCBins,binsNClustersTPC,fgkNCrossedRowsNClusFBins,binsNCrossedRowsNClusF);
  fHistList->Add(fPtNCrRNCrRNClusF);

  fPtSigmaY2 = new TH2F("fPtSigmaY2","fPtSigmaY2",fgkN1PtBins,bins1Pt,fgkNSigmaY2Bins,binsSigmaY2);
  fHistList->Add(fPtSigmaY2);

  fPtSigmaZ2 = new TH2F("fPtSigmaZ2","fPtSigmaZ2",fgkN1PtBins,bins1Pt,fgkNSigmaZ2Bins,binsSigmaZ2);
  fHistList->Add(fPtSigmaZ2);

  fPtSigmaSnp2 = new TH2F("fPtSigmaSnp2","fPtSigmaSnp2",fgkN1PtBins,bins1Pt,fgkNSigmaSnp2Bins,binsSigmaSnp2);
  fHistList->Add(fPtSigmaSnp2);

  fPtSigmaTgl2 = new TH2F("fPtSigmaTgl2","fPtSigmaTgl2",fgkN1PtBins,bins1Pt,fgkNSigmaTgl2Bins,binsSigmaTgl2);
  fHistList->Add(fPtSigmaTgl2);

  fPtSigma1Pt2 = new TH2F("fPtSigma1Pt2","fPtSigma1Pt2",fgkN1PtBins,bins1Pt,fgkNSigma1Pt2Bins,binsSigma1Pt2);
  fHistList->Add(fPtSigma1Pt2);

  fProfPtSigmaY2 = new TProfile("fProfPtSigmaY2","fProfPtSigmaY2",fgkN1PtBins,bins1Pt);
  fHistList->Add(fProfPtSigmaY2);

  fProfPtSigmaZ2 = new TProfile("fProfPtSigmaZ2","fProfPtSigmaZ2",fgkN1PtBins,bins1Pt);
  fHistList->Add(fProfPtSigmaZ2);

  fProfPtSigmaSnp2 = new TProfile("fProfPtSigmaSnp2","fProfPtSigmaSnp2",fgkN1PtBins,bins1Pt);
  fHistList->Add(fProfPtSigmaSnp2);

  fProfPtSigmaTgl2 = new TProfile("fProfPtSigmaTgl2","fProfPtSigmaTgl2",fgkN1PtBins,bins1Pt);
  fHistList->Add(fProfPtSigmaTgl2);

  fProfPtSigma1Pt2 = new TProfile("fProfPtSigma1Pt2","fProfPtSigma1Pt2",fgkN1PtBins,bins1Pt);
  fHistList->Add(fProfPtSigma1Pt2);

  fProfPtSigma1Pt = new TProfile("fProfPtSigma1Pt","fProfPtSigma1Pt;p_{T};#sigma(1/p_{T})",fgkNPtBins,binsPt);
  fHistList->Add(fProfPtSigma1Pt);

  fProfPtPtSigma1Pt = new TProfile("fProfPtPtSigma1Pt","fProfPtPtSigma1Pt;p_{T};p_{T}#sigma(1/p_{T})",fgkNPtBins,binsPt);
  fHistList->Add(fProfPtPtSigma1Pt);

  TH1::AddDirectory(oldStatus); 

  PostData(1, fHistList);

  if(binsPhi)               delete [] binsPhi;
  if(binsPt)                delete [] binsPt;
  if(binsNClustersTPC)      delete [] binsNClustersTPC;
  if(binsDCA2D)             delete [] binsDCA2D;
  if(binsDCAZ)              delete [] binsDCAZ;
  if(binsNPointITS)         delete [] binsNPointITS;
  if(binsNSigmaToVertex)    delete [] binsNSigmaToVertex;
  if(binsChi2C)             delete [] binsChi2C;
  if(binsEta)               delete [] binsEta;
  if(binsRel1PtUncertainty) delete [] binsRel1PtUncertainty;
  if(binsUncertainty1Pt)    delete [] binsUncertainty1Pt;
  if(binsChi2PerClus)       delete [] binsChi2PerClus;
  if(binsChi2PerClus)       delete [] binsNCrossedRowsNClusF;
  if(bins1Pt)               delete [] bins1Pt;
  if(binsSigmaY2)           delete [] binsSigmaY2;
  if(binsSigmaZ2)           delete [] binsSigmaZ2;
  if(binsSigmaSnp2)         delete [] binsSigmaSnp2;
  if(binsSigmaTgl2)         delete [] binsSigmaTgl2;
  if(binsSigma1Pt2)         delete [] binsSigma1Pt2;
}

//________________________________________________________________________
Bool_t AliPWG4HighPtTrackQA::SelectEvent() {
  //
  // Decide if event should be selected for analysis
  //

  // Checks following requirements:
  // - fEvent available
  // - trigger info from AliPhysicsSelection
  // - MCevent available
  // - number of reconstructed tracks > 1
  // - primary vertex reconstructed
  // - z-vertex < 10 cm
  // - centrality in case of PbPb

  Bool_t selectEvent = kTRUE;

  //fEvent object available?
  if (!fEvent) {
    AliDebug(2,Form("ERROR: fInputEvent not available\n"));
    fNEventReject->Fill("noAliVEvent",1);
    selectEvent = kFALSE;
    return selectEvent;
  }

  //Check if number of reconstructed tracks is larger than 1
  if(!fEvent->GetNumberOfTracks() || fEvent->GetNumberOfTracks()<2)  {
    fNEventReject->Fill("NTracks<2",1);
    selectEvent = kFALSE;
    return selectEvent;
  }

  //Check if vertex is reconstructed
  if(fDataType==kESD&&dynamic_cast<AliESDEvent*>(fEvent)) {
    fVtx = ((AliESDEvent*)fEvent)->GetPrimaryVertexSPD();

    if(!fVtx) {
      fNEventReject->Fill("noVTX",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
    
    if(!fVtx->GetStatus()) {
      fNEventReject->Fill("VtxStatus",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
    
    // Need vertex cut
    if(fVtx->GetNContributors()<2) {
      fNEventReject->Fill("NCont<2",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
    
    //Check if z-vertex < 10 cm
    double primVtx[3];
    fVtx->GetXYZ(primVtx);
    if(TMath::Sqrt(primVtx[0]*primVtx[0] + primVtx[1]*primVtx[1])>1. || TMath::Abs(primVtx[2]>10.)){
      fNEventReject->Fill("ZVTX>10",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
  }
  else if(fDataType==kAOD&&dynamic_cast<AliAODEvent*>(fEvent)) {
    const AliAODVertex *vtx = ((AliAODEvent*)fEvent)->GetPrimaryVertexSPD();
    if(!vtx) {
      fNEventReject->Fill("noVTX",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
    
    // Need vertex cut
    if(vtx->GetNContributors()<2) {
      fNEventReject->Fill("NCont<2",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
    
    //Check if z-vertex < 10 cm
    double primVtx[3];
    vtx->GetXYZ(primVtx);
    if(TMath::Sqrt(primVtx[0]*primVtx[0] + primVtx[1]*primVtx[1])>1. || TMath::Abs(primVtx[2]>10.)){
      fNEventReject->Fill("ZVTX>10",1);
      selectEvent = kFALSE;
      return selectEvent;
    }

  }

  //Centrality selection should only be done in case of PbPb
  if(IsPbPb()) {
    Float_t cent = 0.;
    if(fCentClass!=CalculateCentrality(fEvent) && fCentClass!=10) {
      fNEventReject->Fill("cent",1);
      selectEvent = kFALSE;
      return selectEvent;
    }
    else {
      if(fDataType==kESD) {
	if(dynamic_cast<AliESDEvent*>(fEvent)->GetCentrality()) {
	  cent = dynamic_cast<AliESDEvent*>(fEvent)->GetCentrality()->GetCentralityPercentile("V0M");
	}
      }
      else if(fDataType==kAOD) {
	if(dynamic_cast<AliAODEvent*>(fEvent)->GetHeader()->GetCentrality())
	  cent = dynamic_cast<AliAODEvent*>(fEvent)->GetHeader()->GetCentrality();
       }
      if(cent>90.) {
	fNEventReject->Fill("cent>90",1);
	selectEvent = kFALSE;
	return selectEvent;	
      }
      fh1Centrality->Fill(cent);
    }
  }
 
  return selectEvent;

}

//________________________________________________________________________
Int_t AliPWG4HighPtTrackQA::CalculateCentrality(AliVEvent *ev){
  if(fDataType==kESD)
    return CalculateCentrality(dynamic_cast<AliESDEvent*>(ev));
  else if(fDataType==kAOD)
    return CalculateCentrality(dynamic_cast<AliAODEvent*>(ev));
  else
    return 5;
}

//________________________________________________________________________
Int_t AliPWG4HighPtTrackQA::CalculateCentrality(AliESDEvent *esd){


  Float_t cent = 999;

  if(esd){
    if(esd->GetCentrality()){
      cent = esd->GetCentrality()->GetCentralityPercentile("V0M");
      if(fDebug>3) cout << "centrality: " << cent << endl;
    }
  }

  if(cent>80)return 4;
  if(cent>50)return 3;
  if(cent>30)return 2;
  if(cent>10)return 1;
  return 0;

}

//________________________________________________________________________
Int_t AliPWG4HighPtTrackQA::CalculateCentrality(AliAODEvent *aod){

  if(!aod)return 4;
  Float_t cent = aod->GetHeader()->GetCentrality();
  cout << "centrality: " << cent << endl;
  if(cent>80)return 4;
  if(cent>50)return 3;
  if(cent>30)return 2;
  if(cent>10)return 1;
  return 0;

}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::UserExec(Option_t *) {  
  // Main loop
  // Called for each event
  AliDebug(2,Form(">> AliPWG4HighPtTrackQA::UserExec \n"));
  
  fEvent = InputEvent();
  fESD = dynamic_cast<AliESDEvent*>(InputEvent());

  // All events without selection
  fNEventAll->Fill(0.);

  if(!SelectEvent()) {
    // Post output data
    PostData(1, fHistList);
    return;
  }


  //Need to keep track of selected events
  fNEventSel->Fill(0.);

  fVariables = new TArrayF(fNVariables);
  
  if(fDataType==kESD) DoAnalysisESD();
  if(fDataType==kAOD) DoAnalysisAOD();

  //Delete old fVariables
  if(fVariables) delete fVariables;

  // Post output data
  PostData(1, fHistList);

}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::DoAnalysisESD() {

  if(!fESD) {
    PostData(1, fHistList);
    return;
  }

  // ---- Get MC Header information (for MC productions in pThard bins) ----
  Double_t ptHard = 0.;
  Double_t nTrials = 1; // trials for MC trigger weight for real data
  
  AliMCEventHandler *eventHandlerMC = dynamic_cast<AliMCEventHandler*> (AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler());
  if (eventHandlerMC) {
    
    if(eventHandlerMC->MCEvent()){
      AliGenPythiaEventHeader*  pythiaGenHeader = GetPythiaEventHeader(eventHandlerMC->MCEvent());
      if(pythiaGenHeader){
	nTrials = pythiaGenHeader->Trials();
	ptHard  = pythiaGenHeader->GetPtHard();
	
	fh1PtHard->Fill(ptHard);
	fh1PtHardTrials->Fill(ptHard,nTrials);
	
	fh1Trials->Fill("#sum{ntrials}",fAvgTrials);
      }
    }
  }

  Int_t nTracks = fESD->GetNumberOfTracks();
  AliDebug(2,Form("nTracks ESD%d", nTracks));

  /*
    Variables to be put in fVariables
    0: pt
    1: phi
    2: eta
    3: dca2D
    4: dcaZ 
    5: nClustersTPC
    6: nPointITS   
    7: chi2C       
    8: nSigmaToVertex
    9: relUncertainty1Pt
    10: chi2PerClusterTPC
    11: #crossed rows
    12: (#crossed rows)/(#findable clusters)
    13: SigmaY2
    14: SigmaZ2
    15: SigmaSnp2
    16: SigmaTgl2
    17: Sigma1Pt2
  */

  for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    fh1NTracksAll->Fill(0.);

    //Get track for analysis
    AliESDtrack *track = 0x0;
    AliESDtrack *esdtrack = fESD->GetTrack(iTrack);
    if(!esdtrack) {
      fh1NTracksReject->Fill("noESDtrack",1);
      continue;
    }

    if(fTrackType==4) {
      FillSystematicCutHist(esdtrack);
      if (!(fTrackCuts->AcceptTrack(esdtrack))) {
	fh1NTracksReject->Fill("trackCuts",1);
	continue;
      }
    }

    if(fTrackType==1)
      track = AliESDtrackCuts::GetTPCOnlyTrack(fESD,esdtrack->GetID());
    else if(fTrackType==2 || fTrackType==4) {
      track = AliESDtrackCuts::GetTPCOnlyTrack(fESD,esdtrack->GetID());
      if(!track) {
	fh1NTracksReject->Fill("noTPConly",1);
	continue;
      }
      AliExternalTrackParam exParam;
      Bool_t relate = track->RelateToVertexTPC(fVtx,fESD->GetMagneticField(),kVeryBig,&exParam);
      if( !relate ) {
	fh1NTracksReject->Fill("relate",1);
    	delete track;
	continue;
      }
      track->Set(exParam.GetX(),exParam.GetAlpha(),exParam.GetParameter(),exParam.GetCovariance());
    }
    else
      track = esdtrack;
    
    if(!track) {
      if(fTrackType==1 || fTrackType==2 || fTrackType==4) delete track;
      continue;
    }

    if(fTrackType==2 || fTrackType==4) {
      //Cut on chi2 of constrained fit
      if(track->GetConstrainedChi2TPC() > fSigmaConstrainedMax*fSigmaConstrainedMax && fSigmaConstrainedMax>0.) {
	fh1NTracksReject->Fill("chi2",1);
	delete track;
	continue;
      }
    }

    if(fTrackType!=4)
      FillSystematicCutHist(track);

    fPtAll->Fill(track->Pt());

    if (!(fTrackCuts->AcceptTrack(track)) && fTrackType!=4) {
      fh1NTracksReject->Fill("trackCuts",1);
      if(fTrackType==1 || fTrackType==2) delete track;
      continue;
    }

    //Cut out laser tracks
    if(track->GetTPCsignal()<10) { //Cut on laser tracks
      fh1NTracksReject->Fill("laser",1);
      if(fTrackType==1 || fTrackType==2 || fTrackType==4) delete track;
      continue; 
    }

    fh1NTracksSel->Fill(0.);

    fVariables->Reset(0.);
      
    fVariables->SetAt(track->Pt(),0); 
    fVariables->SetAt(track->Phi(),1); 
    fVariables->SetAt(track->Eta(),2); 

    Float_t dca2D = 0.;
    Float_t dcaz  = 0.;
    if(fTrackType==0) {       //Global
      track->GetImpactParameters(dca2D,dcaz);
    }
    else if(fTrackType==1 || fTrackType==2 || fTrackType==4) {  //TPConly
      track->GetImpactParametersTPC(dca2D,dcaz);
    }
    fVariables->SetAt(dca2D,3);
    fVariables->SetAt(dcaz,5);

    fVariables->SetAt((float)track->GetTPCNcls(),5);

    Int_t nPointITS = 0;
    UChar_t itsMap = track->GetITSClusterMap();
    for (Int_t i=0; i < 6; i++) {
      if (itsMap & (1 << i))
	nPointITS ++;
    }
    fVariables->SetAt((float)nPointITS,6);
    Float_t chi2C = (float)track->GetConstrainedChi2();
    if(fTrackType==1 || fTrackType==2 || fTrackType==4)
      chi2C = (float)track->GetConstrainedChi2TPC();
    fVariables->SetAt(chi2C,7);
    fVariables->SetAt(fTrackCuts->GetSigmaToVertex(track),8);// Calculates the number of sigma to the vertex for a track.
  
    fVariables->SetAt(TMath::Sqrt(track->GetSigma1Pt2())*fVariables->At(0),9);
  
    if(fVariables->At(5)>0.) fVariables->SetAt(track->GetTPCchi2()/fVariables->At(5),10);
    
    //cout << "#crossed rows (1): " << track->GetTPCClusterInfo(1) << endl;
    //cout << "#crossed rows (2): " << track->GetTPCClusterInfo(2) << endl;
    fVariables->SetAt(track->GetTPCClusterInfo(2,1),11); //#crossed rows
    Float_t crossedRowsTPCNClsF = track->GetTPCClusterInfo(2,0);
    //if(track->GetTPCNclsF()>0.) crossedRowsTPCNClsF = fVariables->At(11)/track->GetTPCNclsF();
    fVariables->SetAt(crossedRowsTPCNClsF,12);//(#crossed rows)/(#findable clusters)
    fVariables->SetAt(track->GetSigmaY2(),13);
    fVariables->SetAt(track->GetSigmaZ2(),14);
    fVariables->SetAt(track->GetSigmaSnp2(),15);
    fVariables->SetAt(track->GetSigmaTgl2(),16);
    fVariables->SetAt(track->GetSigma1Pt2(),17);
    
    FillHistograms();
  
    //      int mult = fTrackCuts->CountAcceptedTracks(fESD);

    if(fTrackType==1  || fTrackType==2 || fTrackType==4) delete track;
    
  }//track loop

}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::DoAnalysisAOD() {

  AliAODEvent *aod = dynamic_cast<AliAODEvent*>(fEvent);
  if(!aod)return;
  for (Int_t iTrack = 0; iTrack < fEvent->GetNumberOfTracks(); iTrack++) {

    AliAODTrack *aodtrack = aod->GetTrack(iTrack);
    if( !aodtrack->TestFilterMask(fFilterMask) ) continue;

    fVariables->Reset(0.);

    fVariables->SetAt(aodtrack->Pt(),0);
    fVariables->SetAt(aodtrack->Phi(),1);
    fVariables->SetAt(aodtrack->Eta(),2);

    Double_t dca[2] = {1e6,1e6};
    Double_t covar[3] = {1e6,1e6,1e6};
    if(aodtrack->PropagateToDCA(fEvent->GetPrimaryVertex(),fEvent->GetMagneticField(),100.,dca,covar)) {
      fVariables->SetAt(dca[0],3);
      fVariables->SetAt(dca[1],4);
    }

    fVariables->SetAt((float)aodtrack->GetTPCNcls(),5);
    fVariables->SetAt((float)aodtrack->GetITSNcls(),6);
    fVariables->SetAt(aodtrack->Chi2perNDF(),7);
    fVariables->SetAt(0.,8);
    fVariables->SetAt(0.,9);
    fVariables->SetAt(aodtrack->Chi2perNDF(),10);
    fVariables->SetAt(GetTPCClusterInfo(aodtrack,2),11);
    Float_t crossedRowsTPCNClsF = 0.;
    if(aodtrack->GetTPCNclsF()>0.) crossedRowsTPCNClsF = fVariables->At(11)/aodtrack->GetTPCNclsF();
    fVariables->SetAt(crossedRowsTPCNClsF,12);
    
    fPtAll->Fill(fVariables->At(0));

    FillHistograms();

  }

}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::FillHistograms() {

  fPtSel->Fill(fVariables->At(0));
  fPtPhi->Fill(fVariables->At(0),fVariables->At(1));
  fPtEta->Fill(fVariables->At(0),fVariables->At(2));
  fPtDCA2D->Fill(fVariables->At(0),fVariables->At(3));
  fPtDCAZ->Fill(fVariables->At(0),fVariables->At(4));
  fPtNClustersTPC->Fill(fVariables->At(0),fVariables->At(5));
  fPtNPointITS->Fill(fVariables->At(0),fVariables->At(6));
  if(fDataType==kESD) {
    fPtChi2C->Fill(fVariables->At(0),fVariables->At(7));
    fPtNSigmaToVertex->Fill(fVariables->At(0),fVariables->At(8));
    fPtRelUncertainty1Pt->Fill(fVariables->At(0),fVariables->At(9));
    fPtUncertainty1Pt->Fill(fVariables->At(0),fVariables->At(0)/fVariables->At(9));
    fPtSigmaY2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(13)));
    fPtSigmaZ2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(14)));
    fPtSigmaSnp2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(15)));
    fPtSigmaTgl2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(16)));
    fPtSigma1Pt2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(17)));

    fProfPtSigmaY2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(13)));
    fProfPtSigmaZ2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(14)));
    fProfPtSigmaSnp2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(15)));
    fProfPtSigmaTgl2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(16)));
    fProfPtSigma1Pt2->Fill(1./fVariables->At(0),TMath::Sqrt(fVariables->At(17)));
    fProfPtSigma1Pt->Fill(fVariables->At(0),TMath::Sqrt(fVariables->At(17)));
    fProfPtPtSigma1Pt->Fill(fVariables->At(0),fVariables->At(0)*TMath::Sqrt(fVariables->At(17)));
  }
  fPtChi2PerClusterTPC->Fill(fVariables->At(0),fVariables->At(10));
  fPtNCrossedRows->Fill(fVariables->At(0),fVariables->At(11));
  fPtNCrossedRowsNClusF->Fill(fVariables->At(0),fVariables->At(12));
  fPtNCrRNCrRNClusF->Fill(fVariables->At(0),fVariables->At(11),fVariables->At(12));
}

//________________________________________________________________________
Bool_t AliPWG4HighPtTrackQA::PythiaInfoFromFile(const char* currFile,Float_t &fXsec,Float_t &fTrials){
  //
  // get the cross section and the trails either from pyxsec.root or from pysec_hists.root
  // This is to called in Notify and should provide the path to the AOD/ESD file
  // Copied from AliAnalysisTaskJetSpectrum2
  //

  TString file(currFile);  
  fXsec = 0;
  fTrials = 1;

  if(file.Contains("root_archive.zip#")){
    Ssiz_t pos1 = file.Index("root_archive",12,TString::kExact);
    Ssiz_t pos = file.Index("#",1,pos1,TString::kExact);
    file.Replace(pos+1,20,"");
  }
  else {
    // not an archive take the basename....
    file.ReplaceAll(gSystem->BaseName(file.Data()),"");
  }
  //  Printf("%s",file.Data());
   

  TFile *fxsec = TFile::Open(Form("%s%s",file.Data(),"pyxsec.root")); // problem that we cannot really test the existance of a file in a archive so we have to lvie with open error message from root
  if(!fxsec){
    // next trial fetch the histgram file
    fxsec = TFile::Open(Form("%s%s",file.Data(),"pyxsec_hists.root"));
    if(!fxsec){
	// not a severe condition but inciate that we have no information
      return kFALSE;
    }
    else{
      // find the tlist we want to be independtent of the name so use the Tkey
      TKey* key = (TKey*)fxsec->GetListOfKeys()->At(0); 
      if(!key){
	fxsec->Close();
	return kFALSE;
      }
      TList *list = dynamic_cast<TList*>(key->ReadObj());
      if(!list){
	fxsec->Close();
	return kFALSE;
      }
      fXsec = ((TProfile*)list->FindObject("h1Xsec"))->GetBinContent(1);
      fTrials  = ((TH1F*)list->FindObject("h1Trials"))->GetBinContent(1);
      fxsec->Close();
    }
  } // no tree pyxsec.root
  else {
    TTree *xtree = (TTree*)fxsec->Get("Xsection");
    if(!xtree){
      fxsec->Close();
      return kFALSE;
    }
    UInt_t   ntrials  = 0;
    Double_t  xsection  = 0;
    xtree->SetBranchAddress("xsection",&xsection);
    xtree->SetBranchAddress("ntrials",&ntrials);
    xtree->GetEntry(0);
    fTrials = ntrials;
    fXsec = xsection;
    fxsec->Close();
  }
  return kTRUE;
}
//________________________________________________________________________
Bool_t AliPWG4HighPtTrackQA::Notify()
{
  //
  // Implemented Notify() to read the cross sections
  // and number of trials from pyxsec.root
  // Copied from AliAnalysisTaskJetSpectrum2
  // 

  TTree *tree = AliAnalysisManager::GetAnalysisManager()->GetTree();
  Float_t xsection = 0;
  Float_t ftrials  = 1;

  fAvgTrials = 1;
  if(tree){
    TFile *curfile = tree->GetCurrentFile();
    if (!curfile) {
      Error("Notify","No current file");
      return kFALSE;
    }
    if(!fh1Xsec||!fh1Trials){
      //      Printf("%s%d No Histogram fh1Xsec",(char*)__FILE__,__LINE__);
      return kFALSE;
    }
    PythiaInfoFromFile(curfile->GetName(),xsection,ftrials);
    fh1Xsec->Fill("<#sigma>",xsection);
    // construct a poor man average trials 
    Float_t nEntries = (Float_t)tree->GetTree()->GetEntries();
    if(ftrials>=nEntries && nEntries>0.)fAvgTrials = ftrials/nEntries;
  }  
  return kTRUE;
}

//________________________________________________________________________
AliGenPythiaEventHeader*  AliPWG4HighPtTrackQA::GetPythiaEventHeader(AliMCEvent *mcEvent){
  
  if(!mcEvent)return 0;
  AliGenEventHeader* genHeader = mcEvent->GenEventHeader();
  AliGenPythiaEventHeader* pythiaGenHeader = dynamic_cast<AliGenPythiaEventHeader*>(genHeader);
  if(!pythiaGenHeader){
    // cocktail ??
    AliGenCocktailEventHeader* genCocktailHeader = dynamic_cast<AliGenCocktailEventHeader*>(genHeader);
    
    if (!genCocktailHeader) {
      //      AliWarningGeneral(Form(" %s:%d",(char*)__FILE__,__LINE__),"Unknown header type (not Pythia or Cocktail)");
      //      AliWarning(Form("%s %d: Unknown header type (not Pythia or Cocktail)",(char*)__FILE__,__LINE__));
      return 0;
    }
    TList* headerList = genCocktailHeader->GetHeaders();
    for (Int_t i=0; i<headerList->GetEntries(); i++) {
      pythiaGenHeader = dynamic_cast<AliGenPythiaEventHeader*>(headerList->At(i));
      if (pythiaGenHeader)
        break;
    }
    if(!pythiaGenHeader){
      AliWarningGeneral(Form(" %s:%d",(char*)__FILE__,__LINE__),"Pythia event header not found");
      return 0;
    }
  }
  return pythiaGenHeader;

}

//_______________________________________________________________________
Float_t AliPWG4HighPtTrackQA::GetTPCClusterInfo(AliAODTrack *tr,Int_t nNeighbours/*=3*/, Int_t type/*=0*/, Int_t row0, Int_t row1) const
{
  //MV: copied from AliESDtrack since method is not available in AliAODTrack

  //
  // TPC cluster information
  // type 0: get fraction of found/findable clusters with neighbourhood definition
  //      1: findable clusters with neighbourhood definition
  //      2: found clusters
  //
  // definition of findable clusters:
  //            a cluster is defined as findable if there is another cluster
  //           within +- nNeighbours pad rows. The idea is to overcome threshold
  //           effects with a very simple algorithm.
  //

  TBits fTPCClusterMap = tr->GetTPCClusterMap(); 
  if (type==2) return fTPCClusterMap.CountBits();

  Int_t found=0;
  Int_t findable=0;
  Int_t last=-nNeighbours;

  for (Int_t i=row0; i<row1; ++i){
    //look to current row
    if (fTPCClusterMap[i]) {
      last=i;
      ++found;
      ++findable;
      continue;
    }
    //look to nNeighbours before
    if ((i-last)<=nNeighbours) {
      ++findable;
      continue;
    }
    //look to nNeighbours after
    for (Int_t j=i+1; j<i+1+nNeighbours; ++j){
      if (fTPCClusterMap[j]){
        ++findable;
        break;
      }
    }
  }
  if (type==1) return findable;

  if (type==0){
    Float_t fraction=0;
    if (findable>0)
      fraction=(Float_t)found/(Float_t)findable;
    else
      fraction=0;
    return fraction;
  }
  return 0;  // undefined type - default value
}

void AliPWG4HighPtTrackQA::FillSystematicCutHist(AliESDtrack *track) {

  fSystTrackCuts->Fill("noCut",1);
  if(TMath::Abs(track->Eta())>0.9) 
    fSystTrackCuts->Fill("eta",1);
  if(track->Pt()<0.15 || track->Pt()>1e10) 
    fSystTrackCuts->Fill("0.15<pT<1e10",1);
  if(track->GetKinkIndex(0)>0)
    fSystTrackCuts->Fill("kink",1);
  if(track->GetTPCclusters(0)<70)
    fSystTrackCuts->Fill("NClusterTPC",1);
  if(track->GetTPCclusters(0)>0.) {
    if(track->GetTPCchi2()/Float_t(track->GetTPCclusters(0))>4.)
      fSystTrackCuts->Fill("Chi2PerNClusTPC",1);
  }
  
  Float_t dcaToVertexXY = 0.;
  Float_t dcaToVertexZ  = 0.;
  track->GetImpactParameters(dcaToVertexXY,dcaToVertexZ);
  Float_t fCutMaxDCAToVertexXY = 2.4;
  Float_t fCutMaxDCAToVertexZ  = 3.2;
  Float_t dcaToVertex = TMath::Sqrt(dcaToVertexXY*dcaToVertexXY/fCutMaxDCAToVertexXY/fCutMaxDCAToVertexXY + dcaToVertexZ*dcaToVertexZ/fCutMaxDCAToVertexZ/fCutMaxDCAToVertexZ);
  if(dcaToVertex>1.) 
    fSystTrackCuts->Fill("DCA2D",1);
  
}

//________________________________________________________________________
void AliPWG4HighPtTrackQA::Terminate(Option_t *)
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.

}

#endif
