import math
import pylab
import numpy
# problem comes from http://ocw.mit.edu/courses/mathematics/18-086-mathematical-methods-for-engineers-ii-spring-2006/readings/am51.pdf
# true solution is u(0)exp(-alpha*t)

def rhs(alpha, t):
    return -alpha*math.exp(-alpha*t)

def advance_forward(current_state, alpha, time, dt):
    return current_state + dt * rhs(alpha, time)

# parameters
alpha         = 1
initial_state = 10
initial_time  = 1.0
final_time    = 5.0
time_steps    = 100

# computed quantities
dt = float(final_time - initial_time) / time_steps
current_state = initial_state

# solution vector
plot_times  = np.linspace(initial_time, final_time, time_steps, endpoint=True)
plot_values = np.zeros(time_steps+1, 1)
 
for i in range(time_steps):
    time = initial_time + i * dt
    current_state = advance_forward(current_state, alpha, time, dt)
    plot_values[i] = current_state

pylab.plot(plot_times, plot_values)
