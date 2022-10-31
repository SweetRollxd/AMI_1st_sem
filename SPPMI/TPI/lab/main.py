# importing the required module
import math
import random as rand

import matplotlib.pyplot as plt

V_FORM = 0.1
B1 = 1.03
DISPERSION = 0.39
EXCESS = 3.36

B4 = 2 * B1 * math.tan(B1)


def cos_exp_distribution():
    r1 = rand.random()
    if r1 >= V_FORM:
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



res = []
for i in range(0, 1000):
    res_value = cos_exp_distribution()
    print(f'{i}-ая итерация. Результат: {res_value}')
    res.append(res_value)

    # print(cos_exp_distribution())
print(res)
# # plotting the points
# plt.plot(res)
plt.hist(res, histtype='step', cumulative=True, bins=len(res))
# naming the x axis
plt.xlabel('x - axis')
# naming the y axis
plt.ylabel('y - axis')

# giving a title to my graph
plt.title('Косинусно-экспоненциальное распределение')

# function to show the plot
plt.savefig('cos_exp_clean.png')