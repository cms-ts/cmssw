void plot_nVtx() {
    // 1. Open files
    TFile *f_data = TFile::Open("output_ppref_mu_data_ptZ40_Inf.root", "READ");
    TFile *f_mc   = TFile::Open("output_ppref_mu_MC_signal_ptZ40_Inf.root", "READ");

    // 2. Get histograms (adjust path if your MC directory is named differently)
    TH1D *h_data = (TH1D*)f_data->Get("ppref/Muons/h_nVtx");
    TH1D *h_mc   = (TH1D*)f_mc->Get("ppref/Muons/h_nVtx");

    // 3. Normalize to unity
    if (h_data->Integral() > 0) h_data->Scale(1.0 / h_data->Integral());
    if (h_mc->Integral() > 0)   h_mc->Scale(1.0 / h_mc->Integral());

    // 4. Style
    h_data->SetMarkerStyle(20);
    h_data->SetMarkerColor(kBlack);
    h_data->SetLineColor(kBlack);
    
    h_mc->SetMarkerStyle(21);
    h_mc->SetMarkerColor(kBlue);
    h_mc->SetLineColor(kBlue);

    // 5. Draw
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    h_data->SetTitle("Pileup Check;Number of Primary Vertices (nVtx);Normalized to unity");
    h_data->Draw("EP");
    h_mc->Draw("EP SAME");

    TLegend *leg = new TLegend(0.6, 0.7, 0.85, 0.85);
    leg->AddEntry(h_data, "Data", "lp");
    leg->AddEntry(h_mc, "MC (signal)", "lp");
    leg->Draw();

    std::cout << "Average nVtx (Data): " << h_data->GetMean() << std::endl;
    std::cout << "Average nVtx (MC): "   << h_mc->GetMean()   << std::endl;

    c1->SaveAs("nVtx_overlay.pdf");
}
