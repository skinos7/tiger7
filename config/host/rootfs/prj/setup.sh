#!/bin/bash

sudo mkdir /tmp/skin
sudo mkdir /var/skin
sudo chmod a+rwx /tmp/skin
sudo chmod a+rwx /var/skin

he arch@data.setup
he land@machine.setup
he land@syslog.setup

daemon&

he init.call[general]

