#/bin/bash
 gcc -fPIC -shared -o a.so a.c 

 gcc -o testa testa.c ./a.so

 ./testa
