from os import listdir
import shutil
from os.path import join
import sys

mypath = sys.argv[1]
folders_count = 2000

if sys.argv[2]:
    folders_count = int(sys.argv[2])

folders = [folder for folder in listdir(mypath) if (join(mypath, folder))]

count = 0
for folder in listdir(mypath):
    folder = join(mypath, folder)
    count += 1
    if count > folders_count:
        shutil.rmtree(folder)

