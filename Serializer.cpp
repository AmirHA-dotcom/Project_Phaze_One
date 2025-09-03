//
// Created by Arian Sadeghi on 9/2/25.
//

#include "Serializer.hpp"
#include "Libraries.h"
#include <fstream>


// فرض بر اینه که کلاس‌ها در فایل‌های جداگانه تعریف شدن و include شدن

void Serializer::save_elements(const std::vector<std::shared_ptr<Element>>& elements, const std::string& filename) {
    std::ofstream os(filename, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    archive(elements);
}

std::vector<std::shared_ptr<Element>> Serializer::load_elements(const std::string& filename) {
    std::ifstream is(filename, std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    std::vector<std::shared_ptr<Element>> elements;
    archive(elements);
    return elements;
}

void Serializer::save_nodes(const std::vector<Node>& nodes, const std::string& filename) {
    std::ofstream os(filename, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    archive(nodes);
}

std::vector<Node> Serializer::load_nodes(const std::string& filename) {
    std::ifstream is(filename, std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    std::vector<Node> nodes;
    archive(nodes);
    return nodes;
}
