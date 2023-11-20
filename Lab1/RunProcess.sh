#! /bin/bash

PROCESS_PATH=$2
IS_BINARY_OR_JAVA=$1 # 1 means java and 2 means binary code
NO_PARAMS_FOR_PROCESS=`expr $# - 2`
EXECUTE_COMMAND=""
if [ $IS_BINARY_OR_JAVA -eq 1 ]; then
	EXECUTE_COMMAND="java -jar $PROCESS_PATH"
else
	EXECUTE_COMMAND=".$PROCESS_PATH"
fi

shift 2

while [ $NO_PARAMS_FOR_PROCESS -ge 0 ]; do

	EXECUTE_COMMAND+=" $1"
	shift	
	NO_PARAMS_FOR_PROCESS=`expr $NO_PARAMS_FOR_PROCESS - 1`
done
$EXECUTE_COMMAND
