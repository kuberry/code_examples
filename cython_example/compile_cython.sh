# from http://blog.perrygeo.net/2008/04/19/a-quick-cython-introduction/
cython cython_pi.pyx

# Compile the object file   
gcc -O2 -c -fPIC -I/usr/include/python2.7/ cython_pi.c

# Link it into a shared library
gcc -shared cython_pi.o -o cython_pi.so

