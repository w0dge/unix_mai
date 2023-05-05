#!/bin/bash

connected=false
start_time=0
end_time=0

dbus-monitor --system "type='signal',interface='org.freedesktop.NetworkManager'" | while read -r line; do
    if [[ $line == *"uint32 70"* ]]; then
        if [ "$connected" = false ]; then
            start_time=$(date +%s)
            connected=true
            echo "Internet connection started: $(date +%F\ %T)"
            echo "Internet connection started: $(date +%F\ %T)" >> /var/log/network_monitor.log
        fi
    fi

    if [[ $line == *"uint32 50"* ]]; then
        if [ "$connected" = true ]; then
            end_time=$(date +%s)
            time_diff=$((end_time-start_time))
            echo "Internet connection ended: start_time=$(date -d @$start_time +%F\ %T), end_time=$(date -d @$end_time +%F\ %T), duration=${time_diff}s"
            echo "Internet connection ended: start_time=$(date -d @$start_time +%F\ %T), end_time=$(date -d @$end_time +%F\ %T), duration=${time_diff}s" >> /var/log/network_monitor.log
            connected=false
        fi
    fi
done
