shim: 
	gcc -O2 -Wall -ldl -fPIC -o malloc_wrap.so -shared malloc_wrap.c

tester:
	gcc -ggdb -Wall -ldl -fPIC -o tester alloc_tester.c

clean: 
	rm -f *.so tester
