from imutils import paths
import random

data_path = "../data/faces"
images_paths = list(paths.list_images(data_path))
random.shuffle(images_paths)

labels = []
for image_path in images_paths:
    labels.append(image_path.split("/")[-2])

data_len = len(images_paths)
train_len = int(0.9*data_len)

train_images_paths = images_paths[:train_len]
train_labels = labels[:train_len]
test_images_paths = images_paths[-train_len:]
test_labels = labels[-train_len:]

unique_labels = list(set(labels))
# unique_train_labels = list(set(train_labels))
# unique_test_labels = list(set(test_labels))

train_indeces = [unique_labels.index(x) for x in train_labels]
test_indeces = [unique_labels.index(x) for x in test_labels]

with open("../data/images.txt", "w+") as input_file:
    for i in range(len(train_images_paths)):
        input_file.write(
            train_images_paths[i] + " " + str(train_indeces[i]) + " train\n")
    for i in range(len(test_images_paths)):
        input_file.write(
            test_images_paths[i] + " " + str(test_indeces[i]) + " test\n")

with open("../data/map.txt", "w+") as input_file:
    for i in range(len(unique_labels)):
        input_file.write(
            unique_labels[i] + " " + str(i) + "\n")

with open("../data/faces_count.txt", "w+") as input_file:
    input_file.write(str(len(unique_labels)) + "\n")
