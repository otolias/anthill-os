#include <unity/unity.h>

#include <printf.c>

static char buffer[PRINTF_BUFFER_SIZE];

void setUp() {}
void tearDown() {}

void test_parse_int() {
    TEST_ASSERT_EQUAL_INT(3, _parse_int(132, 10, buffer, 0));
    TEST_ASSERT_EQUAL_INT(4, _parse_int(132, 16, buffer, 0));

    _parse_int(132, 10, buffer, 0);
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("132", buffer, 3, "Signed Integer");
    _parse_int(132, 16, buffer, 0);
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("0x84", buffer, 3, "Hexadecimal");
}

void test_parse_string() {
    TEST_ASSERT_EQUAL_INT(10, _parse_string("Entomology", buffer, 0));

    _parse_string("Entomology", buffer, 0);
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("Entomology", buffer, 10, "String");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_parse_int);
    RUN_TEST(test_parse_string);
    return UNITY_END();
}
