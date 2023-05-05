#!/bin/bash

/root/dbbus/dbus_monitor.service &

echo $! > /root/dbbus/dbus_monitor.pid
