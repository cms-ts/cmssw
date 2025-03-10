#ifndef TIGHTID_SF_H
#define TIGHTID_SF_H

#include <cmath> // For fabs (absolute value)

// Function to calculate the tight ID scale factor
float tightID_SF(float eta, float pt) {
  float weight = 1.0; // Default weight

  if (fabs(eta) > 0 && fabs(eta) <= 0.35) {
    if (pt > 15 && pt <= 25) {
      weight = 1.001;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.997;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.984;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.983;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.986;
    }
  } else if (fabs(eta) > 0.35 && fabs(eta) <= 0.6) {
    if (pt > 15 && pt <= 25) {
      weight = 0.966;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.997;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.995;
    } else if (pt > 45 && pt <= 60) {
      weight = 1.000;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.993;
    }
  } else if (fabs(eta) > 0.6 && fabs(eta) <= 1.2) {
    if (pt > 15 && pt <= 25) {
      weight = 0.978;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.982;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.990;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.982;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.959;
    }
  } else if (fabs(eta) > 1.2 && fabs(eta) <= 1.6) {
    if (pt > 15 && pt <= 25) {
      weight = 1.002;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.998;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.999;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.996;
    } else if (pt > 60 && pt <= 200) {
      weight = 1.002;
    }
  } else if (fabs(eta) > 1.6 && fabs(eta) <= 2.1) {
    if (pt > 15 && pt <= 25) {
      weight = 0.988;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.999;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.993;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.997;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.980;
    }
  } else if (fabs(eta) > 2.1 && fabs(eta) <= 2.4) {
    if (pt > 15 && pt <= 25) {
      weight = 0.976;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.976;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.988;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.993;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.970;
    }
  }
  return weight;
}

#endif
