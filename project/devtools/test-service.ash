#!/bin/bash
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader


# setup method for register a normal service
setup()
{
    ret=`he service.register[ test-service-loop, 0, devtools@test-service, service, param1, param2, param3 ]`
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
    ret=`he service.register[ test-service-exit, 0, devtools@test-service, exit_service ]`
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
    ret=`he service.register[ test-service-bad, 0, devtools@test-service, bad_service ]`
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
    ret=`he service.register[ test-service-error, 0, devtools@test-service, error_service ]`
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
    he service.unregister[ test-service-loop ]
    he service.unregister[ test-service-exit ]
    he service.unregister[ test-service-bad ]
    he service.unregister[ test-service-error ]
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
    creturn tfalse
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
# the service will exit with terror, then system never start it again
error_service()
{
    he log.info[ $PROJECT+"@"+$COM+"."+"error_service is start" ]
    sleep 1
    creturn terror
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
