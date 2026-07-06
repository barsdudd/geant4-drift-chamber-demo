#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <TFile.h>
#include <TTree.h>
#include <TLinearFitter.h>
#include <TMath.h>
#include <TEllipse.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TH1F.h>

struct PlaneGeom { double z, pitch, offset, angle; };
struct Hit { int pid, wid; double drift, w_wire; };
struct Pair { Hit h1, h2; };

void DrawEvent(int evId, const std::map<int, PlaneGeom>& geom, const std::vector<Hit>& allHits, 
               const std::vector<std::vector<double>>& allTrackParams) {
  
  int nTracks = allTrackParams.size();
  if (nTracks == 0) return;
  
  int canvasWidth = nTracks * 1500;
  int canvasHeight = 1200;
  
  TCanvas *c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c_display");
  
  if (!c || c->GetWw() != canvasWidth) {
    if (c) delete c;
    c = new TCanvas("c_display", "Multi-Track Analysis", canvasWidth, canvasHeight);
  }
  
  c->cd();
  c->Clear();
  c->Divide(nTracks, 1);
  
  for (int t = 0; t < nTracks; ++t) {
    c->cd(t + 1);
    
    double ax = allTrackParams[t][0], bx = allTrackParams[t][1];
    double ay = allTrackParams[t][2], by = allTrackParams[t][3];
    
    double z_min = 5500.0, z_max = 5700.0;
    double x_start = ax * z_min + bx;
    double x_end   = ax * z_max + bx;
    double x_min_view = std::min(x_start, x_end) - 50.0;
    double x_max_view = std::max(x_start, x_end) + 50.0;
    
    TH1F *frame = gPad->DrawFrame(5490, x_min_view, 5710, x_max_view);
    frame->SetTitle(Form("Event %d - Track %d;Z [mm];X [mm]", evId, t + 1));
    
    for (auto const& [id, p] : geom) {
      double y_track = ay * p.z + by;
      for (int w = 0; w < 160; ++w) {
	double w_pos = w * p.pitch + p.offset + (p.pitch / 2.0);
	double x_wire_slice = (w_pos - y_track * std::sin(p.angle)) / std::cos(p.angle);
	if (x_wire_slice < x_min_view - 10 || x_wire_slice > x_max_view + 10) continue;
        
	TEllipse *dot = new TEllipse(p.z, x_wire_slice, 0.8, 0.8);
	dot->SetFillColor(kGray); dot->SetLineColor(kGray);
	dot->Draw();
      }
      TLine *l = new TLine(p.z, x_min_view, p.z, x_max_view);
      l->SetLineStyle(3); l->SetLineColor(kGray+1); l->Draw();
    }
    
    for (const auto& h : allHits) {
      double z = geom.at(h.pid).z, theta = geom.at(h.pid).angle;
      double y_t = ay * z + by;
      double x_w = (h.w_wire - y_t * std::sin(theta)) / std::cos(theta);
      double rx = h.drift / std::cos(theta);
      
      if (x_w < x_min_view - 20 || x_w > x_max_view + 20) continue;
      
      TEllipse *drift = new TEllipse(z, x_w, rx, rx);
      drift->SetFillStyle(0); drift->SetLineColor(kGreen+2); drift->Draw();
      TEllipse *wire = new TEllipse(z, x_w, 2.0, 2.0);
      wire->SetFillColor(kBlack); wire->Draw();
    }
    
    TLine *line = new TLine(5490, ax*5490 + bx, 5710, ax*5710 + bx);
    line->SetLineColor(kRed); line->SetLineWidth(3); line->Draw();
  }
  c->Update();
  gSystem->ProcessEvents();
}

