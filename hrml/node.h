#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <map>
#include <list>
#include <memory>

namespace HRML {

class Node {
    public:
        Node(void);
        Node(const std::string& source);
        Node(std::string tag, bool is_closing);

        bool is_closing_node(void) { return is_closing_node_; }
        bool is_valid(void) { return is_valid_; }
        void set_valid(void) { is_valid_ = true; }

        std::string tag(void) const;
        std::string attribute(const std::string& key) const;

        std::weak_ptr<Node> parent(void) const;
        void set_parent(std::shared_ptr<Node> node);

        void add_child(std::shared_ptr<Node> node);
        std::weak_ptr<const Node> child(std::string childtag) const;

    private:
        std::string tag_;
        std::map<std::string, std::string> attributes_;
        bool is_closing_node_;
        bool is_valid_;

        std::list<std::shared_ptr<Node>> children_;
        mutable std::weak_ptr<Node> parent_;
};

}
#endif
