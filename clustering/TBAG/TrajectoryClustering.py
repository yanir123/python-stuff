import ctypes
from numpy import ctypeslib as npct

c_lib_numpy = npct.load_library('lib/trajectory_clustering.dll', '.')
