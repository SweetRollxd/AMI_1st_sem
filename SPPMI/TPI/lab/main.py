# importing the required module
import math
import random as rand
import numpy as np
import statistics
import scipy.stats as stats
# import gaussian_kde, norm

import matplotlib.pyplot as plt

V_FORM = 0.1
B1 = 1.03
DISPERSION = 0.39
EXCESS = 3.36
CAPACITY = 10 ** 3

B4 = 2 * B1 * math.tan(B1)
B3 = V_FORM * B4 / 2 * math.exp(B4)
B2 = B4 / (2 + B4)


def cos_exp_dist(v, b1, b4):
    """
    Генерация случайной величины на основе косинусно-эконенциального распределения
    :param v: параметр формы
    :param b1: параметр b1
    :param b4: параметр b4
    :return: значение случайной величины
    """
    r1 = rand.random()
    if r1 < v:
        r4 = rand.random()
        x2 = 1 - math.log(r4) / b4
        if r1 < v / 2:
            return x2
        else:
            return -x2
    else:
        while True:
            r2 = rand.random()
            r3 = rand.random()
            x1 = 2 * r2 - 1
            if r3 <= math.pow(math.cos(b1 * x1), 2):
                return x1


def cos_exp_density(x):
    if abs(x) <= 1:
        return B2 * math.cos(B1 * x)
    else:
        return B3 * math.exp(-B4 * abs(x))


def generate_data(n: int, shift: float = 0, scale: float = 1):
    data = [shift + scale * cos_exp_dist(V_FORM, B1, B4) for x in range(n)]
    data.sort()
    return data, [cos_exp_density((x - shift) / scale) / scale for x in data]


def generate_noisy_data(n: int, shift: float, scale, noise_level: float):
    if 0.5 <= noise_level <= 0:
        raise RuntimeError("Incorrect noise level")
    noisy_data = []
    for i in range(n):
        r = rand.random()
        if r <= 1 - noise_level:
            noisy_data.append(cos_exp_dist(V_FORM, B1, B4))
        else:
            noisy_data.append(shift + scale * cos_exp_dist(V_FORM, B1, B4))
    noisy_data.sort()
    noisy_density = [(1 - noise_level) * cos_exp_density(x) + noise_level * cos_exp_density((x - shift) / scale) / scale for x in noisy_data]
    return noisy_data, noisy_density


def calculate_estimations(data):
    estimations = dict()
    estimations['mean'] = statistics.mean(data)
    estimations['median'] = statistics.median(data)
    estimations['variance'] = statistics.variance(data, estimations['mean'])
    estimations['skewness'] = stats.skew(data)
    estimations['kurtosis'] = stats.kurtosis(data)
    # среднее арифметическое;
    # выборочная медиана;
    # дисперсия
    # коэф ассиметрии
    # коэф эксцесса
    # ОМП
    # Усеченное среднее (0.05, 0.1, 0.15)
    # обобщенные радикальные оценки (0.1, 0.5, 1)
    return estimations


clean_data, clean_density = generate_data(CAPACITY)

print(calculate_estimations(clean_data))

scale = 1
shift = -5

dirty_data, dirty_density = generate_data(CAPACITY, shift=shift, scale=scale)

noisy_data, noisy_density = generate_noisy_data(CAPACITY, shift=shift, scale=scale, noise_level=0.15)

plt.plot(clean_data, clean_density, dirty_data, dirty_density, noisy_data, noisy_density)
# plt.plot(dirty_data, dirty_density)
plt.grid()
plt.xlabel('x')
plt.ylabel('f(x)')
plt.title('Косинусно-экспоненциальное распределение')
plt.savefig('cos_exp_clean.png')
