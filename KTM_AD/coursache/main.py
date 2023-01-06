import random
import numpy


def gen_sample(capacity, mu=0, sigma=1):
    return [random.normalvariate(mu=0, sigma=1) for _ in range(0, capacity)]

def calculate_criteria_stat(sample):
    sample_range = max(sample) - min(sample)

    sum = 0
    mean = numpy.mean(sample)
    for x in sample:
        sum += (x - mean) ** 2

    dispersion_estimate = 1 / (len(sample) - 1) * sum

    return sample_range / dispersion_estimate

def sample_to_file(sample, mu, sigma, path):
    with open(path, 'w') as f:
        n = len(sample)
        f.write(f"Нормальное распределение с масштабом {sigma} и сдвигом {mu}\n")
        f.write(f"0 {n}\n")
        for x in sample:
            f.write(f"{x}\n")

capacity = 100
x_sample = gen_sample(capacity)
sample_to_file(x_sample, 0, 1, 'sample.dat')
iterations = 100
m = 0
x_stat = calculate_criteria_stat(x_sample)
print(f"X-статистика критерия: {x_stat}")
for i in range(0, iterations):
    y_sample = gen_sample(capacity)
    y_stat = calculate_criteria_stat(y_sample)
    # print(f"Y-статистика критерия: {y_stat}")
    if y_stat > x_stat:
        m += 1
print("m:", m)
if m / iterations < (1 - m / iterations):
    p = 2 * m / iterations
else:
    p = 2 * (1 - m / iterations)

print("p-value:", p)