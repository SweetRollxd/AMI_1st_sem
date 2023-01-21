# подключение библиотек RKEA для извлечения ключевых слов
# и tm для обработки текста
library(RKEA)
library(tm)
library(stringdist)

# функция чтения ключевых слов из файла
get_keywords <- function(path){
  kw = vector(mode = "list", length = length(list.files(path, pattern = "*.txt")))
  # print(path)
  i = 1
  for (file in list.files(path, pattern = "*.key", full.names = TRUE)){
    # print(file)
    kw[[i]] = c(scan(file, character(), quote = "", sep="\n"))
    i = i + 1
  }
  names(kw) = sapply(strsplit(list.files(path, pattern = "*.key"), '[.]'), '[', 1)
  # print(str(kw))
  return (kw)
}

# функция создания модели и обучения на тренировочных данных
train_model <- function(model_name, data_folder, keywords) {
  path = paste(getwd(), data_folder, sep = '/')
  # print(path)
  # формирование корпуса из текстовых файлов
  corp = Corpus(DirSource(directory = path, pattern = '*.txt'))
  # создание временного файла, хранящего конфигурацию модели
  tmpdir = tempfile()
  dir.create(tmpdir)
  model = file.path(tmpdir, model_name)
  res = createModel(corp, keywords = keywords, model = model, )
  print(res)
  return (model)
}

# функция извлечения ключевых слов из текстовых файлов
extract_keywords <- function(model, data_folder) {
  path = paste(getwd(), data_folder, sep = '/')
  print(path)
  test_corp = Corpus(DirSource(directory = path, pattern = '*.txt'))
  kw = extractKeywords(test_corp, model)
  names(kw) = sapply(strsplit(names(test_corp), '[.]'), '[', 1)
  return (kw)
}

# функция записи ключевыхслов в файл
write_keywords = function(path, keywords) {
  output_path = sprintf("%s/extracted_kw/", path)
  if (!dir.exists(output_path))
    dir.create(output_path)
  for (i in 1:length(keywords)){
    # print(i)
    write(keywords[[i]], 
          paste(output_path, names(keywords)[[i]], '_1.key', sep = ''))
  }
}

keywords_similarity = function(keywords1, keywords2){
  if (length(keywords1) != length(keywords2))
    stop("Lengths of vectors must be equal")
  
  lv_sim = vector(mode = "double", length = length(keywords1))
  # lv = vector()
  for (i in 1:length(keywords1)){
    # sapply(keywords1, )
    lv = vector(mode = "double", length = length(keywords1[[i]]))
    for (j in 1:length(keywords1[[i]]))
      # нормализуем расстояние Левенштейна
      lv[j] = min(stringdist(tolower(keywords1[[i]][j]), tolower(keywords2[[i]]), method = "lv") / 
                  pmax(nchar(keywords1[[i]][j]), nchar(keywords2[[i]])))
    # получаем сходство строк как обратную величину нормализованному расстоянию
    lv_sim[i] = 1 - mean(lv)
  }
  # print(lv_dist)
  # возвращаем усредненное значение сходства по всем документам
  return (lv_sim)
}

perform_test = function(model_name, train_folder, data_folder, true_keywords_folder){
  # train_keywords = get_keywords(paste(train_folder, "kw", sep = '/'))
  train_keywords = get_keywords(train_folder)
  # print(train_keywords)
  training_mem = mem_change({
    training_time = system.time({
      model = train_model(model_name = model_name, data_folder = train_folder, keywords = train_keywords)
    })
  })
  
  extract_mem = mem_change({
    extract_time = system.time({
      extracted_kw = extract_keywords(model, data_folder)
    })
  })
  print(extracted_kw)
  write_keywords(data_folder, extracted_kw)
  true_keywords = get_keywords(path = true_keywords_folder)
  print(true_keywords)
  similarity = keywords_similarity(extracted_kw, true_keywords)
  print("Keywords:")
  print(length(extracted_kw))
  print("Training time:")
  print(training_time[3])
  print("Extracting time:")
  print(extract_time[3])
  print("Training memory:")
  print(training_mem)
  print("Extracting memory:")
  print(extract_mem)
  print("Average similarity:")
  print(mean(similarity))
  # write(sprintf("Training time: %s\n
  #               Training memory: %s\n
  #               Extracting time: %s\n
  #               Extracting memory: %s\n
  #               Average similarity: %s\n", 
  #               training_time[3], 
  #               training_mem,
  #               extract_time[3],
  #               extract_mem,
  #               mean(similarity)),
  #       file = paste(model_name, ".res", sep = ""))
  
  unlink(dirname(model), recursive=TRUE)
  return (list(extracted_kw = extracted_kw, true_kw = true_keywords, similarity = similarity))
}

# сгенерированные модельные примеры
model_name = "generated"
train_folder = "generated/repeats_sample/training"
data_folder = "generated/repeats_sample/"
res = perform_test(model_name, train_folder, data_folder, paste(data_folder, "true_kw", sep=''))

# Reuters TG
model_name = "reuters_tg"
train_folder = "reuters_tg/full/test_ds/training"
data_folder = "reuters_tg/full/test_ds/"
res = perform_test(model_name, train_folder, data_folder, data_folder)

# Krapivin Dataset
model_name = "krapivin"
train_folder = "krapivin/training"
data_folder = "krapivin/tmp"
res = perform_test(model_name, train_folder, data_folder, data_folder)

# Customer Churn
model_name = "churn"
train_folder = "krapivin"
data_folder = "articles/tmp"
res = perform_test(model_name, train_folder, data_folder, data_folder)

