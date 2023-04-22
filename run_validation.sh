
for val_file in validation_runs/*.txt;
do
    echo "Testing $val_file"
    COMMAND=$(sed '2q;d' $val_file)

    $COMMAND > out_file.txt
    
    diff out_file.txt $val_file
    diff_val=$?
	
	# Output results based on diff's return value.
	if  [[ $diff_val != 0 ]]; then
		echo "** fail ** $val_file"
	else
		echo "PASS! $val_file"
	fi
done