import os
import sys
import pickle
from PIL import Image
from os import listdir
from os.path import isdir
from numpy import asarray
from sklearn.svm import SVC
from numpy import expand_dims
from mtcnn.mtcnn import MTCNN
from numpy import savez_compressed
from keras.models import load_model
from argparse import ArgumentParser
from sklearn.metrics import accuracy_score
from sklearn.preprocessing import Normalizer
from sklearn.preprocessing import LabelEncoder

FACENET_MODEL_PATH = os.path.join(os.getcwd(), 'models')
TRAINING_IMAGES_PATH = os.path.join(os.getcwd(), 'photos')
EMBEDDINGS_PATH = os.path.join(os.getcwd(), 'models')
SVM_MODEL_PATH = os.path.join(os.getcwd(), 'models')
FACENET_MODEL = 'facenet_keras.h5'

PATH_TO_ADDED_PERSON_FOLDER = os.path.join(os.getcwd(), 'photos', 'new_person')

detector = MTCNN()
model = None

def extract_face(filename, required_size=(160, 160)):
    '''
    THIS FUNCTION TAKES (ONE) IMAGE AS INPUT
    DETECT AND RETURN (ONE) FACE ARRAY RESIZED
    '''
    image = Image.open(filename)  # load image from file
    image = image.convert('RGB')  # convert to RGB, if needed
    pixels = asarray(image)  # convert to array
    results = detector.detect_faces(pixels)  # detect faces in the image
    if(len(results) == 0):
        return []
    # extract the bounding box from the first face
    x1, y1, width, height = results[0]['box']
    x1, y1 = abs(x1), abs(y1)
    x2, y2 = x1 + width, y1 + height
    face = pixels[y1:y2, x1:x2]  # extract the face
    image = Image.fromarray(face)
    image = image.resize(required_size)  # resize pixels to the model size
    face_array = asarray(image)
    return face_array


def load_faces(directory):
    '''
    THIS FUNCTION TAKES DIRECTORY OF (ONE) PERSON IMAGES AS INPUT
    OUTPUTS A FACE NUMPY ARRAY
    '''
    faces = list()
    # enumerate files
    for filename in listdir(directory):
        path = os.path.join(directory, filename)
        face = extract_face(path)  # get face
        if(len(face) == 0):
            continue
        faces.append(face)  # store
    return faces


def load_dataset(directory):
    '''
    THIS FUNCTION TAKE DATABASE DIRECTORY WITH PERSONS' NAMES AS SUBDIRECTORY
    RETURNS LABELS AND NUMPY ARRAYS
    '''
    X, y = list(), list()
    for subdir in listdir(directory):  # LOOP OVER SUB-FOLDERS, on per class
        path = os.path.join(directory, subdir)  # PATH = SUBFOLDER
        if not isdir(path):  # skip any files that might be in the dir
            print("WARNING: FILES EXIST IN THE DATA DIRECTORY (ONLY FOLDERS ARE READ)!")
            print("SKIPPING FILE", path, "...")
            continue

        faces = load_faces(path)  # load all faces in the subdirectory
        # create labels FOR THE PERSON, WE DO RANGE BECAUSE DETECTOR CAN MISS SOME IMAGES AND DETECT 0 FACES
        labels = [subdir for _ in range(len(faces))]
        print('   >LOADED %d EXAMPLES FOR CLASS: %s' %
              (len(faces), subdir))  # summarize progress
        X.extend(faces)  # store
        y.extend(labels)  # store
    return asarray(X), asarray(y)


def get_embedding(model, face_pixels):
    '''
    GET EMBEDDINGS FOR ONLY ONE FACE
    '''
    print("Shape of image", face_pixels.shape)
    face_pixels = face_pixels.astype('float32')  # scale pixel values
    # standardize pixel values across channels (global)
    mean, std = face_pixels.mean(), face_pixels.std()
    face_pixels = (face_pixels - mean) / std
    # transform face into one sample
    samples = expand_dims(face_pixels, axis=0)
    print("Shape of normalized image", samples.shape)
    yhat = model.predict(samples)  # make prediction to get embedding
    print("Shape of yhat", yhat.shape)
    return yhat[0]


def dump_classes(trainy):
    encoder = LabelEncoder()  # label encode targets
    encoder.fit(trainy)
    classes = list(encoder.classes_)
    encoding = list(encoder.transform(classes))
    filename = os.path.join(SVM_MODEL_PATH, 'classes_map.txt')
    with open(filename, 'w') as out_file:
        for i in range(len(classes)):
            if i == (len(classes) - 1):
                out_file.write(classes[i] + "=" + str(encoding[i]))
            else:
                out_file.write(classes[i] + "=" + str(encoding[i]) + '\n')


