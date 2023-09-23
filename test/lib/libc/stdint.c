#include <unity/unity.h>

#include <stdint.h>

void setUp() {}
void tearDown() {}

void test_fixed_width_integer_types() {
    TEST_ASSERT_EQUAL_INT(1, sizeof(int8_t));
    TEST_ASSERT_EQUAL_INT(2, sizeof(int16_t));
    TEST_ASSERT_EQUAL_INT(4, sizeof(int32_t));
    TEST_ASSERT_EQUAL_INT(8, sizeof(int64_t));

    TEST_ASSERT_EQUAL_INT(1, sizeof(uint8_t));
    TEST_ASSERT_EQUAL_INT(2, sizeof(uint16_t));
    TEST_ASSERT_EQUAL_INT(4, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_INT(8, sizeof(uint64_t));
}

void test_fixed_width_integer_limits() {
    TEST_ASSERT_EQUAL_INT8(127, INT8_MAX);
    TEST_ASSERT_EQUAL_INT8(-128, INT8_MIN);

    TEST_ASSERT_EQUAL_INT16(32767, INT16_MAX);
    TEST_ASSERT_EQUAL_INT16(-32768, INT16_MIN);

    TEST_ASSERT_EQUAL_INT32(2147483647, INT32_MAX);
    TEST_ASSERT_EQUAL_INT32(-2147483648, INT32_MIN);

    TEST_ASSERT_EQUAL_INT64(9223372036854775807, INT64_MAX);
    TEST_ASSERT_EQUAL_INT64((-9223372036854775807 - 1), INT64_MIN);

    TEST_ASSERT_EQUAL_UINT8(255, UINT8_MAX);
    TEST_ASSERT_EQUAL_UINT16(65535, UINT16_MAX);
    TEST_ASSERT_EQUAL_UINT32(4294967295, UINT32_MAX);
    TEST_ASSERT_EQUAL_UINT64(18446744073709551615, UINT64_MAX);



}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_fixed_width_integer_types);
    RUN_TEST(test_fixed_width_integer_limits);
    return UNITY_END();
}
