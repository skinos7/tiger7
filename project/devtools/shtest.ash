#!/bin/bash
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader


# setup method for register a normal service
setup()
{
    ret=`he service.register[ shtest-loop, 0, devtools@shtest, service, param1, param2, param3 ]`
    if [ $ret == "ttrue" ]
    then
        creturn ttrue
    else
        cretrun tfalse
    fi
}
# setup method for register a exit not faster service
setup_exit()
{
    ret=`he service.register[ shtest-exit, 0, devtools@shtest, exit_service ]`
    if [ $ret == "ttrue" ]
    then
        creturn ttrue
    else
        cretrun tfalse
    fi
}
# setup method for register a exit faster service
setup_bad()
{
    ret=`he service.register[ shtest-bad, 0, devtools@shtest, bad_service ]`
    if [ $ret == "ttrue" ]
    then
        creturn ttrue
    else
        cretrun tfalse
    fi
}
# setup method for register a error service
setup_error()
{
    ret=`he service.register[ shtest-error, 0, devtools@shtest, error_service ]`
    if [ $ret == "ttrue" ]
    then
        creturn ttrue
    else
        cretrun tfalse
    fi
}


# shut method for unregister all service
shut()
{
    he service.unregister[ shtest-loop ]
    he service.unregister[ shtest-exit ]
    he service.unregister[ shtest-bad ]
    he service.unregister[ shtest-error ]
    creturn ttrue
}



# the service will exit with ttrue, the service will log for forever
service()
{
    nubmer=1
    max=11
    while :
    do
        number=$[number+1]
        he log.info[ "reggpio log test loop( $number )"  ]
        sleep 1
    done
    creturn ttrue
}
# the service will exit with tfalse, then system will start it again
exit_service()
{
    he log.info[ $PROJECT+"@"+$COM+"."+"exit_service is start" ]
    sleep 3
    creturn tfalse
}
# the service will exit with tfalse, then system will start it again, but Will not activate it frequently beacuse it exited too faster
bad_service()
{
    he log.info[ $PROJECT+"@"+$COM+"."+"bad_service is start" ]
    sleep 1
    creturn tfalse
}
# the service will exit with TALK_ERROR, then system never start it again
error_service()
{
    he log.info[ $PROJECT+"@"+$COM+"."+"error_service is start" ]
    sleep 1
    creturn TALK_ERROR
}


list()
{
    ret='{"'$PARAM1'":"'$PARAM2'"}'
    creturn $ret
}
joint()
{
    creturn ttrue
}

# call the method, you cannot delete this
cend
