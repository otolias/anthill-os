#include <unity/unity.h>

#include <stdint.h>

void setUp() {}
void tearDown() {}

void test_integer_types() {
    TEST_ASSERT_EQUAL_INT(1, sizeof(int8_t));
    TEST_ASSERT_EQUAL_INT(2, sizeof(int16_t));
    TEST_ASSERT_EQUAL_INT(4, sizeof(int32_t));
    TEST_ASSERT_EQUAL_INT(8, sizeof(int64_t));

    TEST_ASSERT_EQUAL_INT(1, sizeof(uint8_t));
    TEST_ASSERT_EQUAL_INT(2, sizeof(uint16_t));
    TEST_ASSERT_EQUAL_INT(4, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_INT(8, sizeof(uint64_t));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_integer_types);
    return UNITY_END();
}
