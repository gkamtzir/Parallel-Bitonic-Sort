#!/bin/bash

#THREADS ARE IGNORED AS qsort of std library run on one thread
#so MIN_THREADS should be equal to MAX_THREADS and do not effect
#what so ever the algorithm.

MIN_ARRAY_SIZE=16 #real_min_array_size = 2^MIN_ARRAY_SIZE
MAX_ARRAY_SIZE=24 #real_max_array_size = 2^MAX_ARRAY_SIZE
MIN_THREADS=1 #real_min_treads = 2^MIN_THREADS
MAX_THREADS=1 #again real_max_threads = 2^MAX_THREADS
SAMPLES=10

h=$MIN_THREADS
while [ $h -le $MAX_THREADS ]
do
	echo "q$h = ["
	j=1
	while [ $j -le $SAMPLES ]
	do
		i=$MIN_ARRAY_SIZE
		while [ $i -le $MAX_ARRAY_SIZE ]
		do
			../datagen_bin/datagenQS "$i" "$h" "$MAX_ARRAY_SIZE"
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

