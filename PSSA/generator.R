dictionary = scan(file = "en_dict.txt", character())

gen_path = "generated/repeats_sample"
repeats = 5
keywords_cnt = 10
n = 100
N = 10
test_N = 50
file.remove(list.files(gen_path, pattern = "*.txt", full.names = TRUE))
file.remove(list.files(paste(gen_path, "true_kw", sep="/"), pattern = "*.key", full.names = TRUE))
file.remove(list.files(paste(gen_path, "training", sep="/"), pattern = "*txt", full.names = TRUE))
file.remove(list.files(paste(gen_path, "training/kw", sep="/"), pattern = "*.key", full.names = TRUE))
# генерируем тренировочные данные
for (i in 1:test_N){
  file.remove(list.files(gen_path, pattern = "*.txt", full.names = TRUE))
  sample_vec = sample(x = dictionary, size = n)
  keywords = sample(sample_vec, size = keywords_cnt)
  for (word in keywords)
    for (j in 1:sample(1:repeats, size = 1)){
      sample_vec[sample(1:length(sample_vec), size = 1)] = word
    }
  
  write(paste(sample_vec, collapse = " "), file = sprintf("%s/training/%s.txt", gen_path, i))
  write(paste(keywords, collapse = "\n"), file = sprintf("%s/training/kw/%s_kw.key", gen_path, i))
}

# генерируем "реальные" данные
for (i in 1:N){
  sample_vec = sample(x = dictionary, size = n)
  keywords = sample(sample_vec, size = keywords_cnt)
  for (word in keywords)
    for (j in 1:sample(1:repeats, size = 1)){
      sample_vec[sample(1:length(sample_vec), size = 1)] = word
    }
  write(paste(keywords, collapse = "\n"), file = sprintf("%s/true_kw/%s_kw.key", gen_path, i))
  write(paste(sample_vec, collapse = " "), file = sprintf("%s/%s.txt", gen_path, i))
}

