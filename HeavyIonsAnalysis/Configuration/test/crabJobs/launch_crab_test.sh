#!/bin/bash

sleep 6000s

crab submit -c crab_mc_MinBias.py
sleep 6000s

crab submit -c crab_data_MinBias.py
