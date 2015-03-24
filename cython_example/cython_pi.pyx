cdef extern from "math.h":
    float pow(int theta, int theta2)
    float sqrt(float theta)

def cython_get_pi(int num_digits):
    cdef float pi_sq = 0
    for i in range(num_digits):
        pi_sq += 1./pow(i+1,2)
    pi_sq *= 6
    return sqrt(pi_sq)

