#!/bin/bash

sudo mkdir /tmp/skin
sudo mkdir /var/skin
sudo chmod a+rwx /tmp/skin
sudo chmod a+rwx /var/skin

# need arch@data.setup init the dir
he arch@data.setup
# need storage@media ready for mount the sd or usb
he storage@media.setup
# need land@machine.setup init the configure and register the project
he land@machine.setup
# start the log first
he land@syslog.setup
# enable the sdk watchdog
#watchdog
# skinos daemon
daemon&
sleep 1
# core
he land@init.call[core]
he land@init.call[core2]
# network frame
he land@init.call[network]
# setup modem
he land@init.call[modem]
# ethernet
he land@init.call[ethernet]
# wifi
he land@init.call[nradio]
he land@init.call[aradio]
# internet
he land@init.call[extern]
# manage
he land@init.call[manage]
he land@init.call[manage2]
# app
he land@init.call[app]
he land@init.call[app2]
he land@init.call[general]

