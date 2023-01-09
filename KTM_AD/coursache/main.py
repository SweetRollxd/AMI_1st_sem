import math
import numpy
from matplotlib import pyplot

from sample_generator import gen_sample

def calculate_criteria_stat(sample):
    sample_range = max(sample) - min(sample)

    sum = 0
    mean = numpy.mean(sample)
    for x in sample:
        sum += (x - mean) ** 2

    dispersion_estimate = 1 / (len(sample) - 1) * sum

    return sample_range / math.sqrt(dispersion_estimate)


def calculate_p_value(sample, iterations, mu=0, sigma=1):
    m = 0
    n = len(sample)
    x_stat = calculate_criteria_stat(sample)
    print(f"X-статистика критерия: {x_stat}")
    for i in range(0, iterations):
        y_sample = gen_sample(n, mu=mu, sigma=sigma)
        y_stat = calculate_criteria_stat(y_sample)
        if y_stat > x_stat:
            m += 1
    if m / iterations < (1 - m / iterations):
        return 2 * m / iterations
    else:
        return 2 * (1 - m / iterations)

def sample_from_file(path):
    with open(path, 'r', encoding='cp1251') as f:
        name = f.readline()
        n = int(f.readline().split(' ')[1])
        sample = list()
        for i in range(0, n):
            sample.append(float(f.readline()))

    return sample


if __name__ == "__main__":
    print("Проверка нормальности выборки критерием Дэвида-Хартли-Пирсона.")
    filename = input("Файл с выборкой: ")
    mu = int(input("Сдвиг: "))
    sigma = int(input("Масштаб: "))
    alfa = float(input("Вероятность ошибки первого рода: "))
    iterations = int(input("Количество повторений в методе Монте-Карло: "))

    x_sample = sample_from_file(filename)
    pyplot.hist(x_sample, bins=len(x_sample))
    pyplot.savefig("sample.png")
    p_value = calculate_p_value(x_sample, iterations, mu, sigma)

    print("p-value:", p_value)

    if p_value < alfa:
        print("Достигаемый уровень значимости менее вероятности ошибки первого рода. Гипотеза ОТКЛОНЯЕТСЯ.")
    else:
        print("Достигаемый уровень значимости более вероятности ошибки первого рода. Гипотеза НЕ ОТКЛОНЯЕТСЯ.")


