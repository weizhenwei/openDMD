#include "Foo.h"
#include "gtest/gtest.h"

TEST(TFoo, Test) {
    ASSERT_EQ(6, add_foo(2, 4));
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

