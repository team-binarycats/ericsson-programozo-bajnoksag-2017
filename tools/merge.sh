#!/bin/sh
# Merge all forks recursively

drop_shell(){
	reason="$1"
	echo -e "\033[41m$reason\033[0m" >&2
	echo -e "Please fix the errors in this shell and press Ctrl+D. or" >&2
	echo -e "do an \`exit 1\` if you want to abort merging \033[1m$mother\033[0m" >&2
	"$(getent passwd `logname` | cut -d: -f7)" || exit $?
	echo -e "\033[42mContinuing merge...\033[0m"
}

mother="$(git symbolic-ref --short HEAD)"
forkdb="$(git rev-parse --show-toplevel)/tools/fork_db"

if [ -f "$forkdb/$mother" ]; then
	cat "$forkdb/$mother" | while read fork; do
		git checkout "$fork" || drop_shell "Failed to checkout $fork"
		echo -e "\033[33mMerging into \033[1m$fork\033[0m" >&2
		git merge "$mother" "$@" || drop_shell "Merge from $mother failed (status: $?)"
		"$0" "$@" || drop_shell "Recursive merging $fork failed (status: $?)"
	done
	echo -e "\033[32mMerge done from \033[1m$mother\033[0m" >&2
	git checkout "$mother" || drop_shell "Failed to checkout $mother"
else
	echo -e "\033[31mNo forks defined for \033[1m$mother\033[0m" >&2
fi
