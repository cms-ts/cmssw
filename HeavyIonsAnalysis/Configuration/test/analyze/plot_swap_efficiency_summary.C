#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TString.h"
#include "TStyle.h"
#include <vector>
#include <iostream>
#include "TEfficiency.h"

void plot_single_case(TString eff_filename, TString out_name, bool is_pp, TString title_label, TString mc_truth_filename = "") {
    TFile* fIn = TFile::Open(eff_filename, "READ");
    if (!fIn || fIn->IsZombie()) {
        std::cerr << "Error: Cannot open " << eff_filename << std::endl;
        return;
    }

    TFile* fTruth = nullptr;
    if (mc_truth_filename != "") {
        fTruth = TFile::Open(mc_truth_filename, "READ");
        if (!fTruth || fTruth->IsZombie()) {
            std::cout << "Warning: Could not open MC Truth file " << mc_truth_filename << std::endl;
            fTruth = nullptr;
        }
    }

    TCanvas* c1 = new TCanvas(out_name, out_name, 800, 800);
    c1->cd();
    c1->SetLeftMargin(0.15);
    c1->SetBottomMargin(0.12);

    TLegend* leg = new TLegend(0.4, 0.15, 0.88, 0.45);
    leg->SetHeader(title_label);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.025);

    std::vector<std::pair<double, double>> bins;
    if (is_pp) {
        bins = {{0.0, 5.0}}; 
    } else {
        bins = {{0.0, 5.0}, {5.0, 10.0}, {10.0, 20.0}, {20.0, 30.0}, {30.0, 100.0}};
    }

    int colors[] = {kBlue+1, kRed+1, kGreen+2, kMagenta+2, kOrange+7, kCyan+2, kBlack};
    bool first = true;
    bool truth_legend_added = false;

    for (size_t i = 0; i < bins.size(); ++i) {
        TString s_min = Form("%.1f", bins[i].first); s_min.ReplaceAll(".", "p");
        TString s_max = Form("%.1f", bins[i].second); s_max.ReplaceAll(".", "p");

        TString h_name = Form("h_cdf_cen_%s_%s", s_min.Data(), s_max.Data());
        TString f_name = Form("fit_cen_%s_%s", s_min.Data(), s_max.Data());

        TH1D* h = (TH1D*)fIn->Get(h_name);
        TF1* fit = (TF1*)fIn->Get(f_name);

        if (!h || !fit) continue;

        // Data-Driven Styling
        h->SetMarkerColor(colors[i % 7]);
        h->SetLineColor(colors[i % 7]);
        h->SetMarkerStyle(20); // Solid circles
        h->SetMarkerSize(1.2);

        fit->SetLineColor(colors[i % 7]);
        fit->SetLineWidth(2);

        if (first) {
            h->SetTitle(""); 
            h->GetXaxis()->SetTitle("Jet p_{T} [GeV]");
            h->GetYaxis()->SetTitle("Signal jet finding efficiency");
            h->GetXaxis()->SetTitleSize(0.045);
            h->GetYaxis()->SetTitleSize(0.045);
            h->GetXaxis()->SetLabelSize(0.04);
            h->GetYaxis()->SetLabelSize(0.04);
            h->GetYaxis()->SetTitleOffset(1.5);
            h->GetYaxis()->SetRangeUser(0.8, 1.0); 
            h->GetXaxis()->SetRangeUser(30, 80); 
            h->SetStats(0);
            h->Draw("EP");
            first = false;
        } else {
            h->Draw("EP SAME");
        }
        fit->Draw("SAME");

        TString label = is_pp ? "pp (inclusive)" : Form("PbPb %.1f-%.1f%%", bins[i].first, bins[i].second);
        leg->AddEntry(h, label + " (Data-Driven)", "p");
        // --- Plot True MC Efficiency if file is provided ---
        if (fTruth) {
            TString dir_name = is_pp ? "ppref/Muons" : "HI/Muons";
            
            // 1. Total Efficiency (Open Circles)
            TH2D* h2_tot = (TH2D*)fTruth->Get(dir_name + "/h2_gen_pt_total_vs_cen");
            TH2D* h2_mat = (TH2D*)fTruth->Get(dir_name + "/h2_gen_pt_matched_vs_cen");

            int binX_min = (bins[i].first * 2) + 1;
            int binX_max = (bins[i].second * 2);
            if (is_pp) { binX_min = 1; binX_max = 200; } 
            
            if (h2_tot && h2_mat) {
                TH1D* h_tot = h2_tot->ProjectionY(Form("tot_%zu", i), binX_min, binX_max);
                TH1D* h_mat = h2_mat->ProjectionY(Form("mat_%zu", i), binX_min, binX_max);
                
                TH1D* h_true_eff = (TH1D*)h_mat->Clone(Form("true_eff_%zu", i));
                // --- NEW: Clamp floating point rounding errors ---
                for (int b = 1; b <= h_mat->GetNbinsX(); ++b) {
                    if (h_mat->GetBinContent(b) > h_tot->GetBinContent(b)) {
                        h_mat->SetBinContent(b, h_tot->GetBinContent(b));
                    }
                }
                // -------------------------------------------------
                // Use TEfficiency for rigorous weighted MC error bars
                if (TEfficiency::CheckConsistency(*h_mat, *h_tot)) {
                    TEfficiency* pEff = new TEfficiency(*h_mat, *h_tot);
                    for (int b = 1; b <= h_true_eff->GetNbinsX(); ++b) {
                        h_true_eff->SetBinContent(b, pEff->GetEfficiency(b));
                        // Take the larger of the asymmetric errors to display safely on a TH1
                        double err = std::max(pEff->GetEfficiencyErrorUp(b), pEff->GetEfficiencyErrorLow(b));
                        h_true_eff->SetBinError(b, err);
                    }
                    delete pEff;
                }
                h_true_eff->SetMarkerStyle(24); // Open circles
                h_true_eff->SetMarkerSize(1.5);
                h_true_eff->SetMarkerColor(colors[i % 7]);
                h_true_eff->SetLineColor(colors[i % 7]);
//                h_true_eff->Draw("EP SAME");

                if (!truth_legend_added) {
//                    leg->AddEntry(h_true_eff, "MC Total Efficiency (Smearing + Swaps)", "p");
                }
            }

            // 2. Pure Swap Efficiency (Open Squares)
            TH2D* h2_swap_den = (TH2D*)fTruth->Get(dir_name + "/h2_swap_pure_den_vs_cen");
            TH2D* h2_swap_num = (TH2D*)fTruth->Get(dir_name + "/h2_swap_pure_num_vs_cen");

            if (h2_swap_den && h2_swap_num) {
                TH1D* h_sden = h2_swap_den->ProjectionY(Form("sden_%zu", i), binX_min, binX_max);
                TH1D* h_snum = h2_swap_num->ProjectionY(Form("snum_%zu", i), binX_min, binX_max);
                
                TH1D* h_pure_swap_eff = (TH1D*)h_snum->Clone(Form("pure_swap_eff_%zu", i));
                // --- NEW: Clamp floating point rounding errors ---
                for (int b = 1; b <= h_snum->GetNbinsX(); ++b) {
                    if (h_snum->GetBinContent(b) > h_sden->GetBinContent(b)) {
                        h_snum->SetBinContent(b, h_sden->GetBinContent(b));
                    }
                }
                // -------------------------------------------------
                // Use TEfficiency for rigorous weighted MC error bars
                if (TEfficiency::CheckConsistency(*h_snum, *h_sden)) {
                    TEfficiency* pEffSwap = new TEfficiency(*h_snum, *h_sden);
                    for (int b = 1; b <= h_pure_swap_eff->GetNbinsX(); ++b) {
                        h_pure_swap_eff->SetBinContent(b, pEffSwap->GetEfficiency(b));
                        double err = std::max(pEffSwap->GetEfficiencyErrorUp(b), pEffSwap->GetEfficiencyErrorLow(b));
                        h_pure_swap_eff->SetBinError(b, err);
                    }
                    delete pEffSwap;
                }
                h_pure_swap_eff->SetMarkerStyle(25); // Open squares
                h_pure_swap_eff->SetMarkerSize(1.5);
                h_pure_swap_eff->SetMarkerColor(colors[i % 7]);
                h_pure_swap_eff->SetLineColor(colors[i % 7]);
                h_pure_swap_eff->Draw("EP SAME");

                if (!truth_legend_added) {
                    leg->AddEntry(h_pure_swap_eff, "MC Pure Swap Efficiency (Truth)", "p");
                    truth_legend_added = true; // Set to true after adding both to legend
                }
                // Save the truth efficiency to a file for the main macro to use as a systematic
                TString truth_filename = eff_filename; 
                truth_filename.ReplaceAll(".root", "_Truth.root"); // e.g., swap_efficiency_PbPb_MC_Truth.root

                TFile* fTruthOut = new TFile(truth_filename, "UPDATE");
                h_pure_swap_eff->SetName(Form("truth_cen_%s_%s", s_min.Data(), s_max.Data()));
                h_pure_swap_eff->Write("", TObject::kOverwrite);
                fTruthOut->Close();
                // --------------------------------------------------
            }
        }
    }

    // Add Fit Legend Entry just once at the end
    TF1* dummy_fit = new TF1("dummy", "1", 0, 1);
    dummy_fit->SetLineColor(kBlack);
    dummy_fit->SetLineWidth(2);
    leg->AddEntry(dummy_fit, "Gompertz Fit (Data-Driven)", "l");

    leg->Draw();

    TLatex* latex = new TLatex(); 
    latex->SetNDC();
    latex->SetTextSize(0.06); 
    latex->SetTextColor(kBlack);
    latex->SetTextFont(61);
    // Align with the left margin of 0.15
    latex->DrawLatex(0.15, 0.92, "CMS"); 

    TLatex* latex1 = new TLatex();
    latex1->SetNDC();
    latex1->SetTextSize(0.045); 
    latex1->SetTextColor(kBlack);
    latex1->SetTextFont(52);
    latex1->DrawLatex(0.28, 0.92, "Preliminary");

    c1->SaveAs(out_name + ".pdf");
    
    if (fTruth) fTruth->Close();
    fIn->Close();
}

void plot_swap_efficiency_summary(int isPbPb_int = 1, const char* run_tag = "_Cen0_30_ptZ40_Inf", const char* prefix = "HI") {
    bool isPbPb = (bool)isPbPb_int;
    
    // Dynamically reconstruct the path to the nominal MC file
    TString nominal_mc_path = Form("plot/output_%s_mu_MC_signal%s.root", prefix, run_tag);

    if (!isPbPb) {
        // 1. pp Data
        plot_single_case("swap_efficiency_ppref.root", "plot_swap_eff_pp_Data", true, "pp Data");
        // 2. pp MC (Overlays True MC and generates _Truth.root)
        plot_single_case("swap_efficiency_ppref_MC.root", "plot_swap_eff_pp_MC", true, "pp MC", nominal_mc_path);
    } else {
        // 3. PbPb Data
        plot_single_case("swap_efficiency_PbPb.root", "plot_swap_eff_PbPb_Data", false, "PbPb Data");
        // 4. PbPb MC (Overlays True MC and generates _Truth.root)
        plot_single_case("swap_efficiency_PbPb_MC.root", "plot_swap_eff_PbPb_MC", false, "PbPb MC", nominal_mc_path);
    }
}
