#include<gtest/gtest.h>
#include <iostream>

class Complex {
    public:
        int a, b;
};


Complex operator+(const Complex& x, const Complex& y)
{
    return Complex{x.a+y.a, x.b+y.b};
}


std::ostream& operator<<(std::ostream& out, const Complex& x)
{
    out << (x.a == 0 ? "" : std::to_string(x.a));
    if (x.b == 0)
        out << "";
    else if (x.b > 0)
        out << "+i" + std::to_string(x.b);
    else
        out << "-i" + std::to_string(-x.b);
    return out;
}


bool operator==(const Complex& x, const Complex& y)
{
    return (x.a == y.a) && (x.b == y.b);
}


TEST(testcomplex, sum_pos) {
    Complex x{1, 2};
    Complex y{1, 2};

    Complex response{2, 4};

    ASSERT_TRUE(x+y == response) << "Result: " << x+y;
}


TEST(testcomplex, sum_neg_imag) {
    Complex x{1, 2};
    Complex y{1, -2};

    Complex response{2, 0};

    ASSERT_TRUE(x+y == response) << "Result: " << x+y;
}


TEST(testcomplex, sum_neg_real) {
    Complex x{1, 2};
    Complex y{-1, 2};

    Complex response{0, 4};

    ASSERT_TRUE(x+y == response) << "Result: " << x+y;
}


TEST(testcomplex, out_both_positive) {
    Complex out{2, 4};
    std::ostringstream oss;
    oss << out;

    ASSERT_EQ(oss.str(), "2+i4");
}


TEST(testcomplex, out_imaginary_zero) {
    Complex out{2, 0};

    std::ostringstream oss;
    oss << out;

    ASSERT_EQ(oss.str(), "2");
}


TEST(testcomplex, out_real_zero) {
    Complex out{0, 4};

    std::ostringstream oss;
    oss << out;

    ASSERT_EQ(oss.str(), "+i4");
}


TEST(testcomplex, out_real_neg) {
    Complex out{-1, 4};

    std::ostringstream oss;
    oss << out;

    ASSERT_EQ(oss.str(), "-1+i4");
}


TEST(testcomplex, out_imaginary_negative) {
    Complex out{1, -4};

    std::ostringstream oss;
    oss << out;

    ASSERT_TRUE(oss.str() == "1-i4") << "Result: " << oss.str();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
