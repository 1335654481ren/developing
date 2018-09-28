#! /usr/bin/expect
set timeout 30
spawn sudo apt-get update
expect "password for renxl:"
send "renxl\r"
interact
