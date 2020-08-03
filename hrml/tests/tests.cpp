#include<gtest/gtest.h>
#include<string>
#include<sstream>
#include<cctype>

#include "hrml.h"

using namespace HRML;


TEST(hrml_test, hrml_input_creation_correct_input_format) {
    std::istringstream in {
        "4 3\n" \
        "<tag1 value = \"HelloWorld\">\n" \
        "<tag2 name1 = \"Name1\" name2 = \"Name2\">\n" \
        "</tag2>\n" \
        "</tag1>\n" \
        "tag1.tag2~name1\n" \
        "tag1~name\n" \
        "tag1~value\n"
    };

    Hrml hrml;
    in >> hrml;

    ASSERT_EQ(hrml.number_source_nodes(), 4);
    ASSERT_EQ(hrml.number_queries(), 3);

    auto root_node = hrml.root_node("tag1").lock();
    ASSERT_TRUE(root_node != nullptr);
    ASSERT_TRUE(root_node->attribute("value") == "HelloWorld");

    auto child_node = root_node->child("tag2").lock();
    ASSERT_TRUE(child_node != nullptr);
    ASSERT_TRUE(child_node->attribute("name1") == "Name1");
    ASSERT_TRUE(child_node->attribute("name2") == "Name2");

    std::ostringstream out;
    out << hrml;

    ASSERT_EQ("Name1\nNot Found!\nHelloWorld\n", out.str())
        << "Out: " << out.str();
}

TEST(hrml_test, hrml_input_creation_wrong_input_number_of_lines) {
    std::istringstream input {
        "4 2\n" \
        "<tag1 value = \"HelloWorld\">\n" \
        "<tag2 name = \"Name1\">\n" \
        "</tag2>\n" \
        "</tag1>\n" \
        "tag1.tag2~name\n" \
        "tag1~name\n" \
        "tag1~value\n"
    };

    Hrml hrml;
    ASSERT_THROW(input >> hrml, HrmlIncompleteRead);
}


TEST(hrml_test, hrml_input_creation_validation_numerical_description) {
    std::istringstream input {
        "4 A\n" \
        "<tag1 value = \"HelloWorld\">\n" \
        "<tag2 name = \"Name1\">\n" \
        "</tag2>\n" \
        "</tag1>\n" \
        "tag1.tag2~name\n" \
        "tag1~name\n" \
        "tag1~value\n"
    };

    Hrml hrml;
    ASSERT_THROW(input >> hrml, HrmlNumericalDescription);
}


TEST(hrml_test, hrml_input_creation_validation_node) {
    std::istringstream input {
        "4 3\n" \
        "tag1.tag2~name\n" \
        "<tag2 name = \"Name1\">\n" \
        "</tag2>\n" \
        "</tag1>\n" \
        "tag1.tag2~name\n" \
        "tag1~name\n" \
        "tag1~value\n"
    };

    Hrml hrml;
    ASSERT_THROW(input >> hrml, HrmlNodeError);
}


TEST(hrml_test, hrml_input_validation_query) {
    std::istringstream input {
        "4 3\n" \
        "<tag1 value = \"HelloWorld\">\n" \
        "<tag2 name = \"Name1\">\n" \
        "</tag2>\n" \
        "</tag1>\n" \
        "<tag1 value = \"HelloWorld\">\n" \
        "tag1~name\n" \
        "tag1~value\n"
    };

    Hrml hrml;
    ASSERT_THROW(input >> hrml, HrmlQueryError);
}

TEST(hrml_test, hrml_node_parse_correct) {
    std::string ss = "<tag1 value = \"HelloWorld\">";
    Node node{ss};
    ASSERT_EQ(node.tag(), "tag1");
    ASSERT_EQ(node.attribute("value"), "HelloWorld");
}


TEST(hrml_test, hrml_node_parse_correct_just_tag) {
    std::string ss = "<tag1>";
    Node node{ss};
    ASSERT_EQ(node.tag(), "tag1");
}


