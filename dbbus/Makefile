all:
	cp ./network_monitor.sh /usr/bin/network_monitor.sh
	cp ./dbus_monitor.service /etc/systemd/system/dbus_monitor.service
	systemctl start dbus_monitor
stop:
	systemctl stop dbus_monitor
restart:
	systemctl restart dbus_monitor
