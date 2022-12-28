import math
import random as rand
import numpy
import statistics

import numpy as np
import scipy.stats as stats
from scipy.optimize import minimize as scipy_minimize
from scipy import integrate


V_FORM = 0.1
B1 = 1.03
DISPERSION = 0.39
EXCESS = 3.36

B4 = 2 * B1 * math.tan(B1)
B3 = V_FORM * B4 / 2 * math.exp(B4)
B2 = B4 / (2 + B4)

TRIM_MEAN_PARAMETERS = (0.05, 0.1, 0.15)
ROBUSTNESS_PARAMETERS = (0.1, 0.5, 1)

STANDARD_SCALE = 1
STANDARD_SHIFT = 0

results_folder = "./estimation_results/"


def cos_exp_dist():
    """
    Генерация случайной величины на основе косинусно-эконенциального распределения
    :return: значение случайной величины
    """
    r1 = rand.random()
    if r1 < V_FORM:
        r4 = rand.random()
        x2 = 1 - math.log(r4) / B4
        if r1 < V_FORM / 2:
            return x2
        else:
            return -x2
    else:
        while True:
            r2 = rand.random()
            r3 = rand.random()
            x1 = 2 * r2 - 1
            if r3 <= math.pow(math.cos(B1 * x1), 2):
                return x1


def cos_exp_density(x):
    if abs(x) <= 1:
        return B2 * math.cos(B1 * x)
    else:
        return B3 * math.exp(-B4 * abs(x))


def cos_exp_density_derivative(x):
    if abs(x) <= 1:
        return -B1 * B2 * math.sin(2 * B1 * x)
    else:
        return -B4 * B3 * math.exp(-B4 * abs(x))


def mle_loss_function(shift, data, scale):
    q = 0
    for x in data:
        q += -math.log(cos_exp_density((x - shift) / scale))
    return q


def radical_loss_function(shift, data, scale, delta):
    q = 0
    for x in data:
        q += -1 / pow(cos_exp_density(0), delta) * pow(cos_exp_density((x - shift) / scale), delta)
    return q


def generate_data(n: int, shift: float = STANDARD_SHIFT, scale: float = STANDARD_SCALE):
    data = [shift + scale * cos_exp_dist() for x in range(n)]
    data.sort()
    return data, [cos_exp_density((x - shift) / scale) / scale for x in data]


def generate_noisy_data(n: int, shift: float, scale, noise_level: float):
    if 0.5 <= noise_level <= 0:
        raise RuntimeError("Incorrect noise level")
    noisy_data = []
    for i in range(n):
        r = rand.random()
        if r <= 1 - noise_level:
            noisy_data.append(cos_exp_dist())
        else:
            noisy_data.append(shift + scale * cos_exp_dist())
    noisy_data.sort()
    noisy_density = [((1 - noise_level) * cos_exp_density(x) + noise_level * cos_exp_density((x - shift) / scale) / scale) for x in noisy_data]
    return noisy_data, noisy_density


def calculate_estimations(data, scale, estimate_shift=False):
    estimations = dict()
    estimations['mean'] = statistics.mean(data)
    print(f"Среднее: {estimations['mean']}")
    estimations['median'] = statistics.median(data)
    print(f"Медиана: {estimations['median']}")
    estimations['variance'] = statistics.variance(data, estimations['mean'])
    print(f"Дисперсия: {estimations['variance']}")
    estimations['skewness'] = stats.skew(data)
    print(f"Коэффициент асимметрии: {estimations['skewness']}")
    estimations['kurtosis'] = stats.kurtosis(data, fisher=False)
    print(f"Коэффициент эксцесса: {estimations['kurtosis']}")


    estimations['trim_mean'] = []
    for param in TRIM_MEAN_PARAMETERS:
        estimations['trim_mean'].append(stats.trim_mean(data, param))
    print(f"Усеченное среднее: {estimations['trim_mean']}")

    if estimate_shift:
        result = scipy_minimize(mle_loss_function, estimations['mean'], (data, scale), method='Nelder-Mead')
        if result.success:
            estimations['mle'] = result.x[0]
        else:
            print("MLE estimation error: ", result)
        print(f"ОМП: {estimations['mle']}")

        estimations['radical_estimates'] = list()
        for param in ROBUSTNESS_PARAMETERS:
            result = scipy_minimize(radical_loss_function, estimations['mean'], (data, scale, param), method='Nelder-Mead')
            estimations['radical_estimates'].append(result.x[0])

        print(f"Обобщенные радикальные оценки: {estimations['radical_estimates']}")
    return estimations


def mle_integrate_function(x):
    return math.pow(cos_exp_density_derivative(x), 2) / cos_exp_density(x)

def rad_integrate_function(x, delta=1):
    return math.pow(cos_exp_density_derivative(x), 2) * pow(cos_exp_density(x), delta-1)

def calculate_influence_functions_data(data, shift, scale):
    mean_influence = [y - shift for y in data]
    median_influence = [scale * math.copysign(1, y - shift) / (2 * cos_exp_density(0)) for y in data]

    trim_mean_influence = list()
    for param in TRIM_MEAN_PARAMETERS:
        k = numpy.quantile(generate_data(1000), (1-param))
        trim_param_inf = list()
        for y in data:
            value = (y - shift) / scale
            if value <= -k:
                inf = -k
            elif value >= k:
                inf = k
            else:
                inf = value
            inf *= 1 / (1 - 2 * param)
            trim_param_inf.append(inf)

        trim_mean_influence.append(trim_param_inf)

    # v = integrate.quad(mle_integrate_function, -np.inf, np.inf)
    # mle_influence = [(-scale * cos_exp_density_derivative((y-shift)/scale)
    #                     / cos_exp_density((y-shift)/scale))
    #                 / v[0] for y in data]

    # v = integrate.quad(rad_integrate_function, -np.inf, np.inf, args=(ROBUSTNESS_PARAMETERS[0],))
    # rad_influence = [-scale * cos_exp_density_derivative((y - shift) / scale)
    #        * pow(cos_exp_density((y - shift) / scale), ROBUSTNESS_PARAMETERS[0]-1)
    #        / v[0] for y in data]

    return mean_influence, median_influence, trim_mean_influence

def estimations_to_file(estimations: dict, output_file: str):
    with open(results_folder + output_file, 'w') as out_file:
        for estimation in estimations:
            if estimation in ('trim_mean', 'radical_estimates'):
                out_file.write(f"{estimation}:\n")
                for i, param in enumerate(TRIM_MEAN_PARAMETERS if estimation == 'trim_mean' else ROBUSTNESS_PARAMETERS):
                    out_file.write(f"{param} -> {estimations[estimation][i]}; ")
                out_file.write('\n')
            else:
                out_file.write(f"{estimation}: {estimations[estimation]}\n".capitalize())
