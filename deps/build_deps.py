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
        elif filename.endswith("tar.xz"):
            subprocess.call(["tar", "-xJf", filename])
    os.chdir(cwd)


def standard_build(build_dir, include_dir, lib_dir, lib_name):
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["./configure", 'CPPFLAGS="-I%s"' % include_dir, 'LDFLAGS="-L"%s' % lib_dir], shell=True)
    subprocess.call(["make"])
    subprocess.call(["cp", ".libs/%s" % lib_name, lib_dir])
    for include_file in filter(lambda a: a.endswith(".h"), os.listdir(".")):
        subprocess.call(["cp", "%s" % include_file, include_dir])
    os.chdir(cwd)


def build_sdl(build_dir, include_dir, lib_dir):
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["./configure"])
    subprocess.call(["make"])
    subprocess.call(["cp", "build/.libs/libSDL.so", lib_dir])
    for include_file in os.listdir("include"):
        subprocess.call(["cp", "include/%s" % include_file, include_dir])
    os.chdir(cwd)


def build_png(build_dir, include_dir, lib_dir):
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["./configure"])
    subprocess.call(["make"])
    subprocess.call(["cp", ".libs/libpng16.so", lib_dir])
    for include_file in filter(lambda a: a.endswith(".h"), os.listdir(".")):
        subprocess.call(["cp", "%s" % include_file, include_dir])
    os.chdir(cwd)


def build_sdl_image(build_dir, include_dir, lib_dir):
    standard_build(build_dir, include_dir, lib_dir, "libSDL_image.so")


def build_sdl_ttf(build_dir, include_dir, lib_dir):
    standard_build(build_dir, include_dir, lib_dir, "libSDL_ttf.so")


def build_freealut(build_dir, include_dir, lib_dir):
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["chmod", "+x", "autogen.sh"])
    subprocess.call(["/bin/bash", "./autogen.sh"])
    subprocess.call(["./configure"])
    subprocess.call(["make"])
    subprocess.call(["cp", "src/.libs/libalut.so", lib_dir])
    for include_file in os.listdir("include"):
        subprocess.call(["cp", "-rf", "include/%s" % include_file, include_dir])
    os.chdir(cwd)


def build_glew(build_dir, include_dir, lib_dir):
    cwd = os.getcwd()
    os.chdir(build_dir)
    subprocess.call(["make"])
    subprocess.call(["cp", "lib/libGLEW.so", lib_dir])
    subprocess.call(["cp", "lib/libGLEWmx.so", lib_dir])
    for include_file in os.listdir("include"):
        subprocess.call(["cp", "-rf", "include/%s" % include_file, include_dir])
    os.chdir(cwd)


def build_openal(build_dir, include_dir, lib_dir):
    cwd = os.getcwd()
    os.chdir(os.path.join(build_dir, "build"))
    subprocess.call(["cmake", "../"])
    subprocess.call(["make"])
    # Back to source dir
    os.chdir(os.path.join("../"))
    subprocess.call(["cp", "build/libopenal.so", lib_dir])
    for include_file in os.listdir("include"):
        subprocess.call(["cp", "-rf", "include/%s" % include_file, include_dir])
    os.chdir(cwd)


dl_dir = "downs"
subprocess.call(["rm", "-rf", dl_dir])
download_everything(get_urls_from_csv("deps.csv"), dl_dir)
normalize_filenames(dl_dir)
extract_all(dl_dir)

lib_dir = "lib"
subprocess.call(["rm", "-rf", lib_dir])
os.mkdir(lib_dir)
include_dir = "include"
subprocess.call(["rm", "-rf", include_dir])
os.mkdir(include_dir)

relative_lib_dir = "../../%s" % lib_dir
relative_include_dir = "../../%s" % include_dir

build_sdl(os.path.join(dl_dir, "SDL-1.2.15",), relative_include_dir, relative_lib_dir)
build_png(os.path.join(dl_dir, "libpng-1.6.1",), relative_include_dir, relative_lib_dir)
build_sdl_image(os.path.join(dl_dir, "SDL_image-1.2.12"), relative_include_dir, relative_lib_dir)
build_sdl_ttf(os.path.join(dl_dir, "SDL_ttf-2.0.11"), relative_include_dir, relative_lib_dir)
build_glew(os.path.join(dl_dir, "glew-1.9.0"), relative_include_dir, relative_lib_dir)
build_openal(os.path.join(dl_dir, "openal-soft-1.13"), relative_include_dir, relative_lib_dir)
build_freealut(os.path.join(dl_dir, "freealut-1.1.0-src"), relative_include_dir, relative_lib_dir)
