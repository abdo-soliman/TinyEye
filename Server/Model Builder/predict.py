import os
import cv2
import pickle
import numpy as np
from PIL import Image
from numpy import load
from numpy import asarray
from sklearn.svm import SVC
from numpy import expand_dims
from mtcnn.mtcnn import MTCNN
from keras.models import load_model
from argparse import ArgumentParser
from sklearn.preprocessing import LabelEncoder


FACENET_MODEL_PATH = os.path.join(os.getcwd(), 'models')
EMBEDDINGS_PATH = os.path.join(os.getcwd(), 'models')
SVM_MODEL_PATH = os.path.join(os.getcwd(), 'models')
FACENET_MODEL = 'facenet_keras.h5'

detector = MTCNN()  # Creating instance from the class MTCNN

# Installing facenet_keras.h5
face_full_path = os.path.join(FACENET_MODEL_PATH, FACENET_MODEL)
model = None

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
    print(sample.shape())
    exit()
    print("Shape of normalized image", samples.shape)
    yhat = model.predict(samples)  # make prediction to get embedding
    print("Shape of yhat", yhat.shape)
    return yhat[0]


def multiple_faces(filename, result_filename="result.jpg", required_size=(160, 160)):

    # MUSTAFA SHOULD EDIT THIS, WE WANT TO INVERSE TRANSFORM THE ENCODING
    data = load(EMBEDDINGS_PATH + '/Embeddings-dataset.npz')
    trainy = data['arr_1']
    out_encoder = LabelEncoder()
    out_encoder.fit(trainy)
    trainy = out_encoder.transform(trainy)

    # LOAD THE MODEL
    print("TESTING ON AN IMAGE")
    print("LOADING THE MODEL...")
    svm_model = pickle.load(open(SVM_MODEL_PATH + '/svm_model.sav', 'rb'))
    print("DONE LOADING THE MODEL!")
    print("LOADING THE IMAGE...")
    image = Image.open(filename)  # load image from file
    print("DONE LOADING THE IMAGE!")
    image = image.convert('RGB')  # convert to RGB, if needed
    pixels = asarray(image)  # convert to array
    results = detector.detect_faces(pixels)  # detect faces in the image

    if(len(results) == 0):
        return False

    # LOOP OVER ALL FOUND FACE AND ANNOTATE THEM
    for i in range(len(results)):

        # extract the bounding box from the i-th first face
        x1, y1, width, height = results[i]['box']
        x1, y1 = abs(x1), abs(y1)
        x2, y2 = x1 + width, y1 + height
        face = pixels[y1:y2, x1:x2]  # extract the face
        image = Image.fromarray(face)
        image = image.resize(required_size)  # resize pixels to the model size
        face_array = asarray(image)
        face_emb = get_embedding(model, face_array)  # get the embeddings
        samples = expand_dims(face_emb, axis=0)
        prediction = svm_model.predict(samples)
        predict_name = out_encoder.inverse_transform(prediction)
        font = cv2.FONT_HERSHEY_TRIPLEX
        cv2.putText(pixels, predict_name[0],
                    (x1, y1), font, 2, (255, 255, 0), 2)

    print("SAVING OUTPUT IMAGE...")
    cv2.imwrite(result_filename, pixels)
    print("IMAGE SAVED TO: ", result_filename)
    return result_filename

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument('-i', '--input', default="input.jpg", type=str, help='path to input image for prediction')
    parser.add_argument('-o', '--output', default="result.jpg", type=str, help='path to write prediction result')
    parser.add_argument('-fp', '--facenet_model_path', default=FACENET_MODEL_PATH, type=str, help='path to the directory containing facenet keras model')
    parser.add_argument('-e', '--embeddings_path', default=EMBEDDINGS_PATH, type=str, help='path to write embeddings training dataset')
    parser.add_argument('-s', '--svm_model_path', default=SVM_MODEL_PATH, type=str, help='path to write svm model')
    parser.add_argument('-fn', '--facenet_model', default=FACENET_MODEL, type=str, help='facenet model name')

    args = parser.parse_args()
    input_file = args.input
    output_file = args.output
    FACENET_MODEL_PATH = args.facenet_model_path
    EMBEDDINGS_PATH = args.embeddings_path
    SVM_MODEL_PATH = args.svm_model_path
    FACENET_MODEL = args.facenet_model

    face_full_path = os.path.join(FACENET_MODEL_PATH, FACENET_MODEL)
    model = load_model(face_full_path)
    multiple_faces(input_file, result_filename=output_file)
