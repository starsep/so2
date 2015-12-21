#!/bin/bash

function add_fc {
	if [ $# -ge 1 ] ; then
		cat $1 > .copy
		cat .init .copy > $1
		shift 1
		add_fc $*
	fi
}
x="//Filip Czaplicki 359081"
echo $x > .init
add_fc $*
rm .init .copy
