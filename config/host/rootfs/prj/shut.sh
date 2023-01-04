#!/bin/bash

daemon stop

# app
he land@uninit.call[general]
he land@uninit.call[app2]
he land@uninit.call[app]
# manage
he land@uninit.call[manage2]
he land@uninit.call[manage]
# internet
he land@uninit.call[extern]

# wifi
#he land@uninit.call[aradio]
#he land@uninit.call[nradio]
#he land@uninit.call[wifi]
# ethernet
he land@uninit.call[ethernet]
# modem
#he land@uninit.call[modem]

# network frame
he land@uninit.call[bridge]
he land@uninit.call[vlan]
he land@uninit.call[network]
he network@frame.shut

# core
he land@uninit.call[core]
he land@uninit.call[core2]

he arch@gpio.shut
he land@syslog.shut
he arch@data.shut
daemon exit

sudo rm -fr /tmp/skin
sudo rm -fr /var/skin

