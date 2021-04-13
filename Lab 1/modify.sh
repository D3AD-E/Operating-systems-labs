#!/bin/bash

name=`basename $0`

recursive=n
upper=n
lower=n
sed_pattern=n

error_msg()
{
        echo "$name: error: $1" 1>&2 
}

help() 
{
cat<<EOT 1>&2
Arguments:
[-r]  use recursion
[-l]  rename to lowercase
[-u]  rename to uppercase
<sed pattern> use sed pattern
[-h]  show help

Correct syntax examples:
$name -r -u ./file
$name -l ./File
$name -u ./file
$name -r <sed pattern> ./file
$name <sed pattern> ./File
$name -h

Incorrect syntax examples:
$name -l -u
$name -l
$name -r
EOT
}

rename_internal()
{
	if [ -f "$1" ]; then
		if test $upper = "y"; then
			echo "$name: Uppercasing $1"
			dest=`dirname "${1}"`/`basename "${1}" | tr "[:lower:]" "[:upper:]"`
		elif test $lower = "y"; then
			echo "$name: Lowercasing $1"
			dest=`dirname "${1}"`/`basename "${1}" | tr "[:upper:]" "[:lower:]"`
		elif test $sed_pattern != "n"; then
			echo "$name: Run sed on $1"
			sed -i -e "$sed_pattern" "$1"; return
		else
			error_msg "neither -l or -u or sed pattern was provided"; exit 1;
		fi
		if [ "${1}" != "${dest}" ]; then
	       		[ ! -e "${dest}" ] && mv -T "${1}" "${dest}" || error_msg "${1} was not renamed (same file name?)"
		fi
	fi
}

rename()
{
	if test $recursive = "y"; then
		find "$1" -print0 | while IFS= read -r -d '' f; do
			rename_internal "$f"
		done
	elif test $recursive = "n"; then
		if [ -f "$1" ]; then
			rename_internal "$1"
		else
			for f in "$1"/*; do
				rename_internal "$f"
			done
		fi
	fi
}

if test -z "$1"
then
	help; exit 0;
fi

while test "x$1" != "x"
do
        case "$1" in
		-h) help; exit 0;;
                -r) recursive=y;;
                -l) if test $upper = y; then error_msg "Contradicting parameters -l and -u"; exit 1; else lower=y; fi;;
                -u) if test $lower = y; then error_msg "Contradicting parameters -l and -u"; exit 1; else upper=y; fi;;
		-s) sed_pattern="$2"; shift;;
                -*) error_msg "bad option $1"; exit 1 ;;
                *) rename "$1"
        esac
        shift
done
