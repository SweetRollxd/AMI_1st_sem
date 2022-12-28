import random

import matplotlib.pyplot as plt
import numpy as np

import main as dist


capacity = 10 ** 3
# with_estimate = True

clean_data, clean_density = dist.generate_data(capacity, dist.STANDARD_SHIFT, dist.STANDARD_SCALE)
print("Clean data generated")
clean_data.sort()

# estim = dist.calculate_estimations(clean_data, dist.STANDARD_SCALE)
# dist.estimations_to_file(estim, f'clean_{capacity}_{i}.res')
a = min(clean_data)
b = max(clean_data)
y = np.linspace(a,b,capacity)
clean_data.sort()
mean_inf, \
    median_inf, \
    mean_trim_inf = dist.calculate_influence_functions_data(y, dist.STANDARD_SHIFT, dist.STANDARD_SCALE)

fig = plt.figure()
fig.set_size_inches(15, 8)
plt.plot(y, mean_inf, label="Среднее арифметическое")
plt.plot(y, median_inf, label="Медиана")
for i, inf in enumerate(mean_trim_inf):
    plt.plot(y, inf, label=f"Усеченное среднее {dist.TRIM_MEAN_PARAMETERS[i]}")
# plt.plot(y, mle_inf, label="ОМП")
plt.grid()
plt.legend()
# plt.xlabel('x')
# plt.ylabel('f(x)')
plt.title('Функции влияния')
plt.savefig('img/influence.png')