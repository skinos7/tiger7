#!/bin/bash

# delete the needless init
lookdir="./etc/init.d"
exception="sysfixtime boot system sysctl done set-irq-affinity dropbear land"
for file in `ls ${lookdir}`
do 
    save=0
    for ext in ${exception}
    do 
        if [ ${file} == ${ext} ]
        then 
            save=1
        fi
    done
    if [ $save != 1 ]
    then
        rm -f ${lookdir}/${file}
    fi
done



# delete self
rm -fr ./needless.sh


