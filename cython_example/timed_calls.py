import timeit 

t1 = timeit.Timer("python_pi.python_get_pi(%i)" % (int(1e+8),), "import python_pi")
print "Pure python function", t1.timeit(1), "sec"

t2 = timeit.Timer("cython_pi.cython_get_pi(%i)" % (int(1e+8),), "import cython_pi")
print "Cython function", t2.timeit(1), "sec"
