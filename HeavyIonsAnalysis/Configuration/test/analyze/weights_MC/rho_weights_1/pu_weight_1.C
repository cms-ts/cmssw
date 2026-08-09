#include "TFile.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>
#include "TLegend.h"
#include "TRatioPlot.h"
#include "TLatex.h"
#include "../../helpers.h"
#include "../tdrstyle.C"

void pu_weight_1(const char * collision_type = "ppref24", int after_flag = 0, int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

    std::string histo_name = "h_nVtx";
    std::string x_title = "Number of Primary Vertices";
    std::string y_title = "";

    setTDRStyle();

    // Build the dynamic run tag
    TString run_tag;
    if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
    else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);

    TString name_output = "ppref";
    double Lumi = getLumiFromSummary("../../brilcalc_Collisions2024_ppref.csv"); 

    // Open MC file
    TString MC_file_name = (after_flag == 0) ? "./output_" + name_output + "_mu_MC_pu_weights" + run_tag + ".root"
                                             : "../vz_weights_2/output_" + name_output + "_mu_MC_pu_weights_after" + run_tag + ".root";
    TFile* file_mc = TFile::Open(MC_file_name.Data(), "READ");
    TDirectoryFile* dir_mc = (TDirectoryFile*)file_mc->Get(name_output+"/Muons");
    if (!dir_mc) {
        std::cerr << "Error: Directory not found in MC file. Did the analyzer crash?" << std::endl;
        return;
    }
    TH1D* h_mc = (TH1D*)dir_mc->Get(histo_name.c_str());
    
    // Open Data file
    TFile* file_data = TFile::Open("../../plot/output_" + name_output + "_mu_data" + run_tag + ".root", "READ");
    TDirectoryFile* dir_data = (TDirectoryFile*)file_data->Get(name_output+"/Muons");
    TH1D* h_data = (TH1D*)dir_data->Get(histo_name.c_str());

    // Normalize both to unity (1.0) so we just get the shape differences
    if (h_mc->Integral() > 0) h_mc->Scale(1.0 / h_mc->Integral(0, h_mc->GetNbinsX()+1));
    if (h_data->Integral() > 0) h_data->Scale(1.0 / h_data->Integral(0, h_data->GetNbinsX()+1));

    h_mc->SetFillColor(TColor::GetColor("#e42536"));
    h_mc->SetLineColor(h_mc->GetFillColor());
    h_data->SetMarkerStyle(20);
    h_data->SetLineColor(kBlack);
    h_data->SetMarkerColor(kBlack);

    TCanvas *c = new TCanvas("c", "c", 800, 800);
    c->cd();

    TRatioPlot *h_ratio = new TRatioPlot(h_data, h_mc, "divsym");
    h_ratio->SetH1DrawOpt("EX0");
    h_ratio->SetH2DrawOpt("HIST");
    h_ratio->Draw();
    h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
    h_ratio->GetLowerRefGraph()->SetMinimum(0.4);
    h_ratio->GetLowerRefGraph()->SetMaximum(2.0);
    h_ratio->GetLowerRefYaxis()->SetTitle("Data/MC");
    std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
    h_ratio->SetGridlines(onlyOneGridline);

    h_data->SetMaximum(h_data->GetMaximum() * 1.5);
    h_data->GetXaxis()->SetTitle(x_title.c_str());
    h_data->GetYaxis()->SetTitle(y_title.c_str());

    TPad *pad = h_ratio->GetUpperPad();
    pad->cd();
    h_mc->Draw("HISTsame");
    h_data->Draw("EsameX0");

    TLegend* legend = new TLegend(0.66, 0.7, 0.88, 0.85);
    legend->SetBorderSize(0);
    legend->AddEntry(h_data, "Data", "PE");
    legend->AddEntry(h_mc, "DY + 2j", "f");
    legend->SetTextFont(42);
    legend->SetTextColor(kBlack);
    legend->SetTextSize(0.036);
    legend->Draw();

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.06); latex->SetTextFont(61);
    latex->DrawLatexNDC(0.1, 0.92, "CMS");
    latex->SetTextSize(0.045); latex->SetTextFont(52);
    latex->DrawLatexNDC(0.19, 0.92, "Preliminary");
    latex->SetTextSize(0.05); latex->SetTextFont(42);
    latex->DrawLatexNDC(0.54, 0.92, TString::Format("pp %.2f pb^{-1} (5.36 TeV)", Lumi));

    c->Update();

    // Create and save weight histogram
    TH1D* h_weight_pu = (TH1D*)h_data->Clone("h_weight_pu");
    h_weight_pu->Divide(h_mc);
    
    // Prevent crazy weights in empty tails
    for (int i = 1; i <= h_weight_pu->GetNbinsX(); i++) {
        if (h_weight_pu->GetBinContent(i) <= 0 || h_weight_pu->GetBinContent(i) > 5.0) {
            h_weight_pu->SetBinContent(i, 1.0);
        }
    }

    if (after_flag == 0) {
        TString out_name = "weight_" + name_output + "_pu" + run_tag + ".root";
        TFile* file_weight_pu = new TFile(out_name, "RECREATE");
        h_weight_pu->Write("h_weight_pu");
        file_weight_pu->Close();
        std::cout << "Weight file created: " << out_name << std::endl;
    }

    std::string is_bef_or_aft = (after_flag == 1) ? "_after.pdf" : "_before.pdf";
    c->Print((histo_name + "_" + name_output.Data() + run_tag.Data() + is_bef_or_aft).c_str());
}
