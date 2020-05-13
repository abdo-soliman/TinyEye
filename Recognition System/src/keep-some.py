from os import listdir, remove
from os.path import join
import sys

mypath = sys.argv[1]
files_count = 15

if sys.argv[2]:
    files_count = int(sys.argv[2])

folders = [folder for folder in listdir(mypath) if (join(mypath, folder))]

for folder in listdir(mypath):
    folder = join(mypath, folder)
    count = 0
    for file_name in listdir(folder):
        file_name = join(folder, file_name)
        count += 1
        if count > files_count:
            remove(file_name)

