 awk 'BEGIN { curr=1; count=0 }; {if (curr==) {count = count + 1} else { print curr,count ; curr= ; count = 0 } ; };' bundle.size.sort > bundle.size.awk 
