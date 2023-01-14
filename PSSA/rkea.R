library(RKEA)
library(tm)

data("crude")
# keywords <- list(c("Diamond", "crude oil", "price"),
#                  c("OPEC", "oil", "price"),
#                  c("Texaco", "oil", "price", "decrease"),
#                  c("Marathon Petroleum", "crude", "decrease"),
#                  c("Houston Oil", "revenues", "decrease"),
#                  c("Kuwait", "OPEC", "quota"))
keywords = list(c("Diamond", "crude oil", "price"))

tmpdir = tempfile()
dir.create(tmpdir)
model = file.path(tmpdir, "crudeModel")
createModel(crude[1], keywords = keywords, model = model)
extracted_kw2 = extractKeywords(crude, model)
