#!/bin/sh
rm assets/*
pebble screenshot --emulator flint
mv pebble_screenshot* assets/flint.png
pebble screenshot --emulator emery
mv pebble_screenshot* assets/emery.png
pebble screenshot --emulator gabbro
mv pebble_screenshot* assets/gabbro.png
pebble screenshot --emulator diorite
mv pebble_screenshot* assets/diorite.png
pebble screenshot --emulator chalk
mv pebble_screenshot* assets/chalk.png
pebble screenshot --emulator basalt
mv pebble_screenshot* assets/basalt.png
pebble screenshot --emulator aplite
mv pebble_screenshot* assets/aplite.png