#!/bin/sh

usage()
{
	echo "Usage: $0 path" >&2
	exit 1
}

write_on_file()
{
	for i in $*; do
		filename=$(echo $i | cut -c 1 | tr A-Z a-z)
		if test -f $i; then
			cat $i >> $filename.output
		fi
	done
}

if test $# -ne 1; then
	usage
elif test -d $1; then
	cd $1
	rm -f *.output > /dev/null 2>&1
	txtfiles=$(ls -l | egrep .txt$| awk '{ printf("%s\n", $9) }' | sort -d)
	write_on_file $txtfiles
else
	echo Error: path \"$1\" not found >&2
	exit 1
fi
exit 0
