#!/usr/bin/expect

spawn scp -r . root@172.168.3.27:/root/drm_demo
expect "password:"
send "root\r"
interact