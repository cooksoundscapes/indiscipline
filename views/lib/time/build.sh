#! /bin/bash
gcc -fPIC -o time.o -c time.c -Ilua &&
gcc -fpic -shared -o time.so time.o -Llua -llua &&

rm time.o &&
mv time.so ../
