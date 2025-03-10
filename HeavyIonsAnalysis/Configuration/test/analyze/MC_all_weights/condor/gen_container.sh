#!/bin/bash

base_dir=/gfsvol01/cms/users/rdelliga/work/Hi_forest/new/CMSSW_13_2_4/src/HeavyIonsAnalysis/Configuration/test/analyze/MC_all_weights/condor
exec_dir=${PWD}

cat > exec.sh << EOF

cat /etc/redhat-release
source $HOME/.bashrc
cd ${base_dir}
cmsenv

# whatever CMSSW related command

cd ${exec_dir}
cmsRun forest_miniAOD_run3_MC_ZMM_all_${1}.py

mv HiForestMiniAOD_MC_all_${1}.root ${base_dir}/HiForestMiniAOD_MC_all.root

EOF

cmssw-el8 -B $HOME -B /gfsvol01 -B $PWD --command-to-run source ${PWD}/exec.sh
