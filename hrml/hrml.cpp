#include "hrml.h"
#include <sstream>
#include <stdexcept>

namespace HRML {

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
