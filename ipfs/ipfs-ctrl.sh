#!/bin/bash

source /etc/profile

if [ $# -gt 0 ] 
then
	if [ $1 == "id" ]; then
		echo "`ipfs id -f="<id>\n"`"
	elif [ $1 == "connect" ]; then
		if [ $# -gt 3 ]; then
			echo "err connect no args"
		else
			echo "`ipfs bootstrap add /ip4/$2/tcp/4001/ipfs/$3`"
		fi	
	elif [ $1 == "add" ]; then
		if [ $# -gt 2 ]; then
			echo "err add no args"
		else
			echo "`ipfs add $2 -Q `"
		fi
	elif [ $1 == "get" ]; then
		if [ $# -eq 2 ]; then      
			ipfs get $2 -o temp.txt
		elif [ $# -eq 3 ]; then
			ipfs get $2 -o "$3"
		else 	
			echo "err get no args"
		fi
	elif [ $1 == "swarm" ]; then
		if [ $2 == "peers" ]; then
			ipfs swarm peers
		fi
	elif [ $1 == "cat" ]; then
		if [ $# -eq 2 ]; then
			ipfs cat $2
		else
			echo "err cat no args"
		fi
	else                           	
		echo "err no commend"
	fi	
else
	echo "err no args"
fi
