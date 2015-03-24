from ctypes.util import find_library
from ctypes import *
import sys

f = cdll.LoadLibrary("./lib/libhello.so")
# While we should be able to call "print f.hello()",
# this is C++ and therefore the names are mangled and are
# context and compiler dependent.
print f._Z5hellov()
