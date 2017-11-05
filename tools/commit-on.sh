#!/bin/sh
# Commit on a different branch, recursive merge and push

original="$(git symbolic-ref --short HEAD)"
target="$1"
shift

error(){
	status="$?"
	message="$1"

	echo -e "\033[41m$message\033[0m"
	exit $status
}

go_back(){
	git checkout "$original" || error "Failed to checkout the original branch"
}

git checkout "$target" || error "Failed to checkout target branch"
trap 'go_back' EXIT
trap 'go_back; exit 1' INT
trap 'exit 1' HUP

while [ "$#" -gt 0 ]; do
	case "$1" in
		--)
			shift
			break
			;;
		-*)
			break
			;;
		*)
			git add "$1"
			;;
	esac
	shift
done
git commit "$@" || error "Failed to commit files"

mergetool="$(git rev-parse --show-toplevel)/tools/merge.sh"
"$mergetool" || error "Failed to do the recursive merge"

go_back
trap '' EXIT
trap 'exit 1' INT

for i in {3..1}; do
	echo -ne "Pushing in $i...\r"
	sleep 1
done; echo
git push --all || error "Failed to push"
