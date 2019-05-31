#!/bin/sh

#
# Autorun script
# "--cnr" flag: 		compile and run
# "--compile" flag: 	compile only
# "--run" flag: 		run only
# "--clean" flag: 		removes generated files
#
mkdir -p ./bin

if [ "$1" = "--cnr" ]; then
	sel=1
elif [ "$1" = "--compile" ]; then
	sel=2
elif [ "$1" = "--run" ]; then
	sel=3
elif [ "$1" = "--clean" ]; then
	sel=4
else
	echo "Choose an action:"
	echo "1) Compile and run [default]"
	echo "2) Compile only"
	echo "3) Run only"
	echo "4) Clean project"
	read -p "Selection: " sel
	echo
fi

if [ "${sel}" = "4" ];then
	find . -type f ! -name "*.c" ! -name "*.h" ! -name "*.pdf" ! -name "*.sh"\
			-exec rm {} \;
	echo "Cleared all generated files"
elif [ "${sel}" = "3" ]; then
	sh run.sh
elif [ "${sel}" = "2" ]; then
	sh compile.sh
else
	sh compile.sh
	sh run.sh "${flag}"
fi
