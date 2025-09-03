//
// Created by Arian Sadeghi on 9/2/25.
//

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <string>
#include <vector>
#include <memory>
#include "Element.h"
#include "Node.h"

class Serializer {
public:
    static void save_elements(const std::vector<std::shared_ptr<Element>>& elements, const std::string& filename);
    static std::vector<std::shared_ptr<Element>> load_elements(const std::string& filename);

    static void save_nodes(const std::vector<Node>& nodes, const std::string& filename);
    static std::vector<Node> load_nodes(const std::string& filename);
};

#endif // SERIALIZER_HPP
