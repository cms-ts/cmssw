#!/bin/bash

root -l -b -q 'compare_inclusive_leading.C("xZj")'
root -l -b -q 'compare_inclusive_leading.C("jetPt")'
root -l -b -q 'compare_inclusive_leading.C("deltaPhi")'

root -l -b -q 'compare_inclusive_leading.C("xZj", "ppref24")'
root -l -b -q 'compare_inclusive_leading.C("jetPt", "ppref24")'
root -l -b -q 'compare_inclusive_leading.C("deltaPhi", "ppref24")'

root -l -b -q 'compare_inclusive_leading.C("xZj", "PbPb23", false)'
root -l -b -q 'compare_inclusive_leading.C("jetPt", "PbPb23", false)'
root -l -b -q 'compare_inclusive_leading.C("deltaPhi", "PbPb23", false)'

root -l -b -q 'compare_inclusive_leading.C("xZj", "ppref24", false)'
root -l -b -q 'compare_inclusive_leading.C("jetPt", "ppref24", false)'
root -l -b -q 'compare_inclusive_leading.C("deltaPhi", "ppref24", false)'

