#include <TApplication.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <TWbox.h>
#include <TTUBE.h>
#include <THYPE.h>
#include <TText.h>
#include <TBRIK.h>
#include <TNode.h>
#include <TMarker3DBox.h>
#include <TH2.h>
#include <TGeometry.h>
#include <TView3D.h>
#include <TAxis3D.h>
#include <TGeometry.h>
#include <TGeoManager.h>
#include <TCanvas.h>
#include <sstream>
#include <TStyle.h>
#include <TSystem.h>
#include <TGraph.h>
#include <TLine.h>
#include <TView.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <TFile.h>
#include <TTree.h>
#include "const.h"
#include <unistd.h>

#define NUM_LAYER 24
#define NUM_CH 16
#define MaxNumPad 136
#define NUM_BOARD 522
#define NUM_SECTOR 6

int main(int argc,char *argv[]){  
  int run_num;
  int totevent;
  printf("run_num.?: ");
  scanf("%d",&run_num);
  printf("num_event.?: ");
  scanf("%d",&totevent);
  
  int evedis_cnt =0;
  TApplication app( "app", &argc, argv );
  float peakAdc[10000];
  float peakTdc[10000];
  int chNum[10000];
  int boardNum[10000];
  int nHitTpc;
  float PeakAdc[NUM_BOARD][NUM_CH][20];
  float PeakTdc[NUM_BOARD][NUM_CH][20] ;
  float Pad_adc[NUM_SECTOR][NUM_LAYER][MaxNumPad];
  float Pad_tdc[NUM_SECTOR][NUM_LAYER][MaxNumPad];
  int mHit[NUM_BOARD][NUM_CH];
  char iFile[100];
  float velocity = 2.6; //[cm/us]
  float TimeToZ = 25./1000.*velocity; //[cm/ch]
  float Zoffset = 78.6; //[cm/ch] 
  
  sprintf(iFile,"/np1b/v07/kobaryo/LEPS2ANA/leps2ana/bin/rt/%d.root",run_num);
  TCanvas *c1 = new TCanvas("Event Display","Event Display",800,1200);
  c1->SetFillColor(kBlue+4);
  c1->Divide(2,2);

  
  TString pdf;
  pdf.Form("pdf/display%d.pdf",run_num);
  c1->Print(pdf + "[", "pdf");    
  gSystem->Load("libGeom");  
  TBRIK *brik= new TBRIK("BRIK","BRIK","void",0.,0.,0.);
  
  TNode *node1 = new TNode("NODE1","NODE1","BRIK\
",0,0,0);
  
  THYPE *hype  = new THYPE("HYPE","HYPE","void",6,60,37,0);
  hype->SetLineColor(kYellow+1);
  hype->SetNumberOfDivisions(6);
  hype->SetLineWidth(0.01);  
  THYPE *hype2  = new THYPE("HYPE2","HYPE2","void",6,60,0,0);
  hype2->SetLineColor(kYellow+1);
  hype2->SetNumberOfDivisions(6);
  hype2->SetLineWidth(1);
    THYPE *hype3  = new THYPE("HYPE3","HYPE3","void",6,60,0,0);
  hype3->SetLineColor(kYellow+1);
  hype3->SetNumberOfDivisions(6);
  hype3->SetLineWidth(1);
  THYPE *hype4  = new THYPE("HYPE4","HYPE4","void",6,6,37,0);
  hype4->SetLineColor(8);
  hype4->SetLineWidth(1);  
  TNode *node2  = new TNode("NODE2","NODE2","HYPE",0,0,-37.);
  TNode *node3  = new TNode("NODE3","NODE3","HYPE2",0,0,-74.);
  TNode *node4  = new TNode("NODE4","NODE4","HYPE3",0,0,0.);
  // TNode *node5  = new TNode("NODE5","NODE5","HYPE4",0,0,-35.);
  
  
  TFile*data=TFile::Open(iFile);
  TTree*rawTPC=(TTree*)data->Get("rawTPC");
  rawTPC ->SetBranchAddress("peakAdc",peakAdc);
  rawTPC ->SetBranchAddress("peakTdc",peakTdc);
  rawTPC ->SetBranchAddress("boardNum",boardNum);
  rawTPC ->SetBranchAddress("nHitTpc",&nHitTpc);
  rawTPC ->SetBranchAddress("chNum",chNum);
  printf("openfile\n");
  int  trkevent=0;
  printf("totevent : %d",totevent);
  for (int event=0;event<totevent;event++){
    rawTPC->GetEntry(event);
    // rawTPC->GetEntry(95);
    for(int s=0;s<NUM_SECTOR;s++){
      for(int i =0;i<NUM_LAYER;i++){
        for(int j=0;j<MaxNumPad;j++){
          Pad_adc[s][i][j]=-10000;
          Pad_tdc[s][i][j]=-10000;
        }
      }
    }
    for(int i =0;i<NUM_BOARD;i++ ){
      for(int j=0;j<16;j++){
        mHit[i][j]=0;
        for(int k=0;k<20;k++){
          PeakAdc[i][j][k]=-1000;
        }    
      }
    }
    //printf("nHit : %d\n",nHitTpc);    
    for(int l=0;l<nHitTpc;l++){
      mHit[boardNum[l]][chNum[l]]++;
      if( PeakAdc[boardNum[l]][chNum[l]][0]<peakAdc[l]){
        PeakAdc[boardNum[l]][chNum[l]][0]=peakAdc[l];
        PeakTdc[boardNum[l]][chNum[l]][0]=peakTdc[l];
      }
    }

    //b_id//
    std::ifstream board_id("use_board_20190705.txt");
    int nb=0;
    int b_id[NUM_BOARD];  
    while(board_id >> b_id[nb]){
      // if(event==0)printf("nb = %d NUM_BOARD = %d \n",nb,NUM_BOARD);
      nb++;
    }
    ifstream fin("b_config_2019.txt");
    int npad = 0;
    int a1[5000],a2[5000],a3[5000],a4[5000];
    while( fin >> a1[npad] >> a2[npad] >> a3[npad] >> a4[npad] )npad++;
    for(int j=0;j<NUM_BOARD;j++){ 
      if(b_id[j]<20000){
        for(int i=0;i<npad;i++){
          if(a3[i]==(b_id[j]-10000)){
            int layer,padnum,ich;
            ich=a4[i]-3;
            layer=a1[i];
            padnum=a2[i];
            Pad_adc[0][layer][padnum]=PeakAdc[j][ich][0];
            Pad_tdc[0][layer][padnum]=PeakTdc[j][ich][0];
          }
        }
      }
      if(b_id[j]>20000&&b_id[j]<30000){
        for(int i=0;i<npad;i++){
          if(a3[i]==(b_id[j]-20000)){
            int layer,padnum,ich;
            ich=a4[i]-3;
            layer=a1[i];
            padnum=a2[i];
            Pad_adc[1][layer][padnum]=PeakAdc[j][ich][0];
            Pad_tdc[1][layer][padnum]=PeakTdc[j][ich][0];
	    for(int k=1;k<5;k++){  
              if(Pad_adc[1][layer][padnum]<PeakAdc[j][ich][k]){
                Pad_adc[1][layer][padnum]=PeakAdc[j][ich][k];
                Pad_tdc[1][layer][padnum]=PeakTdc[j][ich][k];
	      }
	    } 
	  }
        }
      }

    if(b_id[j]>30000&&b_id[j]<40000){
        for(int i=0;i<npad;i++){
          if(a3[i]==(b_id[j]-30000)){
            int layer,padnum,ich;
            ich=a4[i]-3;
            layer=a1[i];
            padnum=a2[i];
            Pad_adc[2][layer][padnum]=PeakAdc[j][ich][0];
            Pad_tdc[2][layer][padnum]=PeakTdc[j][ich][0];
	    for(int k=1;k<5;k++){  
              if(Pad_adc[2][layer][padnum]<PeakAdc[j][ich][k]){
                Pad_adc[2][layer][padnum]=PeakAdc[j][ich][k];
                Pad_tdc[2][layer][padnum]=PeakTdc[j][ich][k];
	      }
	    } 
	  }
        }
      }
    if(b_id[j]>40000&&b_id[j]<50000){
        for(int i=0;i<npad;i++){
          if(a3[i]==(b_id[j]-40000)){
            int layer,padnum,ich;
            ich=a4[i]-3;
            layer=a1[i];
            padnum=a2[i];
            Pad_adc[3][layer][padnum]=PeakAdc[j][ich][0];
            Pad_tdc[3][layer][padnum]=PeakTdc[j][ich][0];	      
	    
	    for(int k=1;k<5;k++){  
              if(Pad_adc[3][layer][padnum]<PeakAdc[j][ich][k]){
                Pad_adc[3][layer][padnum]=PeakAdc[j][ich][k];
                Pad_tdc[3][layer][padnum]=PeakTdc[j][ich][k];	      
		//		printf("bbbbbbbbbbbb\n");	
	      }
	    } 
	  }
        }
      }
    if(b_id[j]>50000&&b_id[j]<60000){
        for(int i=0;i<npad;i++){
          if(a3[i]==(b_id[j]-50000)){
            int layer,padnum,ich;
            ich=a4[i]-3;
            layer=a1[i];
            padnum=a2[i];
            Pad_adc[4][layer][padnum]=PeakAdc[j][ich][0];
            Pad_tdc[4][layer][padnum]=PeakTdc[j][ich][0];	      
	    
	    for(int k=1;k<5;k++){  
              if(Pad_adc[4][layer][padnum]<PeakAdc[j][ich][k]){
                Pad_adc[4][layer][padnum]=PeakAdc[j][ich][k];
                Pad_tdc[4][layer][padnum]=PeakTdc[j][ich][k];
	      }
	    } 
	  }
        }
      }
    if(b_id[j]>60000){
        for(int i=0;i<npad;i++){
          if(a3[i]==(b_id[j]-60000)){
            int layer,padnum,ich;
            ich=a4[i]-3;
            layer=a1[i];
            padnum=a2[i];
            Pad_adc[5][layer][padnum]=PeakAdc[j][ich][0];
            Pad_tdc[5][layer][padnum]=PeakTdc[j][ich][0];
	    for(int k=1;k<5;k++){  
              if(Pad_adc[5][layer][padnum]<PeakAdc[j][ich][k]){
                Pad_adc[5][layer][padnum]=PeakAdc[j][ich][k];
                Pad_tdc[5][layer][padnum]=PeakTdc[j][ich][k];
	      }
	    } 
	  }
        }
      }
    }
    int hitpad =0;
    for(int s =0;s<NUM_SECTOR;s++){
      for(int i =0; i<NUM_LAYER;i++){
        for(int j=0; j<NUM_PAD[i];j++){
          if(Pad_adc[s][i][j]>1000)hitpad++;
        }
      }
    }
    if(hitpad>100){
    printf("*** event number :: %d *** \n",event);
    printf("trkevent %d  hitpads %d\n",trkevent,hitpad);  
      if(trkevent%4==0&&trkevent>0){
	c1->Print(pdf, "pdf");
	usleep(1000000);
	c1->Clear();
	c1->Divide(2,2);
      }
      c1->cd(trkevent%4+1);
      char title[50];
      sprintf( title,"event : %d",event );
      TView3D *view =(TView3D*)TView3D::CreateView(1);
      TAxis3D *axis = new TAxis3D();
      node1->cd();
      node1->Draw("same");
      view->SetRange(-70,-70,-100,70,70,-10);
      Int_t test;
      view->SetView(182,65,88,test);
      TText *t = new TText(0.1,0.5,title);
      t->SetTextColor(0);
      
      t->Draw("same");
      axis->Draw();
      axis->SetAxisColor(0,"x");
      axis->SetLabelColor(kBlue+4,"x");
      axis->SetAxisColor(0,"z");
      axis->SetLabelColor(0,"z");
      axis->SetAxisColor(kBlue-10,"y");
      axis->SetLabelColor(kBlue-8,"y");
      trkevent++;
      for(int s=0;s<6;s++){
	for(int i =0; i<NUM_LAYER;i++){
	  for(int j=0; j<NUM_PAD[i];j++){
	    float x1=(j-NUM_PAD[i]/2)*0.46;
	    float x2 =(j+1-NUM_PAD[i]/2)*0.46;
	    float x[6],y[6];
	    x[0]=(x1+x2)/2;
	    y[0]=R_LAYER[i];
	    float pi=3.141592;
	    x[s]=cos(s*pi/3)*x[0]-sin(s*pi/3)*y[0];
	    y[s]=sin(s*pi/3)*x[0]+cos(s*pi/3)*y[0];
	    TMarker3DBox *e[6];
	    float r1=0.23;
	    float r2=0.5;
	    e[s] = new TMarker3DBox(x[s],y[s],-100, r1,r2,0,0,s*60);
	    e[s]->SetLineWidth(0.002);
	    
	    if(Pad_adc[s][i][j]<-1000){    
	    e[s] -> SetLineColor(38);
	  }else if(-1000<=Pad_adc[s][i][j] && Pad_adc[s][i][j]<-10){
	    e[s] -> SetLineColor(0);
	  }else if(-10<=Pad_adc[s][i][j] && Pad_adc[s][i][j]<50){
	    e[s] -> SetLineColor(92);
          }else if(50<=Pad_adc[s][i][j] && Pad_adc[s][i][j]<1000){
	    TMarker3DBox *hit= new TMarker3DBox(x[s],y[s],Pad_tdc[s][i][j]*TimeToZ-Zoffset,r1,r2,0.1,0,s*60);
	    hit->Draw();
	    hit->SetLineColor(kMagenta-9);
	    e[s] -> SetLineColor(96);
          }else if(1000<=Pad_adc[s][i][j] && Pad_adc[s][i][j]<1200){
	    TMarker3DBox *hit= new TMarker3DBox(x[s],y[s],Pad_tdc[s][i][j]*TimeToZ-Zoffset,r1,r2,0.1,0,s*60);
	    hit->Draw();
	    hit->SetLineColor(6);
	    e[s]-> SetLineColor(97);
          }else if(1200<=Pad_adc[s][i][j] && Pad_adc[s][i][j]<2000){
	    TMarker3DBox *hit= new TMarker3DBox(x[s],y[s],Pad_tdc[s][i][j]*TimeToZ-Zoffset,r1,r2,0.1,0,s*60);
	    hit->SetLineColor(6);
	    hit->Draw();
	    e[s] -> SetLineColor(100);
          }else if(2000<=Pad_adc[s][i][j]&& Pad_adc[s][i][j]<16000 ){
	    TMarker3DBox *hit= new TMarker3DBox(x[s],y[s],Pad_tdc[s][i][j]*TimeToZ-Zoffset,r1,r2,0.1,0,s*60);
	    hit->SetLineColor(6);
	    hit->Draw();
	    e[s] -> SetLineColor(100);
          }else if(18000<=Pad_adc[s][i][j] ){
	    e[s] -> SetLineColor(80);
	  }
	  e[s]->Draw();
	  }//pad   
	}//layer      
      }//sector 
      evedis_cnt++;
    }
    c1->Update();
  }
  c1->Print(pdf + "]", "pdf");
  return 0;
}

  
