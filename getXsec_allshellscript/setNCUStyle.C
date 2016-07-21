#include "TStyle.h"
#include "TPad.h"

// fixOverlay: Redraws the axis
// void fixOverlay() {
//   gPad->RedrawAxis();
// }

void setNCUStyle(bool gridOn=false) {
  TStyle* ncuStyle = new TStyle("ncuStyle","Style for P-NCU");

// For the canvas:
  ncuStyle->SetCanvasBorderMode(0);
  ncuStyle->SetCanvasColor(kWhite);
  ncuStyle->SetCanvasDefH(600); //Height of canvas
  ncuStyle->SetCanvasDefW(600); //Width of canvas
  ncuStyle->SetCanvasDefX(0);   //POsition on screen
  ncuStyle->SetCanvasDefY(0);

// For the Pad:
  ncuStyle->SetPadBorderMode(0);
  // ncuStyle->SetPadBorderSize(Width_t size = 1);
  ncuStyle->SetPadColor(kWhite);
  ncuStyle->SetPadGridX(gridOn);
  ncuStyle->SetPadGridY(gridOn);
  ncuStyle->SetGridColor(0);
  ncuStyle->SetGridStyle(3);
  ncuStyle->SetGridWidth(1);

// For the frame:
  ncuStyle->SetFrameBorderMode(0);
  ncuStyle->SetFrameBorderSize(1);
  ncuStyle->SetFrameFillColor(0);
  ncuStyle->SetFrameFillStyle(0);
  ncuStyle->SetFrameLineColor(1);
  ncuStyle->SetFrameLineStyle(1);
  ncuStyle->SetFrameLineWidth(3);

// For the Legend:
  ncuStyle->SetLegendBorderSize(0);
  ncuStyle->SetLegendFillColor(0);
  ncuStyle->SetLegendFont(42);
  //ncuStyle->SetLegendFont(62);
  
// For the histo:
  // ncuStyle->SetHistFillColor(1);
  // ncuStyle->SetHistFillStyle(0);
  ncuStyle->SetHistLineColor(1);
  ncuStyle->SetHistLineStyle(0);
  ncuStyle->SetHistLineWidth(3);
  // ncuStyle->SetLegoInnerR(Float_t rad = 0.5);
  // ncuStyle->SetNumberContours(Int_t number = 20);

  ncuStyle->SetEndErrorSize(2);
  // ncuStyle->SetErrorMarker(20);
  //ncuStyle->SetErrorX(0.);
  
  ncuStyle->SetMarkerStyle(20);
  
//For the fit/function:
  ncuStyle->SetOptFit(1);
  ncuStyle->SetFitFormat("5.4g");
  ncuStyle->SetFuncColor(2);
  ncuStyle->SetFuncStyle(1);
  ncuStyle->SetFuncWidth(1);

//For the date:
  ncuStyle->SetOptDate(0);
  // ncuStyle->SetDateX(Float_t x = 0.01);
  // ncuStyle->SetDateY(Float_t y = 0.01);

// For the statistics box:
  ncuStyle->SetOptFile(0);
  ncuStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  ncuStyle->SetStatColor(kWhite);
  ncuStyle->SetStatFont(42);
  ncuStyle->SetStatFontSize(0.025);
  ncuStyle->SetStatTextColor(1);
  ncuStyle->SetStatFormat("6.4g");
  ncuStyle->SetStatBorderSize(1);
  ncuStyle->SetStatH(0.1);
  ncuStyle->SetStatW(0.15);
  // ncuStyle->SetStatStyle(Style_t style = 1001);
  // ncuStyle->SetStatX(Float_t x = 0);
  // ncuStyle->SetStatY(Float_t y = 0);

// Margins:
  ncuStyle->SetPadTopMargin(0.08);
  ncuStyle->SetPadBottomMargin(0.13);
  ncuStyle->SetPadLeftMargin(0.13);
  ncuStyle->SetPadRightMargin(0.04);

// For the Global title:

  ncuStyle->SetOptTitle(0);
  ncuStyle->SetTitleFont(62);
  ncuStyle->SetTitleColor(1);
  ncuStyle->SetTitleTextColor(1);
  ncuStyle->SetTitleFillColor(10);
  ncuStyle->SetTitleFontSize(0.05);
  // ncuStyle->SetTitleH(0); // Set the height of the title box
  // ncuStyle->SetTitleW(0); // Set the width of the title box
  // ncuStyle->SetTitleX(0); // Set the position of the title box
  // ncuStyle->SetTitleY(0.985); // Set the position of the title box
  // ncuStyle->SetTitleStyle(Style_t style = 1001);
  // ncuStyle->SetTitleBorderSize(2);

// For the axis titles:

  ncuStyle->SetTitleColor(1, "XYZ");
  ncuStyle->SetTitleSize(0.06, "XYZ");
  // ncuStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // ncuStyle->SetTitleYSize(Float_t size = 0.02);
  // the following commands are not doing any thing
  // ncuStyle->SetTitleOffset(1.5, "X"); // Another way to set the Offset
  // ncuStyle->SetTitleOffset(1.5, "Y"); // Another way to set the Offset
  // ncuStyle->SetTitleFont(42, "XYZ");

// For the axis labels:
  ncuStyle->SetLabelColor(1, "XYZ");
  // the following command is not doing any thing
  // ncuStyle->SetLabelFont(42, "XYZ");
  ncuStyle->SetLabelOffset(0.007, "XYZ");
  ncuStyle->SetLabelSize(0.05, "XYZ");

// For the axis:

  ncuStyle->SetAxisColor(1, "XYZ");
  ncuStyle->SetStripDecimals(kFALSE);
  ncuStyle->SetTickLength(0.03, "XYZ");
  ncuStyle->SetNdivisions(510, "XYZ");
  ncuStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  ncuStyle->SetPadTickY(1);

// Change for log plots:
  ncuStyle->SetOptLogx(0);
  ncuStyle->SetOptLogy(0);
  ncuStyle->SetOptLogz(0);

// Postscript options:
  ncuStyle->SetPaperSize(20.,20.);
  // ncuStyle->SetLineScalePS(Float_t scale = 3);
  // ncuStyle->SetLineStyleString(Int_t i, const char* text);
  // ncuStyle->SetHeaderPS(const char* header);
  // ncuStyle->SetTitlePS(const char* pstitle);

  // ncuStyle->SetBarOffset(Float_t baroff = 0.5);
  // ncuStyle->SetBarWidth(Float_t barwidth = 0.5);
  // ncuStyle->SetPaintTextFormat(const char* format = "g");
  // ncuStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // ncuStyle->SetTimeOffset(Double_t toffset);
  // ncuStyle->SetHistMinimumZero(kTRUE);

  ncuStyle->SetHatchesLineWidth(5);
  ncuStyle->SetHatchesSpacing(0.05);

  ncuStyle->cd();

}
