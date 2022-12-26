import math

R0 = 1
R = 10
TRUE_CONDUCTIVITY = 0.01
I = 10

INITIAL_CONDUCTIVITY = 0.001
INITIAL_POWER = 0.1
MINIMAL_RESID = 10 ** -10

CONDUCTIVITY_PARAMETER_TYPE = 0
POWER_PARAMETER_TYPE = 1


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __repr__(self):
        return f"({self.x}, {self.y})"

    def dist(self, p):
        return math.sqrt(pow(self.x - p.x, 2) + pow(self.y - p.y, 2))


class CurrentSource:
    def __init__(self, A, B, power):
        self.A = A
        self.B = B
        self.set_power(power)

    def set_power(self, power):
        self.jA = power / (2 * math.pi * A.x)
        self.jB = -power / (2 * math.pi * B.x)

    def generate_potential(self, pnt1, pnt2, conduct):
        potential = 1 / (2 * math.pi * conduct) * ((self.jB / B.dist(pnt1) + self.jA / A.dist(pnt1)) -
                                                   ((self.jB / B.dist(pnt2)) + self.jA / A.dist(pnt2)))

        return potential

    def generate_potential_derivative(self, pnt1, pnt2, type, value):
        if type == CONDUCTIVITY_PARAMETER_TYPE:
            return -self.generate_potential(pnt1, pnt2, value) / value
        elif type == POWER_PARAMETER_TYPE:
            return self.generate_potential(pnt1, pnt2, TRUE_CONDUCTIVITY) / value


class Receiver:
    def __init__(self, pnt1, pnt2):
        self.synthetic_potential = None
        self.M = pnt1
        self.N = pnt2
        self.weight = 1

    def set_synthetic_potential(self, source, conduct, noise_level=0.0):
        self.synthetic_potential = source.generate_potential(self.M, self.N, conduct)
        self.synthetic_potential *= (1 + noise_level)
        self.weight = 1 / self.synthetic_potential
        print(f"M{self.M}, N{self.N} synthetic potential: {self.synthetic_potential}")


def find_parameter(current_source, receivers, parameter_type, initial_value):
    param_var = initial_value
    prev_residual = None
    iterations_cnt = 0
    while True:
        residual = 0
        new_potentials = []
        for receiver in receivers:
            if parameter_type == POWER_PARAMETER_TYPE:
                current_source.set_power(param_var)
                new_potential = current_source.generate_potential(receiver.M, receiver.N, TRUE_CONDUCTIVITY)
            else:
                new_potential = current_source.generate_potential(receiver.M, receiver.N, param_var)

            new_potentials.append(new_potential)
            residual += math.pow(receiver.weight * (new_potential - receiver.synthetic_potential), 2)
        print(f"Iteration {iterations_cnt}. Parameter value: {param_var}. Residual: {residual}")
        if residual < MINIMAL_RESID or iterations_cnt > 100:
            print('Solution: ', param_var)
            break

        if prev_residual is not None and math.fabs(prev_residual - residual) < MINIMAL_RESID:
            print('Stagnation')
            break

        print(new_potentials)
        a = 0
        b = 0
        for i in range(len(receivers)):
            derivative = current_source.generate_potential_derivative(receivers[i].M, receivers[i].N, parameter_type, param_var)
            a += (receivers[i].weight * derivative) ** 2

            b -= (receivers[i].weight ** 2) * derivative * (new_potentials[i] - receivers[i].synthetic_potential)

        print("A11:", a)
        print("b1:", b)
        step = b / a
        param_var += step
        prev_residual = residual

        iterations_cnt += 1

    return param_var


if __name__ == '__main__':
    A = Point(R0, 0)
    B = Point(R, 0)
    current_source = CurrentSource(A, B, I)

    receivers = []
    M1 = Point(100, 0)
    N1 = Point(200, 0)
    M2 = Point(400, 0)
    N2 = Point(450, 0)
    M3 = Point(800, 0)
    N3 = Point(1000, 0)

    receivers.append(Receiver(M1, N1))
    receivers.append(Receiver(M2, N2))
    receivers.append(Receiver(M3, N3))

    receivers[0].set_synthetic_potential(current_source, TRUE_CONDUCTIVITY, 0.1)
    receivers[1].set_synthetic_potential(current_source, TRUE_CONDUCTIVITY, 0.0)
    receivers[2].set_synthetic_potential(current_source, TRUE_CONDUCTIVITY, 0.0)
    conduct = find_parameter(current_source, receivers, CONDUCTIVITY_PARAMETER_TYPE, initial_value=INITIAL_CONDUCTIVITY)
    print(f'True conductivity: {TRUE_CONDUCTIVITY}, experimental conductivity: {conduct}')

    current = find_parameter(current_source, receivers, POWER_PARAMETER_TYPE, initial_value=INITIAL_POWER)
    print(f'True I: {I}, experimental I: {current}')
