#include "lfw_loader.h"
#include "config.h"

namespace mobile_facenet
{
    lfw_loader::lfw_loader(const std::vector<std::string> &images_left, const std::vector<std::string> &images_right)
    {
        imgl_list = images_left;
        imgr_list = images_right;
    }

    torch::Tensor image2tensor(cv::Mat img)
    {
        img.convertTo(img, CV_32FC1);
        img = (img - 127.5) / 128.0;

        if (img.channels == 2)
        {
            auto channel = torch::from_blob(
                img.ptr(),
                {config.image_height, config.image_width},
                torch::kFloat);

            return torch::cat({channel, channel, channel})
                .view({3, config.image_height, config.image_width})
                .to(torch::kFloat);
        }

        std::vector<cv::Mat> channels(3);
        cv::split(img, channels);

        auto R = torch::from_blob(
            channels[2].ptr(),
            {config.image_height, config.image_width},
            torch::kFloat);
        auto G = torch::from_blob(
            channels[1].ptr(),
            {config.image_height, config.image_width},
            torch::kFloat);
        auto B = torch::from_blob(
            channels[0].ptr(),
            {config.image_height, config.image_width},
            torch::kFloat);

        return torch::cat({R, G, B})
            .view({3, config.image_height, config.image_width})
            .to(torch::kFloat);
    }

    torch::data::Example<> lfw_loader::get(size_t index)
    {
        cv::Mat image, imgl, imgr, imgl_fliped, imgr_fliped;

        imgl = cv::imread(imgl_list[index]);
        imgr = cv::imread(imgr_list[index]);

        assert(!imgl.empty() && imgl.channels() >= 2);
        assert(!imgr.empty() && imgr.channels() >= 2);

        cv::flip(imgl, imgl_fliped, 1);
        cv::flip(imgr, imgr_fliped, 1);

        std::vector<torch::Tensor> imgs;
        imgs.push_back(image2tensor(imgl));
        imgs.push_back(image2tensor(imgl_fliped));
        imgs.push_back(image2tensor(imgr));
        imgs.push_back(image2tensor(imgr_fliped));

        return imgs;
    }

    torch::optional<size_t> lfw_loader::size() const
    {
        return imgl_list.size();
    }
} // namespace mobile_facenet
