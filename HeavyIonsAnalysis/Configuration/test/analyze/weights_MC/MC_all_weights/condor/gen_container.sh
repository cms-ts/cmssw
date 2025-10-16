#!/bin/bash
base_dir=/gfsvol01/cms/users/rdelliga/work/Hi_forest/2023/CMSSW_13_2_13/src/HeavyIonsAnalysis/Configuration/test/analyze/weights_MC/MC_all_weights/condor
exec_dir=${PWD}

nome=$(paste -s -d, ${base_dir}/list_MC_ZMM_all) 
cat > exec.sh << EOF
set -o noclobber
cat /etc/redhat-release
source $HOME/.bashrc
cd ${base_dir}
cmsenv

# whatever CMSSW related command

cd ${exec_dir}
cmsRun forest_miniAOD_run3_MC_ZMM_all.py inputFiles=${nome}

mv HiForestMiniAOD_MC_all.root ${base_dir}/signal/HiForestMiniAOD_MC_all_${1}.root

EOF

cmssw-el8 -B $HOME -B /gfsvol01 -B $PWD --command-to-run source ${PWD}/exec.sh
