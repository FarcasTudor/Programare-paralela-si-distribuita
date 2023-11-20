#! /bin/bash
NO_ROWS=$1
NO_COLUMNS=$2
OUTPUT_FILE=$3
UPPER_LIMIT=$4

random_array () {
	NO_ELEMENTS=$1
	result=()
	for i in $(seq 1 $NO_ELEMENTS)
	do
		result+=(`expr $RANDOM % $UPPER_LIMIT`)
	done
	echo ${result[@]}
}

while [ $NO_ROWS -gt 0 ]; do
	res=`random_array $NO_COLUMNS`
	echo $res >> $3
	NO_ROWS=`expr $NO_ROWS - 1`
done
