import random


def gen_sample(capacity, mu=0, sigma=1):
    return [random.normalvariate(mu=mu, sigma=sigma) for _ in range(0, capacity)]


def sample_to_file(sample, mu, sigma, path):
    with open(path, 'w', encoding='cp1251') as f:
        n = len(sample)
        f.write(f"Нормальное распределение с масштабом {sigma} и сдвигом {mu}\n")
        f.write(f"0 {n}\n")
        for x in sample:
            f.write(f"{x}\n")


if __name__ == '__main__':
    print("Создание нормально-распределенной выборки ...\nВведите следующие параметры.")
    try:
        capacity = int(input("Объем выборки: "))
        mu = int(input("Сдвиг: "))
        sigma = int(input("Масштаб: "))
        sample = gen_sample(capacity, mu, sigma)
        filename = input("Выходной файл: ")
        sample_to_file(sample, mu, sigma, filename)

    except Exception as e:
        print("Some error", e.__str__())
