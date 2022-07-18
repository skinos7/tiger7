#!/bin/sh

cons="  0 0 662"
disk="  0 0 660"
null="  0 0 666"
urandom="  root root  644"
mkdev () 
{    # usage: mkdev name [bcu] major minor owner group mode
    if [ "$opt_v" ]
    then    
        if [ "$opt_d" ]
        then    
            echo "sudo rm -f $1"
        else
            echo "$1        = $2 $3 $4 $5:$6 $7"
        fi
    fi
    [ ! "$opt_n" ] && sudo rm -f $1 &&
    [ ! "$opt_d" ] && sudo mknod $1 $2 $3 $4 &&
    sudo chown $5:$6 $1 &&
    sudo chmod $7 $1
}
cd $1



# Serial

# Memory

# Storage

# Misc