def svm_train(newTrainX, trainy):
    in_encoder = Normalizer(norm='l2')
    newTrainX = in_encoder.transform(newTrainX)
    out_encoder = LabelEncoder()  # label encode targets
    out_encoder.fit(trainy)
    trainy = out_encoder.transform(trainy)
    svm_model = SVC(kernel='linear', probability=True)
    svm_model.fit(newTrainX, trainy)
    print("SUCCESSFULLY FINISHED TRAINING THE SVM MODEL SUCCES!")

    yhat_train = svm_model.predict(newTrainX)
    score_train = accuracy_score(trainy, yhat_train)  # score
    print('ACCURACY: TRAIN=%.3f' % (score_train*100))

    # SAVING THE MODEL
    print("SAVING SVM MODEL TO:", os.path.join(SVM_MODEL_PATH, 'svm_model.sav'), ' ...')
    filename = os.path.join(SVM_MODEL_PATH, 'svm_model.sav')
    pickle.dump(svm_model, open(filename, 'wb'))
    print("DONE SAVING THE MODEL!")
    return


def train():
    # load train dataset
    print("STARTING THE TRAINING PROCESS...")
    print("DATA PATH IS:", TRAINING_IMAGES_PATH)

    if not isdir(TRAINING_IMAGES_PATH):
        print("ERROR: GIVEN PATH IS NOT A DIRECTORY, PATH IS:", TRAINING_IMAGES_PATH)
        print("TERMINATING...")
        sys.exit(0)
    trainX, trainy = load_dataset(TRAINING_IMAGES_PATH)
    print("SHAPE OF TRAINING EXAMPLES:", trainX.shape,
          "SHAPE OF LABELS:", trainy.shape)

    # CONVERT EACH FACE IN THE TRAIN SET TO AN EMBEDDING
    newTrainX = list()
    for face_pixels in trainX:
        embedding = get_embedding(model, face_pixels)
        newTrainX.append(embedding)
    newTrainX = asarray(newTrainX)
    print("SHAPE OF TRAINING EMBEDDINGS:", newTrainX.shape,
          "SHAPE OF LABELS:", trainy.shape)
    print("SAVING THE EMBEDDINGS NUMBY ARRAY...")
    # save arrays to one file in compressed format WILL BE USED WHEN WE ADD NEW PERSON TO THE DATABASE WE'LL LOAD THIS AND APPEND
    savez_compressed(os.path.join(EMBEDDINGS_PATH, 'Embeddings-dataset.npz'), newTrainX, trainy)
    dump_classes(trainy)
    
    print("STARTING TRAINING THE SVM MODEL...")
    svm_train(newTrainX, trainy)
    return

def train_add_person():
    # load train dataset
    print("Data is at:", PATH_TO_ADDED_PERSON_FOLDER)
    trainX, trainy = load_dataset(PATH_TO_ADDED_PERSON_FOLDER)
    print("SHAPE OF ADDED TRAINING EXAMPLES:",
          trainX.shape, "SHAPE OF LABELS:", trainy.shape)

    # CONVERT EACH FACE IN THE TRAIN SET TO AN EMBEDDING
    newTrainX = list()
    for face_pixels in trainX:
        embedding = get_embedding(model, face_pixels)
        newTrainX.append(embedding)
    newTrainX = asarray(newTrainX)
    print("SHAPE OF ADDED EMBEDDINGS:", newTrainX.shape,
          "SHAPE OF LABELS:", trainy.shape)

    print("SAVING THE NEW EMBEDDINGS NUMBY ARRAY...")
    savez_compressed(os.path.join(EMBEDDINGS_PATH, 'Embeddings-dataset.npz'), newTrainX, trainy)
    dump_classes(trainy)
    
    print("STARTING TRAINING THE NEW SVM MODEL...")
    svm_train(newTrainX, trainy)
    return

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('-m', '--mode', default="train", type=str, help='mode of operation either train (to train from scratch) or add (to a new person to an existing model)')
    parser.add_argument('-fp', '--facenet_model_path', default=FACENET_MODEL_PATH, type=str, help='path to the directory containing facenet keras model')
    parser.add_argument('-t', '--training_images_path', default=TRAINING_IMAGES_PATH, type=str, help='path to images dataset for training')
    parser.add_argument('-e', '--embeddings_path', default=EMBEDDINGS_PATH, type=str, help='path to write embeddings training dataset')
    parser.add_argument('-s', '--svm_model_path', default=SVM_MODEL_PATH, type=str, help='path to write svm model')
    parser.add_argument('-fn', '--facenet_model', default=FACENET_MODEL, type=str, help='facenet model name')
    parser.add_argument('-a', '--added_person_folder_path', default=PATH_TO_ADDED_PERSON_FOLDER, type=str, help='path to added person images folder')

    args = parser.parse_args()
    mode = args.mode
    FACENET_MODEL_PATH = args.facenet_model_path
    TRAINING_IMAGES_PATH = args.training_images_path
    EMBEDDINGS_PATH = args.embeddings_path
    SVM_MODEL_PATH = args.svm_model_path
    FACENET_MODEL = args.facenet_model

    model = load_model(os.path.join(FACENET_MODEL_PATH, FACENET_MODEL))
    if mode == "train":
        train()
    elif mode == "add":
        train_add_person()
    else:
        print("Error: Invalid mode of operation must be either train (to train from scratch) or add (to a new person to an existing model)")
