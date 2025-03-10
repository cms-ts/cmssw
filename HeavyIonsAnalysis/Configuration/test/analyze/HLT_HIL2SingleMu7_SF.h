#ifndef HLT_HIL2SINGLEMU7_SF_H
#define HLT_HIL2SINGLEMU7_SF_H

#include <cmath> // For fabs (absolute value)

// Function to calculate the tight ID scale factor
float HLT_HIL2SingleMu7_SF(float eta, float pt) {
  float weight = 1.0; // Default weight

  if (fabs(eta) > 0 && fabs(eta) <= 0.25) {
    if (pt > 15 && pt <= 25) {
      weight = 0.981;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.994;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.984;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.988;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.991;
    }
  } else if (fabs(eta) > 0.25 && fabs(eta) <= 0.35) {
    if (pt > 15 && pt <= 25) {
      weight = 1.004;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.975;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.966;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.972;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.933;
    }
  } else if (fabs(eta) > 0.35 && fabs(eta) <= 0.6) {
    if (pt > 15 && pt <= 25) {
      weight = 1.004;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.999;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.995;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.990;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.998;
    }
  } else if (fabs(eta) > 0.6 && fabs(eta) <= 1.6) {
    if (pt > 15 && pt <= 25) {
      weight = 0.976;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.985;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.985;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.983;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.996;
    }
  } else if (fabs(eta) > 1.6 && fabs(eta) <= 2.1) {
    if (pt > 15 && pt <= 25) {
      weight = 0.991;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.994;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.992;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.996;
    } else if (pt > 60 && pt <= 200) {
      weight = 0.996;
    }
  } else if (fabs(eta) > 2.1 && fabs(eta) <= 2.4) {
    if (pt > 15 && pt <= 25) {
      weight = 0.923;
    } else if (pt > 25 && pt <= 35) {
      weight = 0.963;
    } else if (pt > 35 && pt <= 45) {
      weight = 0.994;
    } else if (pt > 45 && pt <= 60) {
      weight = 0.994;
    } else if (pt > 60 && pt <= 200) {
      weight = 1.011;
    }
  }
  return weight;
}

#endif