TEST(hrml_test, hrml_node_parse_error_no_space_after_equal) {
    std::string ss = "<tag1 value =\"HelloWorld\">";
    Node node{ss};
    ASSERT_FALSE(node.is_valid());
}


TEST(hrml_test, hrml_node_parse_error_no_space_before_equal) {
    std::string ss = "<tag1 value= \"HelloWorld\">";
    Node node{ss};
    ASSERT_FALSE(node.is_valid());
}


TEST(hrml_test, hrml_node_parse_error_no_attribute_value) {
    std::string ss = "<tag1 value =>";
    Node node{ss};
    ASSERT_FALSE(node.is_valid());
}


TEST(hrml_test, hrml_node_parse_error_no_attribute_name) {
    std::string ss = "<tag1 = \"HelloWorld\">";
    Node node{ss};
    ASSERT_FALSE(node.is_valid());
}

TEST(hrml_hacker_rank, hrml_test_01) {
    std::istringstream in {
        "4 3\n" \
        "<tag1 value = \"HelloWorld\">\n" \
        "<tag2 name = \"Name1\">\n" \
        "</tag2>\n" \
        "</tag1>\n" \
        "tag1.tag2~name\n" \
        "tag1~name\n" \
        "tag1~value\n"
    };
    Hrml hrml;
    in >> hrml;

    std::ostringstream out;
    out << hrml;

    std::string expect = "Name1\nNot Found!\nHelloWorld\n";
    ASSERT_EQ(expect, out.str()) << "Output: " << out.str();
}

TEST(hrml_hacker_rank, hrml_test_05) {
    std::istringstream in {
        "20 19\n" \
        "<tag1 v1 = \"123\" v2 = \"43.4\" v3 = \"hello\">\n" \
        "</tag1>\n" \
        "<tag2 v4 = \"v2\" name = \"Tag2\">\n" \
        "<tag3 v1 = \"Hello\" v2 = \"World!\">\n" \
        "</tag3>\n" \
        "<tag4 v1 = \"Hello\" v2 = \"Universe!\">\n" \
        "</tag4>\n" \
        "</tag2>\n" \
        "<tag5>\n" \
        "<tag7 new_val = \"New\">\n" \
        "</tag7>\n" \
        "</tag5>\n" \
        "<tag6>\n" \
        "<tag8 intval = \"34\" floatval = \"9.845\">\n" \
        "<ntag nv = \"4$\">\n" \
        "<nvtag nv = \"6$\">\n" \
        "</nvtag>\n" \
        "</ntag>\n" \
        "</tag8>\n" \
        "</tag6>\n" \
        "tag1~v1\n" \
        "tag1~v2\n" \
        "tag1~v3\n" \
        "tag4~v2\n" \
        "tag2.tag4~v1\n" \
        "tag2.tag4~v2\n" \
        "tag2.tag3~v2\n" \
        "tag5.tag7~new_val\n" \
        "tag5~new_val\n" \
        "tag7~new_val\n" \
        "tag6.tag8~intval\n" \
        "tag6.tag8~floatval\n" \
        "tag6.tag8~val\n" \
        "tag8~intval\n" \
        "tag6.tag8.ntag~nv\n" \
        "tag6.tag8.ntag.nvtag~nv\n" \
        "tag6.tag8.nvtag~nv\n" \
        "randomtag~nv\n" \
        "tag283.tag21.den~jef"
    };

    Hrml hrml;
    in >> hrml;

    std::ostringstream out;
    out << hrml;

    std::string expect = "123\n" \
                         "43.4\n" \
                         "hello\n" \
                         "Not Found!\n" \
                         "Hello\n" \
                         "Universe!\n" \
                         "World!\n" \
                         "New\n" \
                         "Not Found!\n" \
                         "Not Found!\n" \
                         "34\n" \
                         "9.845\n" \
                         "Not Found!\n" \
                         "Not Found!\n" \
                         "4$\n" \
                         "6$\n" \
                         "Not Found!\n" \
                         "Not Found!\n" \
                         "Not Found!\n";
    ASSERT_EQ(expect, out.str()) << "Output: " << out.str();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
