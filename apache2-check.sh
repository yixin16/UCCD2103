#!/bin/bash

# check apache syntax
output=$(apachectl configtest 2>&1)

# check the OK
if echo "$output" | grep -q "Syntax OK"; then
	echo "Apache 2 config file OK"
	sudo systemctl reload apache2
	# capture third line of apache2 status
	apache_status=$(systemctl status apache2 | sed -n '3p')
	echo "Apache2 status: $apache_status"
else
	echo "Apache2 config file error!"
fi