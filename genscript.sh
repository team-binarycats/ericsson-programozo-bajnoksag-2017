#!/bin/bash

commit(){
	#echo "COMMITTING(cnt=$cmdcnt, new=$newcmd, old=$oldcmd, revert=$revert)"
	if [ $cmdcnt != 0 ]; then
		if [ $cmdcnt = 1 ]; then
			echo "$oldcmd"
		else
			echo "repeat($cmdcnt, $oldcmd)"
		fi
	fi
	let cmdcnt=1
	oldcmd="$newcmd"
}

simplify(){
	oldcmd=""
	cmdcnt=0
	while read cmd; do
		newcmd="$cmd"
		revert="$cmd"

		if [ "x$newcmd" = "x$oldcmd" ]; then
			let cmdcnt++
		else
			commit
		fi
	done
	commit
}

generate(){
	echo "include(\`library.m4')dnl"
	grep '^]' \
		| sed -s 's/^] Sent moves, 0 to \(.*\)$/\1/' \
		| simplify
}

cat log | generate >scripts/$(LANG=us date +%A | tr '[:upper:]' '[:lower:]' | tr -d '\n').m4
