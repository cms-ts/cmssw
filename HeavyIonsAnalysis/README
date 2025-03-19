Setup instructions

cmssw-el8 
export SCRAM_ARCH=el8_amd64_gcc11

cmsrel CMSSW_13_2_13
cd CMSSW_13_2_13/src
cmsenv
git cms-merge-topic cms-ts:ts_forest_CMSSW_13_2_X_v2
git remote add my-cmssw https://github.com/cms-ts/cmssw.git
git fetch my-cmssw --no-tags
git checkout ts_forest_CMSSW_13_2_X_v2
scram b -j8