void TrackRecon(bool draw = true) {
    std::map<int, PlaneGeom> geom;
    std::ifstream f("geometry.csv");
    if (!f.is_open()) return;
    std::string l;
    while (std::getline(f, l)) {
        if (l[0] == '#' || l.empty()) continue;
        int id; double z, p, o, ang;
        if (sscanf(l.c_str(), "%d, %lf, %lf, %lf, %lf", &id, &z, &p, &o, &ang) == 5)
            geom[id] = {z, p, o, ang * TMath::Pi() / 180.0};
    }

    TFile *file = TFile::Open("../build/ChamberHits.root");
    if (!file || file->IsZombie()) return;
    TTree *tree = (TTree*)file->Get("Hits");

    int eventID; std::vector<int> *pID = 0, *wID = 0; std::vector<double> *dDist = 0;
    tree->SetBranchAddress("EventID", &eventID);
    tree->SetBranchAddress("PlaneID", &pID);
    tree->SetBranchAddress("WireID", &wID);
    tree->SetBranchAddress("DriftDistance", &dDist);

    TLinearFitter *fitter = new TLinearFitter(4, "x0++x1++x2++x3");

    for (int i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        if (!pID || pID->empty()) continue;

        std::map<int, std::vector<Hit>> hits; std::vector<Hit> allHits;
        for (size_t j = 0; j < pID->size(); ++j) {
            int p = (*pID)[j];
            double ww = (double)(*wID)[j] * geom[p].pitch + geom[p].offset + (geom[p].pitch / 2.0);
            Hit h = {p, (*wID)[j], (*dDist)[j], ww};
            hits[p].push_back(h); allHits.push_back(h);
        }

        auto getPairs = [&](int p1, int p2) {
            std::vector<Pair> pairs;
            if (hits.count(p1) && hits.count(p2))
                for (auto& h1 : hits[p1]) for (auto& h2 : hits[p2])
                    if (std::abs(h1.wid - h2.wid) <= 2) pairs.push_back({h1, h2});
            return pairs;
        };

        auto uPairs = getPairs(0, 1); auto xPairs = getPairs(2, 3); auto vPairs = getPairs(4, 5);
        std::vector<std::vector<double>> eventTracks;

        for (auto& xp : xPairs) {
            for (auto& vp : vPairs) {
                double ax_pre = (xp.h2.w_wire - xp.h1.w_wire) / (geom[3].z - geom[2].z);
                double bx_pre = xp.h1.w_wire - ax_pre * geom[2].z;
                double av_v = (vp.h2.w_wire - vp.h1.w_wire) / (geom[5].z - geom[4].z);
                double bv_v = vp.h1.w_wire - av_v * geom[4].z;
                double ay_pre = (ax_pre * std::cos(geom[0].angle) - av_v) / std::sin(geom[0].angle);
                double by_pre = (bx_pre * std::cos(geom[0].angle) - bv_v) / std::sin(geom[0].angle);
                double u_pred = (ax_pre * geom[0].z + bx_pre) * std::cos(geom[0].angle) + (ay_pre * geom[0].z + by_pre) * std::sin(geom[0].angle);

                for (auto& up : uPairs) {
                    if (std::abs(up.h1.w_wire - u_pred) > 100.0) continue;
                    Hit road[6] = {up.h1, up.h2, xp.h1, xp.h2, vp.h1, vp.h2};
                    double minChi2 = 1e12; std::vector<double> bestPars(4);
                    for (int comb = 0; comb < 64; ++comb) {
                        fitter->ClearPoints();
                        for (int j = 0; j < 6; ++j) {
                            double side = (comb & (1 << j)) ? 1.0 : -1.0;
                            double w_hit = road[j].w_wire + side * road[j].drift;
                            double z = geom[road[j].pid].z, ang = geom[road[j].pid].angle;
                            double basis[4] = {z * std::cos(ang), std::cos(ang), z * std::sin(ang), std::sin(ang)};
                            fitter->AddPoint(basis, w_hit, 0.4);
                        }
                        if (fitter->Eval() == 0 && fitter->GetChisquare()/2.0 < minChi2) {
                            minChi2 = fitter->GetChisquare()/2.0;
                            for(int k=0; k<4; k++) bestPars[k] = fitter->GetParameter(k);
                        }
                    }
                    if (minChi2 < 30.0) eventTracks.push_back(bestPars);
                }
            }
        }

        if (draw && !eventTracks.empty()) {
            DrawEvent(eventID, geom, allHits, eventTracks);
            std::cout << "Event " << eventID << ": Found " << eventTracks.size() << " track(s). Press Enter..." << std::flush;
            std::string dummy; std::getline(std::cin, dummy);
        }
    }
    file->Close();
}
