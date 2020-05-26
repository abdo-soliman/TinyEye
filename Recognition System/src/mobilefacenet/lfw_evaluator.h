#ifndef LFW_EVALUATOR_H_H_
#define LFW_EVALUATOR_H_H_

#include <string>
#include <vector>
#include "mobilefacenet.h"

namespace mobile_facenet
{
class lfw_evaluator
{
public:
    static void parse_pairs(const std::string &root, const std::string &foldername, const std::string &pairs_filename,
                            std::vector<std::string> &nameLs, std::vector<std::string> &nameRs, std::vector<int> &folds,
                            std::vector<int> &flags);

    static void evaluate(mobilefacenet& network,std::string root, std::string foldername,
                         std::string pairs_filename, std::string feature_save_dir);

private:
    static std::string format(const std::string &root, const std::string &foldername,
                              const std::string &filename, const std::string &number);
};
} // namespace mobile_facenet

#endif
