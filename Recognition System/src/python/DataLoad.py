from __future__ import print_function, division
import os
import torch
import numpy as np
from torch.utils.data import Dataset, DataLoader
from torchvision import transforms, utils
import cv2
# Ignore warnings
import warnings
warnings.filterwarnings("ignore")


def read_info(info_path):
    train_images = []
    train_labels = []
    test_images = []
    test_labels = []
    with open(info_path, 'r') as input_file:
        lines = input_file.read().splitlines()
        for line in lines:
            image_path, label, type = line.split(" ")
            if type == "train":
                train_images.append(image_path)
                train_labels.append(int(label))
            elif type == "test":
                test_images.append(image_path)
                test_labels.append(int(label))

    train_data = {"data": train_images, "label": train_labels}
    test_data = {"data": test_images, "label": test_labels}
    return train_data, test_data


def img_loader(path):
    try:
        with open(path, 'rb') as f:
            img = cv2.imread(path)
            # img = cv2.resize(img, (112, 96))
            if len(img.shape) == 2:
                img = np.stack([img] * 3, 2)
            return img
    except IOError:
        print('Cannot load image ' + path)


class DataLoad(Dataset):

    def __init__(self, data, transform=None, loader=img_loader):
        self.data = data["data"]
        self.labels = data["label"]
        self.loader = loader
        self.transform = transform
        self.class_nums = len(np.unique(self.labels))
        print("dataset size: ", len(self.data), '/', self.class_nums)

    def __len__(self):
        return len(self.labels)

    def __getitem__(self, idx):
        img_name = "../"+self.data[idx]
        img = self.loader(img_name)
        label = self.labels[idx]
        # random flip with ratio of 0.5
        flip = np.random.choice(2) * 2 - 1
        if flip == 1:
            img = cv2.flip(img, 1)

        if self.transform is not None:
            img = self.transform(img)
        else:
            img = torch.from_numpy(img)

        return img, label
