make

for input_file in OS_PJ1_Test/*
do 
	sudo dmesg --clear
	output_file=${input_file/OS_PJ1_Test/output}
	dmesg_name=${output_file/\.txt/_dmesg\.txt}
	stdout_name=${output_file/\.txt/_stdout\.txt}
	sudo ./scheduler < ${input_file} > ${stdout_name}
	dmesg | grep Project1 > ${dmesg_name}
done

make clear