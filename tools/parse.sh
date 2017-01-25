  blkparse sdg | grep  " Q "  >Q
 blkparse sdg | grep  " I "  >I
 blkparse sdg | grep  " D " >D
 blkparse sdg | grep  " C " >C
 awk '{print $8 }' D >D.Lba
awk '{print  $8}' C >C.Lba
awk '{print  $8}' I >I.Lba
awk '{print  $8}' Q >Q.Lba 
sort -g D.Lba >D.sort
sort -g C.Lba >C.sort
sort -g I.Lba >I.sort
sort -g Q.Lba >Q.sort
cat D.sort |  uniq -d --count >D.Dub
cat C.sort |  uniq -d --count >C.Dub
cat I.sort |  uniq -d --count >I.Dub
cat Q.sort |  uniq -d --count >Q.Dub
