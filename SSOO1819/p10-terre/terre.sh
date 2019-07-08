#!/bin/sh
usage()
{
    echo "Usage: $0 file" >&2
    exit 1
}

bad_file()
{
    echo "Error: invalid file \"$1\"" >&2
    exit 1
}

show_result()
{
    cat $1 | tr A-Z a-z | sed -n '/^[a-z|0-9]/p' | awk '
        $1 ~ /^[0-9]/ {
            if (highest < $4)
                highest = $4
            total++
            accum+=$3
        }
        $1 ~ /^[a-z]/ {
            if (total > 0)
                printf("%s %f %f\n", city, accum/total, highest)
                accum=0
                total=0
                highest=0
            city=$1
        }
        END{
            if (total > 0)
                printf("%s %f %f\n", city, accum/total, highest)
        }'
}

if test $# -ne 1; then
	usage
elif test -f $1; then
    show_result $1
else
    bad_file $1
fi
