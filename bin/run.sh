#hfplayer
#sample run command
#

sudo ./hfplayer -nt 1 -cfg sampleConf-sda8.csv sampleTrace.csv

#sample dependency replay commands:
./depAnalyser -m 1 -w 1000 sampleTrace.csv
sudo ./hfplayer -mode dep -nt 1 -cfg sampleConf-sda8.csv sampleTrace.csv.annot
