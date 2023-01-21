from glob import glob
import os
# from rake_nltk import Rake
from keybert import KeyBERT
import tracemalloc

from scipy.signal import peak_prominences


def extract_keywords_from_directory(model : KeyBERT, data_folder):
    path = data_folder + "*.txt"
    keywords_list = dict()
    for f in glob(path):
        keywords_list[f.split('/')[-1].split('.')[0]] = extract_keywords(model, f)[0:10]
    print(keywords_list)
    return keywords_list

def extract_keywords(model : KeyBERT, path):
    # path = "../" + data_folder + "*.txt"
    print(path)

    with open(path, 'r') as file:
        txt = file.read()
        # model.extract_keywords_from_text(txt)
        keywords = [x[0] for x in model.extract_keywords(txt, keyphrase_ngram_range=(1,2), top_n=10)]
        return keywords


def write_keywords(path, keywords):
    with open(path + ".key", 'w') as file:
        for keyword in keywords:
            file.write(keyword + "\n")
    return

def write_keywords_list(path, keywords_list):
    if not os.path.exists(path):
        os.mkdir(path)
    for name in keywords_list:
        print(path+name)
        write_keywords(path + name, keywords_list[name])
    return


if __name__ == '__main__':
    # r = Rake()
    kw_model = KeyBERT()

    # data_folder = "../reuters_tg/full/test_ds/"
    # data_folder = "../generated/repeats_sample/"
    # data_folder = "../reuters_tg/ukraine/tmp/"
    # data_folder = "../krapivin/tmp/"
    data_folder = "../articles/tmp/"

    tracemalloc.start()
    kws = extract_keywords_from_directory(kw_model, data_folder)
    current, peak = tracemalloc.get_traced_memory()
    print(f"Traced memory:  current {(current/1024):.2f} kb, peak {(peak/1024):.2f} kb")
    tracemalloc.stop()
    write_keywords_list(data_folder + "py_kw/", kws)
    for kw in kws:
        print(kws[kw])

