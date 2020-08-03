#ifndef INPUT_HPP_
#define INPUT_HPP_

#include "node.h"

#include <memory>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <istream>

namespace HRML {

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

}  /* <-- end of namespace hrml */
#endif
