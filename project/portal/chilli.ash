#!/bin/bash
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

# setup method for register a normal service
setup()
{
    he log.info[ "run the chilli" ]
    chilli -c /etc/chilli.conf &
    creturn ttrue
}
# shut method for unregister all service
shut()
{
    he log.info[ "shut the chilli" ]
    killall chilli
    creturn ttrue
}

# test the shell component is work
test()
{
    he log.info[ "test the chilli" ]
    echo "test the chilli is work"
    creturn ttrue
}

# call the method, you cannot delete this
cend

