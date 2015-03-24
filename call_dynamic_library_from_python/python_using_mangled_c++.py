from ctypes.util import find_library
from ctypes import *
import sys

f = cdll.LoadLibrary("./lib/libhello.so")
#print f.hello()
print f._Z5hellov()
