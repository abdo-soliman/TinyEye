#ifndef CASIA_WEBFACE_LOADER_H_H_
#define CASIA_WEBFACE_LOADER_H_H_

#include <vector>
#include <string>
#include <torch/torch.h>

namespace mobile_facenet
{
class CASIA_WebFace_loader : public torch::data::datasets::Dataset<CASIA_WebFace_loader>
{
public:
    CASIA_WebFace_loader(std::string root_dir, std::string maping_filename);
    torch::data::Example<> get(size_t index);
    torch::optional<size_t> size() const;

private:
    std::vector<std::string> images_list;
    std::vector<long> labels_list;
    long num_classes;
};
} // namespace mobile_facenet

#endif
