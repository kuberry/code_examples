import math
import pylab
import numpy as np
# problem comes from http://ocw.mit.edu/courses/mathematics/18-086-mathematical-methods-for-engineers-ii-spring-2006/readings/am51.pdf
# true solution is u(0)exp(-alpha*t)

def true_solution(initial_state, alpha, t):
    return initial_state * np.exp(-alpha*t)

def rhs(initial_state, alpha, t):
    return -initial_state * alpha * np.exp(-alpha*t)

def advance_forward(initial_state, current_state, alpha, time, dt):
    return current_state + dt * rhs(initial_state, alpha, time)

# parameters
alpha         = 5.0
initial_time  = 1.0
initial_state = 10.0
initial_state_at_initial_time = true_solution(initial_state, alpha, initial_time)
final_time    = 5.0
time_steps    = 1000

# computed quantities
dt = float(final_time - initial_time) / time_steps
current_state = initial_state_at_initial_time

# solution vector
plot_times  = np.linspace(initial_time, final_time, time_steps, endpoint=True)
plot_true_values = true_solution(initial_state, alpha, plot_times)
plot_values = np.zeros(time_steps)
 
for i in range(time_steps):
    time = initial_time + i * dt
    current_state = advance_forward(initial_state, current_state, alpha, time, dt)
    plot_values[i] = current_state

pylab.plot(plot_times, plot_values, 'bo')#, plot_true_values, 'k')

pylab.plot(plot_times, plot_true_values, 'k')