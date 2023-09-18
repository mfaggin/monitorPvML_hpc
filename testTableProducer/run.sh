#!bin/bash
o2-analysis-qa-event-track-lite-producer -b --configuration json://configuration.json | \
o2-analysis-pid-tof-base -b --configuration json://configuration.json | \
o2-analysis-track-propagation -b --configuration json://configuration.json | \
o2-analysis-timestamp -b --configuration json://configuration.json | \
o2-analysis-ft0-corrected-table -b --configuration json://configuration.json | \
o2-analysis-multiplicity-table -b --configuration json://configuration.json | \
o2-analysis-event-selection -b --configuration json://configuration.json | \
o2-analysis-mccollisionextra -b --configuration json://configuration.json | \
#o2-analysis-lf-mccollisionextra -b --configuration json://configuration.json | \
o2-analysis-trackselection -b --configuration json://configuration.json --aod-file @input_data.txt --aod-writer-json OutputDirector.json