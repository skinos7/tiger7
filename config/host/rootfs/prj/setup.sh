#!/bin/bash

# need arch@data.setup init the dir
he arch@data.setup
# need land@machine.setup init the configure and register the project
he land@machine.setup
# start the log first
he land@syslog.setup
# enable the sdk watchdog
#watchdog
# skinos daemon
daemon&
# init the date
he clock@date.setup
# init the hotplug
#he arch@hotplug.setup
# init the led
#he arch@gpio.setup
# core
he land@init.call[core]
he land@init.call[core2]

# network frame
he network@frame.setup
he land@init.call[network]
he land@init.call[vlan]
he land@init.call[bridge]
# local preset
he land@init.call[local]

# setup modem
#he land@init.call[modem]
# ethernet
he land@init.call[ethernet]

# wifi
#sleep 1
#he land@init.call[wifi]
#he land@init.call[nradio]
#he land@init.call[aradio]

# internet
he land@init.call[extern]

# manage
he land@init.call[manage]
he land@init.call[manage2]
# app
he land@init.call[app]
he land@init.call[app2]
he land@init.call[general]

