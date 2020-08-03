#include <iostream>
#include <istream>
#include <sstream>

#include <string>
#include <map>
#include <list>
#include <vector>

#include <memory>
#include <stdexcept>

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


class Hrml {
    public:
        Hrml(void);

        unsigned number_source_nodes(void) const { return nsrcs_; }
        unsigned number_queries(void) const { return nqueries_; }

        std::weak_ptr<const Node> root_node(std::string roottag) const;
        friend std::istream& operator>>(std::istream& in, Hrml& hrml);
        friend std::ostream& operator<<(std::ostream& out, Hrml& hrml);

    private:
        unsigned nsrcs_;
        unsigned nqueries_;

        std::list<std::shared_ptr<Node>> nodes_;

        bool light_node_validation(std::string s);
        bool light_query_validation(std::string s);

        void init_nodes(const std::vector<std::string>& srcs);
        void answer_queries(const std::vector<std::string>& queries);

        std::vector<std::string> answers_;
};


/*
 * Exceptions
 * */

class HrmlNumericalDescription: public std::runtime_error {
    public:
        HrmlNumericalDescription(const std::string& msg="")
            :std::runtime_error(msg) {}
};


class HrmlNodeError: public std::runtime_error {
    public:
        HrmlNodeError(const std::string& msg="")
            :std::runtime_error(msg) {}
};


class HrmlQueryError: public std::runtime_error {
    public:
        HrmlQueryError(const std::string& msg="")
            :std::runtime_error(msg) {}
};


class HrmlParse: public std::runtime_error {
    public:
        HrmlParse(const std::string& msg="")
            :std::runtime_error(msg) {}
};


class HrmlFail: public std::runtime_error {
    public:
        HrmlFail(const std::string& msg="")
            :std::runtime_error(msg) {}
};


class HrmlIncompleteRead: public std::runtime_error {
    public:
        HrmlIncompleteRead(const std::string& msg="")
            :std::runtime_error(msg) {}
};


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

        if (!is_valid_)
            return;

        c = static_cast<char>(iss.get());
        if (c != '"')
            is_valid_ = false;

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


Hrml::Hrml(void)
    :nsrcs_{0}, nqueries_{0}
{

}


void
Hrml::init_nodes(const std::vector<std::string>& srcs)
{
    std::shared_ptr<Node> current_node;

    for (const auto& src : srcs) {
        auto node = std::make_shared<Node>(src);
        if (!node->is_valid())
            throw HrmlParse("Non-valid node: " + node->tag());

        if (node->is_closing_node()) {
            if (current_node == nullptr)
                throw HrmlParse("Error parsing - bad tag: " + node->tag());

            if (current_node->tag() == node->tag())
                current_node = current_node->parent().lock();
            else
                throw HrmlParse("Error parsing - bad tag: " + node->tag());
        }  else {
            if (current_node == nullptr) {
                nodes_.push_back(node);
            } else {
                current_node->add_child(node);
                node->set_parent(current_node);
            }
            current_node = node;
        }
    }
}


void
Hrml::answer_queries(const std::vector<std::string>& queries)
{

    for (const auto& query : queries)
    {
        char c;
        std::istringstream iss{query};
        std::string tag;
        std::string value;
        std::weak_ptr<const Node> node;
        bool root_search = true;

        while (!iss.eof()) {
            c = static_cast<char>(iss.get());
            if (c == '.' || c == '~') {
                if (root_search) {
                    node = root_node(tag);
                    if (!node.lock()) {
                        value = "";
                        break;
                    }
                    root_search = false;
                } else {
                    if (auto n = node.lock())
                        node = n->child(tag);
                    if (!node.lock()) {
                        value = "";
                        break;
                    }
                }
                if (c == '~') {
                    iss >> value;
                    if (auto n = node.lock())
                        value = n->attribute(value);
                    else
                        value = "";
                }
                tag.clear();
            } else {
                tag += c;
            }
        }
        answers_.push_back(!value.empty() ? value : std::string("Not Found!"));
    }

}


std::weak_ptr<const Node>
Hrml::root_node(std::string roottag) const
{
    for (const auto& child : nodes_)
        if (child->tag() == roottag) return child;
    return std::weak_ptr<const Node>();
}


bool
Hrml::light_node_validation(std::string s)
{
    return s.find('<') != std::string::npos &&
           s.find('>') != std::string::npos;
}


bool
Hrml::light_query_validation(std::string s)
{
    return s.find('~') != std::string::npos;
}


std::istream&
operator>>(std::istream& in, Hrml& hrml)
{
    std::string s;

    /*
     * Read hrml line number description (hrml nodes, hrml queries)
     */
    std::getline(in, s);
    std::istringstream iss{s};
    iss >> hrml.nsrcs_ >> hrml.nqueries_;
    if (iss.fail())
        throw HrmlNumericalDescription();

    /*
     * Read hrml nodes
     */
    std::vector<std::string> hrml_srcs;
    for(unsigned i = 0; i < hrml.nsrcs_ && !in.fail(); i++) {
        std::getline(in, s);
        if (!hrml.light_node_validation(s))
            throw HrmlNodeError();
        hrml_srcs.push_back(s);
    }

    hrml.init_nodes(hrml_srcs);

    /*
     * Read hrml queries
     */
    std::vector<std::string> hrml_queries;
    for(unsigned i = 0; i < hrml.nqueries_ && !in.fail(); i++) {
        std::getline(in, s);
        if (!hrml.light_query_validation(s))
            throw HrmlQueryError();
        hrml_queries.push_back(s);
    }

    if (in.fail())
        // Fail happened when reading hrml file
        throw HrmlFail();

    std::getline(in, s); // Control end-of-line
    if (!in.eof())
        // Wrong line number description in hrml file
        throw HrmlIncompleteRead();

    hrml.answer_queries(hrml_queries);

    return in;
}


std::ostream&
operator<<(std::ostream& out, Hrml& hrml)
{
    for (const auto& answer : hrml.answers_)
        out << answer + "\n";
    return out;
}

} /* <-- end namespace hrml */


int main(void)
{
    HRML::Hrml hrml;
    std::cin >> hrml;
    std::cout << hrml;

    return 0;
}
