#!/bin/bash
wget http://www.mimuw.edu.pl/~mp249046/teaching/SO/zad2.html &> /dev/null
if [ $? = 0 ] & [ 1 = 1 ]; then
	mv zad2.html task.html
	echo "task.html updated"
else
	echo "Error in wget"
fi
wget http://www.mimuw.edu.pl/~mp249046/teaching/SO/faq2.html &> /dev/null
if [ $? = 0 ] ; then
	mv faq2.html faq.html
        echo "faq2.html updated"
else
        echo "Error in wget"
fi

rm -rf zad2.html.*
