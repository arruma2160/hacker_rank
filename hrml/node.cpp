#include "node.h"
#include <sstream>

namespace HRML {

Node::Node(void)
    : is_closing_node_{false}, is_valid_{false}
{

}


Node::Node(std::string tag, bool is_closing)
    : tag_{tag}, is_closing_node_{is_closing}, is_valid_{true}
{

}


Node::Node(const std::string& s)
    : is_closing_node_{false}, is_valid_{true}
{
    std::string tag_name;
    std::map<std::string, std::string> attributes;

    std::istringstream iss{s}; // Input
    char c;

    c = static_cast<char>(iss.get());
    if (c != '<') {
        is_valid_ = false;
        return;
    }

    c = static_cast<char>(iss.get());
    while (isalnum(c) || c == '_' || c == '"' || c == '/') {
        tag_name += c;
        c = static_cast<char>(iss.get());
    }

    if (c == '>') {
        if (tag_name[0] == '/') {
            tag_ = tag_name.erase(0, 1);
            is_closing_node_ = true;
        } else
            tag_ = tag_name;
        is_valid_ = true;
        return;
    }

    if (c != ' ') {
        is_valid_ = false;
        return;
    }

    tag_ = tag_name;

    do {
        std::string attr_name;
        c = static_cast<char>(iss.get());
        while (isalnum(c) || c == '_' || c == '"') {
            if (c != '\\' && c != '"')
                attr_name += c;
            c = static_cast<char>(iss.get());
        }

        if (c != ' ')
            is_valid_ = false;

        c = static_cast<char>(iss.get());
        if (c != '=')
            is_valid_ = false;

        c = static_cast<char>(iss.get());
        if (c != ' ')
            is_valid_ = false;

        c = static_cast<char>(iss.get());
        if (c != '"')
            is_valid_ = false;

        if (!is_valid_)
            return;

        std::string attr_value;
        c = static_cast<char>(iss.get());
        while (c != '"') {
            if (c != '\\') attr_value += c;
            c = static_cast<char>(iss.get());
        }

        c = static_cast<char>(iss.get());
        if (c != ' ' && c != '>') {
            is_valid_ = false;
            return;
        }
        attributes_[attr_name] = attr_value;
    } while (c == ' ');
}


std::string
Node::tag(void) const
{
    return tag_;
}


std::string
Node::attribute(const std::string& key) const
{
    auto it = attributes_.find(key);
    return it != attributes_.end() ? it->second : "";
}


std::weak_ptr<Node>
Node::parent(void) const
{
    return parent_;
}


void
Node::set_parent(std::shared_ptr<Node> node)
{
    parent_ = node;
}


void
Node::add_child(std::shared_ptr<Node> node)
{
    children_.push_back(node);
}


std::weak_ptr<const Node>
Node::child(std::string childtag) const
{
    for (const auto& child : children_)
        if (child->tag() == childtag) return child;
    return std::weak_ptr<const Node>();
}

}
