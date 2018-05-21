# #!/bin/bash
# echo "Hello World! Here is a calendar"
# cal
# echo "Today's date is" `date +%a`
# cat /etc/shadow
# 
# echo "Age please: "
# read age

# IF THEN ELSE
# if  [ $age -lt 20 ]
# then
# 	echo "Cant drive"
# else
# 	echo "U can drive"
# fi

# DO WHILE DONE
# echo "please enter a number"
# read num
# while [ $num -le 100 ]
# do
# 	echo "Number not greater than 100"
# 	echo "please enter a number"
# 	read num
# done
# echo "Ok, now $num greater than 100"

# FOR ... IN DO DONE
# for name in /etc/passwd /etc/hosts
# do
	# wc $name # wc word count
# done

# FOR x IN `SEQ n m` DO DONE
# ls
# for num in `seq 1 12`
# do
# 	touch test$num
# done
# ls

# INSTALLED MAIL CHECK THAT A MAIL 
# EXISTS AND SEND MAIL IF NOT THERE
# if [ -f /tmp/check ]
# then
# 	echo "All good"
# else
# 	echo "/tmp/check missing" | mail root
# fi
# 
