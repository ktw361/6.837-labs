Arguments:
    ./a3 [integrator] [stepSize] [vis_index]

optional parameters:
    [integrator] one of "e" "t" "r" "mr"
        "e": Euler
        "t": Trapezoidal
        "r": RK4
        "mr": My implementation of RK4

    [stepSize] float, < 0.015 for a reasonable stepSize

    [vis_index] int, index of the particle in pendulum system, -1 displays nothing


Implemented all requirements (rendering, swing & wind), finished easy extra credits.
The max canvas cloth size (in num_intervals) I can achieve is 10x10,
and the speed is slower than a3soln (larger viscous force), otherwise it will explode.
The stepSize also need to be small.
