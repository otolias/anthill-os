#include <unity/unity.h>

#include <kernel/string.h>

void setUp() {}
void tearDown() {}

void test_strncmp() {
    TEST_ASSERT_EQUAL_INT(0, strncmp("", "", 1));
    TEST_ASSERT_EQUAL_INT(0, strncmp("Entomology", "Entomology", 10));
    TEST_ASSERT_LESS_THAN_INT(0, strncmp("Entomo", "Entomology", 10));
    TEST_ASSERT_GREATER_THAN_INT(0, strncmp("Entomology", "En", 10));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_strncmp);
    return UNITY_END();
}
