import math
import matplotlib.pyplot as plt

import main as dist

# def

capacity = 10 ** 3
with_estimate = True
dirty_scale = 3
dirty_shift = 2 * math.sqrt(dist.DISPERSION)
noise_level = 0.2

clean_data = list()
clean_density = list()
noisy_data = list()
noisy_density = list()
dirty_data = list()
dirty_density = list()
for i in range(3):
    print(f"----- Iteration {i} -----")
    clean_data, clean_density = dist.generate_data(capacity, dist.STANDARD_SHIFT, dist.STANDARD_SCALE)
    print("Clean data generated")

    dirty_data, dirty_density = dist.generate_data(capacity, shift=dirty_shift, scale=dist.STANDARD_SCALE)

    noisy_data, noisy_density = dist.generate_noisy_data(capacity, shift=dirty_shift, scale=dist.STANDARD_SCALE, noise_level=noise_level)

    estim = dist.calculate_estimations(noisy_data, dist.STANDARD_SCALE, estimate_shift=with_estimate)
    dist.estimations_to_file(estim, f'noisy_asym_{capacity}_{i}.res')

fig = plt.figure()
fig.set_size_inches(15, 8)
plt.plot(clean_data, clean_density, label="Чистое распределение")
plt.plot(dirty_data, dirty_density, label="Засоряющее распределение")
plt.plot(noisy_data, noisy_density, label="Засоренное распределение")
plt.legend(loc='upper right')
plt.grid()
plt.xlabel('x')
plt.ylabel('f(x)')
plt.title(f'Асимметричное засорение с уровнем засорения {noise_level * 100}%')
plt.savefig(f'img/noisy_asym_{capacity}.png', dpi=200)