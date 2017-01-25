
for i in $@
    do 
    cp parse.sh $i
    cd $i 

    ./parse.sh sdt

    awk 'BEGIN { FS = "," ; OFS = "," }; {print $11-i; i=$11}' $i | sed '1,2d' > F.Lbadiff

    for j in Q I D 
        do
        awk 'BEGIN { FS = "," ; OFS = "," }; {print $1-i; i=$1}' ${j}.Lba > ${j}.Lbadiff
        done
   echo "$i "
    for j in Q I D F
        do
        b=`grep -c ^8$ ${j}.Lbadiff`
        a=`grep -c ^0$ ${j}.Lbadiff`
        f=`wc -l <${j}.Lbadiff`
        echo -e "$j: `expr $f - $b - $a`"
    done
    echo
    cd ..
done

