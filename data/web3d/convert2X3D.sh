#!/bin/bash

myfp=`which $0`
mydir=`dirname $myfp`

echo $mydir

for f in $mydir/*.x3db
do
	echo "Processing - $f"
	if ./x3db2x3d "$f" > /dev/null; then
		echo "succesfull"
	fi
done
