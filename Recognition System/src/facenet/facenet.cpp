#include "facenet.h"

void Facenet::normalize_faces(std::vector<cv::Mat> &faces)
{
    cv::Scalar mean_vector, std_vector;
    std::vector<uchar> face_array;
    for (int i = 0; i < faces.size(); i++)
    {
        if (faces[i].isContinuous())
            face_array.assign((uchar*)faces[i].datastart, (uchar*)faces[i].dataend);
        else
        {
            for (int i = 0; i < faces[i].rows; ++i)
                face_array.insert(face_array.end(), faces[i].ptr<uchar>(i), faces[i].ptr<uchar>(i)+faces[i].cols);
        }

        cv::meanStdDev(face_array, mean_vector, std_vector);
        faces[i] = (faces[i] - mean_vector.val[0]) / std_vector.val[0];
        face_array.clear();
    }
}

Facenet::Facenet(std::string model_path)
{

}

void Facenet::get_embeddings(std::vector<cv::Mat> &faces)
{
    normalize_faces(faces);
    
}
