#!/bin/bash

# delete lan
rm -fr ./usr/prj/ifname/lan.cfg
rm -fr ./usr/prj/ifname/lan2.cfg
rm -fr ./usr/prj/ifname/lan3.cfg
rm -fr ./usr/prj/ifname/lan4.cfg
# delete hosts
rm -fr ./usr/prj/network/hosts.cfg
# delete clock
rm -fr ./usr/prj/clock/date.cfg
rm -fr ./usr/prj/clock/restart.cfg

# delete self
rm -fr ./needless.sh


