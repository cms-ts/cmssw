#include "TColor.h"

//color palette recommendations at https://cms-analysis.docs.cern.ch/guidelines/plotting/colors/
Int_t my_color_ten (int i_col) {
    int r1 = 63, g1 = 144, b1 = 218; // blue, Hex code: #3f90da
    int r2 = 255, g2 = 169, b2 = 14; // light orange, Hex code: #ffa90e
    int r3 = 189, g3 = 31, b3 = 1; // red, Hex code: #bd1f01
    int r4 = 148, g4 = 164, b4 = 162; // grey, Hex code: #94a4a2
    int r5 = 131, g5 = 45, b5 = 182; // violet, Hex code: #832db6
    int r6 = 169, g6 = 107, b6 = 89; // brown, Hex code: #a96b59
    int r7 = 231, g7 = 99, b7 = 0; // orange, Hex code: #e76300
    int r8 = 185, g8 = 172, b8 = 112; // light green, Hex code: #b9ac70
    int r9 = 113, g9 = 117, b9 = 129; // dark grey, Hex code: #717581
    int r10 = 146, g10 = 218, b10 = 221; // light blue, Hex code: #92dadd
    
    //Create TColor objects
    Int_t c_blue = TColor::GetFreeColorIndex();
    auto color_blue = new TColor(c_blue, r1/255., g1/255., b1/255.);
    Int_t c_light_orange = TColor::GetFreeColorIndex();
    auto color_light_orange = new TColor(c_light_orange, r2/255., g2/255., b2/255.);
    Int_t c_red = TColor::GetFreeColorIndex();
    auto color_red = new TColor(c_red, r3/255., g3/255., b3/255.);
    Int_t c_grey = TColor::GetFreeColorIndex();
    auto color_grey = new TColor(c_grey, r4/255., g4/255., b4/255.);
    Int_t c_violet = TColor::GetFreeColorIndex();
    auto color_violet = new TColor(c_violet, r5/255., g5/255., b5/255.);
    Int_t c_brown = TColor::GetFreeColorIndex();
    auto color_brown = new TColor(c_brown, r6/255., g6/255., b6/255.);
    Int_t c_orange = TColor::GetFreeColorIndex();
    auto color_orange = new TColor(c_orange, r7/255., g7/255., b7/255.);
    Int_t c_light_green = TColor::GetFreeColorIndex();
    auto color_light_green = new TColor(c_light_green, r8/255., g8/255., b8/255.);
    Int_t c_dark_grey = TColor::GetFreeColorIndex();
    auto color_dark_grey = new TColor(c_dark_grey, r9/255., g9/255., b9/255.);
    Int_t c_light_blue = TColor::GetFreeColorIndex();
    auto color_light_blue = new TColor(c_light_blue, r10/255., g10/255., b10/255.);
    
    if (i_col == 1) return c_blue;
    if (i_col == 2) return c_light_orange;
    if (i_col == 3) return c_red;
    if (i_col == 4) return c_grey;
    if (i_col == 5) return c_violet;
    if (i_col == 6) return c_brown;
    if (i_col == 7) return c_orange;
    if (i_col == 8) return c_light_green;
    if (i_col == 9) return c_dark_grey;
    if (i_col == 10) return c_light_blue;
    //default black
    else return 1;
    
};

Int_t my_color_six (int i_col) {
    int r1 = 87, g1 = 144, b1 = 252; // blue, Hex code: #5790fc
    int r2 = 248, g2 = 156, b2 = 32; // light orange, Hex code: #f89c20
    int r3 = 228, g3 = 37, b3 = 54; // red, Hex code: #e42536
    int r4 = 150, g4 = 74, b4 = 139; // violet, Hex code: #964a8b
    int r5 = 156, g5 = 156, b5 = 161; // grey, Hex code: #9c9ca1
    int r6 = 122, g6 = 33, b6 = 221; // purple, Hex code: #7a21dd

    //Create TColor objects
    Int_t c_blue = TColor::GetFreeColorIndex();
    auto color_blue = new TColor(c_blue, r1/255., g1/255., b1/255.);
    Int_t c_light_orange = TColor::GetFreeColorIndex();
    auto color_light_orange = new TColor(c_light_orange, r2/255., g2/255., b2/255.);
    Int_t c_red = TColor::GetFreeColorIndex();
    auto color_red = new TColor(c_red, r3/255., g3/255., b3/255.);
    Int_t c_violet = TColor::GetFreeColorIndex();
    auto color_violet = new TColor(c_violet, r4/255., g4/255., b4/255.);
    Int_t c_grey = TColor::GetFreeColorIndex();
    auto color_grey = new TColor(c_grey, r5/255., g5/255., b5/255.);
    Int_t c_purple = TColor::GetFreeColorIndex();
    auto color_purple = new TColor(c_purple, r6/255., g6/255., b6/255.);

    if (i_col == 1) return c_blue;
    if (i_col == 2) return c_light_orange;
    if (i_col == 3) return c_red;
    if (i_col == 4) return c_violet;
    if (i_col == 5) return c_grey;
    if (i_col == 6) return c_purple;

    //default black
    else return 1;
    
};

