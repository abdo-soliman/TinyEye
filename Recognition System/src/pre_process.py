import os
from imutils import paths
import cv2
from mtcnn import MTCNN
import datetime

data_path = "../data/lfw"

images_paths = list(paths.list_images(data_path))
detector = MTCNN()

faces_count = 0

start = datetime.datetime.now()
for image_path in images_paths:
    output_image_path = image_path.replace("lfw", "faces", 1)
    print("Reading image", image_path)
    image = cv2.imread(os.path.join(os.getcwd(), image_path))
    print("detecting faces...")
    faces = detector.detect_faces(image)
    print("detected {} faces...".format(len(faces)))
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
        print("Writing image output", output_image_path)
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