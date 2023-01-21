library(rjson)

messages = fromJSON(file = "reuters_tg/ChatExport_2023-01-19/result.json")$messages

output_path = paste(getwd(), "reuters_tg/full/", sep = '/')
for (i in 1:length(messages)){
  print(i)
  msg = messages[[i]]$text_entities
  write(paste(msg[[1]]$text, msg[[2]]$text, sep="\n"), 
        paste(output_path, i, '.txt', sep = ''))
}

