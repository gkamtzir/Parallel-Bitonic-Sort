#!/bin/bash
MIN_ARRAY_SIZE=16 #real_min_array_size = 2^MIN_ARRAY_SIZE
MAX_ARRAY_SIZE=24 #real_max_array_size = 2^MAX_ARRAY_SIZE
MIN_THREADS=1 #real_min_treads = 2^MIN_THREADS
MAX_THREADS=8 #again real_max_threads = 2^MAX_THREADS
SAMPLES=50

h=$MIN_THREADS
while [ $h -le $MAX_THREADS ]
do
	echo "ompr$h = ["
	j=1
	while [ $j -le $SAMPLES ]
	do
		i=$MIN_ARRAY_SIZE
		while [ $i -le $MAX_ARRAY_SIZE ]
		do
			../datagen_bin/datagenOMPR "$i" "$h" "$MAX_ARRAY_SIZE"
			i=$(($i + 1))
		done
		if [ $j != $SAMPLES ]
		then
			echo ";"
		fi

		j=$(($j + 1))
	done
	echo "];"
	h=$(($h + 1))
done


