import math
import pylab
import numpy as np
# approach comes from http://ocw.mit.edu/courses/mathematics/18-086-mathematical-methods-for-engineers-ii-spring-2006/readings/am51.pdf
# problem comes from http://mathinsight.org/ordinary_differential_equation_introduction_examples
#
# problem:
#    dx/dt = 5x - 3
#     x(2) = 1
#
# solution:
#    true solution is 2/5*exp(5*(t-2)) + 3/5
#

def true_solution(initial_state, alpha, t):
    return .4 * np.exp(5. * (t-2.)) + .6

def rhs(initial_state, current_state, alpha, t):
    return 5 * current_state - 3

def advance_forward(initial_state, current_state, alpha, time, dt):
    return previous_state + dt * rhs(initial_state, current_state, alpha, time)

def advance_trapezoid(initial_state, current_state, alpha, time, dt):
    return previous_state + .5 * dt * ( rhs(initial_state, previous_state, alpha, time - dt) + rhs(initial_state, current_state, alpha, time) )

def advance_implicit(dt):
    return 1. / ( 1 - 2.5 * dt ) * (previous_state_implicit + 0.5 * dt * ( 5 * previous_state_implicit - 6 ) )
    
# parameters
alpha         = -0.05
initial_time  = 2.0
initial_state = 10.0
initial_state_at_initial_time = true_solution(initial_state, alpha, initial_time)
final_time    = 4.0
time_steps    = 11
number_of_predictor_steps = 1
number_of_corrector_steps = 4


# computed quantities
dt = float(final_time - initial_time) / time_steps
current_state = initial_state_at_initial_time
current_state_implicit = initial_state_at_initial_time
# solution vector
plot_times  = np.linspace(initial_time, final_time, time_steps, endpoint=True)
plot_true_values = true_solution(initial_state, alpha, plot_times)
plot_values = np.zeros(time_steps)
plot_values_implicit = np.zeros(time_steps)
 
for i in range(time_steps):
    time = initial_time + i * dt
    previous_state = current_state
    previous_state_implicit = current_state_implicit
    
    for j in range(number_of_predictor_steps):
        # advance the predictor step
        current_state = advance_forward(initial_state, current_state, alpha, time, dt)
        
        for k in range(number_of_corrector_steps):
            # advance the corrector step
            current_state = advance_trapezoid(initial_state, current_state, alpha, time, dt)

    current_state_implicit = advance_implicit(dt)

    plot_values[i] = current_state
    plot_values_implicit[i] = current_state_implicit

pylab.plot(plot_times, plot_values, 'bo')#, plot_true_values, 'k')
pylab.plot(plot_times, plot_values_implicit, 'ro')#, plot_true_values, 'k')
pylab.plot(plot_times, plot_true_values, 'k')
pylab.show()
