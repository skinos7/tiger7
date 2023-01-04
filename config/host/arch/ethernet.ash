#!/bin/sh
# Copyright (C) 2020-2022 ASHYELF
# include fundamental function support, you cannot delete this
. $cheader

setup()
{
    dev=`he land@register.string[ land,local_netdev ]`
    he land@com.register[ ethernet@lan,arch@ethernet ]
    he network@frame.add[ ethernet@lan,$dev ]
    echo "{\"netdev\":\"$dev\"}" > /var/skin/network/ifname@lan.ol
    # default the configure if order
    creturn ttrue
}

netdev()
{
    dev=`he land@register.string[land,local_netdev]`
    creturn $dev
}
up()
{
    creturn ttrue
}
down()
{
    creturn ttrue
}
connect()
{
    creturn ttrue
}
connected()
{
    creturn ttrue
}

shut()
{
    he land@com.unregister[ethernet@lan]
    creturn ttrue
}

# call the method, you cannot delete this
cend

