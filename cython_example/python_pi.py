import math

def python_get_pi(num_digits):
    pi_sq = 0
    for i in range(num_digits):
        pi_sq += 1./math.pow(i+1,2)
    pi_sq *= 6
    return math.sqrt(pi_sq)

