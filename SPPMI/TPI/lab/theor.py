import matplotlib.pyplot as plt

import main as dist

capacity = 10 ** 7
with_estimate = True
clean_data, clean_density = dist.generate_data(capacity, dist.STANDARD_SHIFT, dist.STANDARD_SCALE)
print("Clean data generated")
estim = dist.calculate_estimations(clean_data, dist.STANDARD_SCALE, estimate_shift=with_estimate)
dist.estimations_to_file(estim, f'clean_{capacity}.res')

plt.plot(clean_data, clean_density)
plt.grid()
plt.xlabel('x')
plt.ylabel('f(x)')
plt.title(f'Косинусно-экспоненциальное распределение N = 10^7')
plt.savefig(f'clear_{capacity}.png')