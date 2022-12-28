import matplotlib.pyplot as plt

import main as dist


capacity = 10 ** 3
with_estimate = True

for i in range(3):
    print(f"----- Iteration {i} -----")
    clean_data, clean_density = dist.generate_data(capacity, dist.STANDARD_SHIFT, dist.STANDARD_SCALE)
    print("Clean data generated")

    # dirty_data, dirty_density = generate_data(CAPACITY, shift=STANDART_SHIFT, scale=5)
    #
    # noisy_data, noisy_density = generate_noisy_data(CAPACITY, shift=STANDART_SHIFT, scale=5, noise_level=0.15)

    estim = dist.calculate_estimations(clean_data, dist.STANDARD_SCALE, estimate_shift=with_estimate)
    dist.estimations_to_file(estim, f'clean_{capacity}_{i}.res')

# plt.plot(clean_data, clean_density, dirty_data, dirty_density, noisy_data, noisy_density)
# plt.plot(clean_data, clean_density)
# plt.grid()
# plt.xlabel('x')
# plt.ylabel('f(x)')
# plt.title('Косинусно-экспоненциальное распределение')
# plt.savefig('cos_exp.png')