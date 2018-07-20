setlocal
cd src
gcc -c *.c -I ../include
ar rcs libDynamicConnectivity.a *.o
cd ..
cd example
gcc example.c -I ../include -L ../src -l DynamicConnectivity
a.exe