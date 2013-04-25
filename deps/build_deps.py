import csv
import os
import subprocess


def get_urls_from_csv(csv_filename):
    urls = []
    with open(csv_filename, 'rb') as csvfile:
        deps_reader = csv.reader(csvfile, delimiter=',', quotechar='|')
        for row in deps_reader:
            print str(row)
            urls.append(row[2])
    print("urls :" + str(urls))
    return urls


def download_everything(urls, dl_dir):
    os.mkdir(dl_dir)
    cwd = os.getcwd()
    os.chdir(dl_dir)
    for url in urls:
        print("Downloading " + url)
        subprocess.call(["wget", url])
    os.chdir(cwd)


def normalize_filenames(dl_dir):
    cwd = os.getcwd()
    os.chdir(dl_dir)
    for orig_filename in os.listdir("."):
        if(orig_filename.find("?") != -1):
            os.rename(orig_filename, orig_filename.split("?")[0])
    os.chdir(cwd)


def extract_all(dl_dir):
    cwd = os.getcwd()
    os.chdir(dl_dir)
    for filename in os.listdir("."):
        if filename.endswith("tar.gz") or filename.endswith("tgz"):
            subprocess.call(["tar", "-zxvf", filename])
        elif filename.endswith("zip"):
            subprocess.call(["unzip", filename])
        elif filename.endswith("tbz2"):
            subprocess.call(["tar", "-jxvf", filename])
    os.chdir(cwd)


def build_sdl(build_dir, lib_dir):
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["./configure"])
    subprocess.call(["make"])
    subprocess.call(["cp", "build/.libs/libSDL.so", lib_dir])
    os.chdir(cwd)


dl_dir = "downs"
#subprocess.call(["rm", "-rf", dl_dir])
#download_everything(get_urls_from_csv("deps.csv"), dl_dir)
#normalize_filenames(dl_dir)
#extract_all(dl_dir)

lib_dir = "lib"
subprocess.call(["rm", "-rf", lib_dir])
os.mkdir(lib_dir)
relative_lib_dir = "../../%s" % lib_dir
build_sdl(os.path.join(dl_dir, "SDL-1.2.15",), relative_lib_dir)
