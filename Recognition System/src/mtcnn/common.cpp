#include "mtcnn.h"

namespace mtcnn
{
void MTCNN::non_maximum_suppression(vector<Bbox> boxes, double threshold, char method, vector<Bbox> &result)
{
    if (boxes.empty())
        return;

    if (!(method == 'u' || method == 'U' || method == 'm' || method == 'M'))
        throw "Invalid method for non maximum suppression must be either m (min) or u (union)";

    vector<double> areas;
    vector<double> scores;

    for (const auto &box : boxes)
    {
        scores.push_back(box.score);
        areas.push_back(box.width * box.height);
    }

    size_t index, counter;
    double width, height, inter;
    std::vector<double> o;
    std::list<size_t> sorted_scores = argsort(scores);

    while (!sorted_scores.empty())
    {
        index = sorted_scores.back();
        sorted_scores.pop_back();
        result.push_back(boxes[index]);

        for (const auto &score : sorted_scores)
        {
            if (boxes[index].x1 >= boxes[score].x1)
            {
                if (boxes[index].x2 <= boxes[score].x2)
                    width = boxes[index].x2 - boxes[index].x1 + 1;
                else
                    width = boxes[score].x2 - boxes[index].x1 + 1;
            }
            else
            {
                if (boxes[index].x2 <= boxes[score].x2)
                    width = boxes[index].x2 - boxes[score].x1 + 1;
                else
                    width = boxes[score].x2 - boxes[score].x1 + 1;
            }

            if (boxes[index].y1 >= boxes[score].y1)
            {
                if (boxes[index].y2 <= boxes[score].y2)
                    height = boxes[index].y2 - boxes[index].y1 + 1;
                else
                    height = boxes[score].y2 - boxes[index].y1 + 1;
            }
            else
            {
                if (boxes[index].y2 <= boxes[score].y2)
                    height = boxes[index].y2 - boxes[score].y1 + 1;
                else
                    height = boxes[score].y2 - boxes[score].y1 + 1;
            }

            width = (width >= 0) ? width : 0;
            height = (height >= 0) ? height : 0;
            inter = width * height;
            if (method == 'm' || method == 'M')
            {
                if (areas[index] <= areas[score])
                    o.push_back(inter / areas[index]);
                else
                    o.push_back(inter / areas[score]);
            }
            else
                o.push_back(inter / (areas[score] + areas[index] - inter));
        }

        counter = 0;
        sorted_scores.remove_if([&o, &threshold, &counter](size_t n) { return o[counter++] > threshold; });
        o.clear();
    }
}

void MTCNN::box_regress(std::vector<Bbox> &boxes, int stage)
{
    float regw, regh;
    for (auto &box : boxes)
    {
        regw = box.y2 - box.y1;
        regw += (stage == 1) ? 0 : 1;
        regh = box.x2 - box.x1;
        regh += (stage == 1) ? 0 : 1;

        box.y1 = box.y1 + regw * box.reg[0];
        box.x1 = box.x1 + regh * box.reg[1];
        box.y2 = box.y2 + regw * box.reg[2];
        box.x2 = box.x2 + regh * box.reg[3];

        box.width = box.x2 - box.x1 + 1;
        box.height = box.y2 - box.y1 + 1;
    }
}

void MTCNN::box_to_square(const vector<Bbox> &boxes)
{
    int max_side_length = 0;
    regrec_1_result = vector<Bbox>(boxes.size());
    for (size_t i = 0; i < boxes.size(); i++)
    {
        max_side_length = (boxes[i].height > boxes[i].width) ? boxes[i].height : boxes[i].width;
        regrec_1_result[i].x1 = static_cast<int>(boxes[i].x1 + (boxes[i].width - max_side_length) * 0.5);
        regrec_1_result[i].y1 = static_cast<int>(boxes[i].y1 + (boxes[i].height - max_side_length) * 0.5);
        regrec_1_result[i].x2 = regrec_1_result[i].x1 + max_side_length;
        regrec_1_result[i].y2 = regrec_1_result[i].y1 + max_side_length;

        regrec_1_result[i].width = regrec_1_result[i].x2 - regrec_1_result[i].x1 + 1;
        regrec_1_result[i].height = regrec_1_result[i].y2 - regrec_1_result[i].y1 + 1;

        regrec_1_result[i].score = boxes[i].score;
        regrec_1_result[i].reg = boxes[i].reg;
    }
}

void MTCNN::padding(const vector<Bbox> &boxes, int img_w, int img_h)
{
    padding_1_result = vector<Bbox>(boxes.size());
    for (size_t i = 0; i < boxes.size(); i++)
    {
        padding_1_result[i].y2 = (boxes[i].y2 >= img_w) ? img_w : boxes[i].y2;
        padding_1_result[i].x2 = (boxes[i].x2 >= img_h) ? img_h : boxes[i].x2;
        padding_1_result[i].y1 = (boxes[i].y1 < 1) ? 1 : boxes[i].y1;
        padding_1_result[i].x1 = (boxes[i].x1 < 1) ? 1 : boxes[i].x1;

        padding_1_result[i].width = padding_1_result[i].x2 - padding_1_result[i].x1 + 1;
        padding_1_result[i].height = padding_1_result[i].y2 - padding_1_result[i].y1 + 1;

        padding_1_result[i].score = boxes[i].score;
        padding_1_result[i].reg = boxes[i].reg;
    }
}
} // namespace mtcnn
