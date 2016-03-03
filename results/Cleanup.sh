#!/bin/bash
# Script name: Cleanup.sh
# Author: Kyu Kim, Alan Lu, Donald Wilson
# Date: February 2016
#
# Description: Removes logs from unsuccessful runs


filenames=(`find . -type f -printf '%f\n'`)
i=0
while [ $i -lt ${#filenames[*]} ]; do	
	curr=${filenames[$i]}

	if [ "$curr" != "Cleanup.sh" ]; then
		if grep -vq "Maze solved" $curr; then
			rm -f $curr
		fi
	fi

	let i++
done

exit 0