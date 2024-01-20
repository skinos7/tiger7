#!/bin/bash
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader


# setup method for register a normal service
setup()
{
    echo "the ${PROJECT}@${COM} setup has be called"
    he log.info[ "the ${PROJECT}@${COM} setup has be called"  ]
    creturn ttrue
}
# shut method for unregister all service
shut()
{
    echo "the ${PROJECT}@${COM} setup has be called"
    he log.info[ "the ${PROJECT}@${COM} setup has be called"  ]
    creturn ttrue
}



# method, service will exit with ttrue, the service will log for forever
service()
{
    nubmer=1
    max=11
    while :
    do
        number=$[number+1]
        he log.info[ "hello world ( $number ) times"  ]
        sleep 1
    done
    creturn tfalse
}

# method, call by ${PROJECT}@${COM}.list
list()
{
    ret='{"'$PARAM1'":"'$PARAM2'"}'
    creturn $ret
}

# call the method, you cannot delete this
cend
