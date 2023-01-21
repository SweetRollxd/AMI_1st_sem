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

# path = "reuters_tg/full/test_ds/"

# path = "reuters_tg/ukraine/tmp/"
# path = "krapivin/tmp/"
path = "articles/tmp/"

r_kw = get_keywords(paste(path, "extracted_kw", sep = "/"))
py_kw = get_keywords(paste(path, "py_kw", sep = "/"))
# true_kw = get_keywords(paste(path, "true_kw", sep = "/"))
true_kw = get_keywords(path)
r_sim = keywords_similarity(r_kw, true_kw)
py_sim = keywords_similarity(py_kw, true_kw)
print(r_sim)
print(py_sim)
