import os
from imutils import paths
import cv2
from mtcnn import MTCNN
import datetime
import sys

print(sys.argv[1])
data_path = sys.argv[1]

data_dir = data_path.split("/")[-1]
if not(data_dir):
    data_dir = data_path.split("/")[-2]

images_paths = list(paths.list_images(data_path))
images_len = len(images_paths)
detector = MTCNN()

faces_count = 0
images_count = 0

start = datetime.datetime.now()
for image_path in images_paths:
    images_count += 1
    output_image_path = image_path.replace(data_dir, "faces", 1)
    #print("[",int((images_count/images_len)*100),"%]", images_count , "/", images_len, "Reading image", image_path)
    image = cv2.imread(os.path.join(os.getcwd(), image_path))
    print("[",int((images_count/images_len)*100),"%]", images_count , "/", images_len, "detecting faces...")
    faces = detector.detect_faces(image)
    print("[",int((images_count/images_len)*100),"%]", images_count , "/", images_len, "detected {} faces...".format(len(faces)))
    for i in range(len(faces)):
        # get coordinates
        if i > 0:
            break
        x1, y1, width, height = faces[i]['box']
        x2, y2 = x1 + width, y1 + height
        if x1 < 0:
            x1 = 0
        if x2 < 0:
            x2 = 0
        if y1 < 0:
            y1 = 0
        if y2 < 0:
            y2 = 0
        # save face
        print("[",int((images_count/images_len)*100),"%]", images_count , "/", images_len, "Writing image output", output_image_path)
        dirs_list = output_image_path.split("/")[:-1]
        out_dir = "/".join(dirs_list)
        if not(os.path.isdir(out_dir)):
            os.mkdir(out_dir)
        image = image[y1:y2, x1:x2]
        image = cv2.resize(image, (112, 112))
        cv2.imwrite(output_image_path, image)
        faces_count += 1

end = datetime.datetime.now()

print("processed {} images".format(faces_count))

print((start - end).seconds, " seconds")
