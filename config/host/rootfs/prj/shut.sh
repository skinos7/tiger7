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
# modem
he land@uninit.call[modem]
# wifi
he land@uninit.call[aradio]
he land@uninit.call[nradio]
# ethernet
he land@uninit.call[ethernet]
# network frame
he land@uninit.call[network]
# core
he land@uninit.call[core]
he land@uninit.call[core2]

he land@syslog.shut
he arch@data.shut
daemon exit

sudo rm -fr /tmp/skin
sudo rm -fr /var/skin

