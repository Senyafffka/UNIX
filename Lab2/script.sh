#!/bin/bash 

cont_id=$(shuf -i 1-999999 -n 1)
sync_file="/data/sync.lock"
file_counter=1

while true; do 
(
	flock -x 200
	while true; do
    	file_name=$(printf "%03d" "$file_counter")
      	if [ ! -f /data/$file_name ]; then
        	echo "$cont_id:$file_counter" > /data/$file_name
		echo "$(cat "/data/$file_name")"
		break
      	fi
    	file_counter=$((file_counter + 1))
  	done
)200>"$sync_file"

sleep 1
(
	flock -x 200
        if [ -f "/data/$file_name" ]; then
            rm "/data/$file_name"
        fi
)200<"$sync_file"
sleep 1
done
