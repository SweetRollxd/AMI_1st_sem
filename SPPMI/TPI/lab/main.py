# importing the required module
import math
import random as rand
from scipy.stats import gaussian_kde

import matplotlib.pyplot as plt

V_FORM = 0.1
B1 = 1.03
DISPERSION = 0.39
EXCESS = 3.36
CAPACITY = 1000

B4 = 2 * B1 * math.tan(B1)

def cos_exp_clean(v, b1, b4):
    """
    Генерация случайной величины на основе косинусно-эконенциального распределения
    :param v: параметр формы
    :param b1: параметр b1
    :param b4: параметр b4
    :return: значение случайной величины
    """
    r1 = rand.random()
    if r1 >= v:
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

res = []
for i in range(0, CAPACITY):
    res_value = cos_exp_clean(V_FORM, B1, B4)
    # print(f'{i}-ая итерация. Результат: {res_value}')
    res.append(res_value)

    # print(cos_exp_distribution())
# print(res)
density = gaussian_kde(res)
# # plotting the points
plt.plot(density)
# plt.hist(res, histtype='step', density=True, cumulative=True, bins=len(res))
#plt.hist(res, density=True, histtype='stepfilled', bins=len(res))
# naming the x axis
plt.xlabel('x - axis')
# naming the y axis
plt.ylabel('y - axis')

# giving a title to my graph
plt.title('Косинусно-экспоненциальное распределение')

# function to show the plot
plt.savefig('cos_exp_clean.png')