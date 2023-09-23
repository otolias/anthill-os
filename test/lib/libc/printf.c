#include <unity/unity.h>

#include <printf.c>

static char buffer[PRINTF_BUFFER_SIZE];

void setUp() {}
void tearDown() {}

void test_sprintf_signed_integers() {
    TEST_ASSERT_EQUAL_INT(1, sprintf(buffer, "%d", 0));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0", buffer, 2);

    TEST_ASSERT_EQUAL_INT(3, sprintf(buffer, "%d", INT8_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("127", buffer, 4);
    TEST_ASSERT_EQUAL_INT(4, sprintf(buffer, "%d", INT8_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("-128", buffer, 5);

    TEST_ASSERT_EQUAL_INT(5, sprintf(buffer, "%d", INT16_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("32767", buffer, 6);
    TEST_ASSERT_EQUAL_INT(6, sprintf(buffer, "%d", INT16_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("-32768", buffer, 7);

    TEST_ASSERT_EQUAL_INT(10, sprintf(buffer, "%d", INT32_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("2147483647", buffer, 11);
    TEST_ASSERT_EQUAL_INT(11, sprintf(buffer, "%d", INT32_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("-2147483648", buffer, 12);

    TEST_ASSERT_EQUAL_INT(10, sprintf(buffer, "%d", INT32_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("2147483647", buffer, 11);
    TEST_ASSERT_EQUAL_INT(11, sprintf(buffer, "%d", INT32_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("-2147483648", buffer, 12);
}

void test_sprintf_unsigned_integers() {
    TEST_ASSERT_EQUAL_INT(3, sprintf(buffer, "%u", UINT8_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("255", buffer, 4);
    TEST_ASSERT_EQUAL_INT(5, sprintf(buffer, "%u", UINT16_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("65535", buffer, 6);
    TEST_ASSERT_EQUAL_INT(10, sprintf(buffer, "%u", UINT32_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("4294967295", buffer, 11);
}

void test_sprintf_hexadecimals() {
    TEST_ASSERT_EQUAL_INT(1, sprintf(buffer, "%x", 0));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0", buffer, 2);

    TEST_ASSERT_EQUAL_INT(2, sprintf(buffer, "%x", INT8_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("7f", buffer, 3);
    TEST_ASSERT_EQUAL_INT(8, sprintf(buffer, "%x", INT8_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ffffff80", buffer, 9);

    TEST_ASSERT_EQUAL_INT(4, sprintf(buffer, "%x", INT16_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("7fff", buffer, 5);
    TEST_ASSERT_EQUAL_INT(8, sprintf(buffer, "%x", INT16_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ffff8000", buffer, 9);

    TEST_ASSERT_EQUAL_INT(8, sprintf(buffer, "%x", INT32_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("7fffffff", buffer, 9);
    TEST_ASSERT_EQUAL_INT(8, sprintf(buffer, "%x", INT32_MIN));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("80000000", buffer, 9);

    TEST_ASSERT_EQUAL_INT(2, sprintf(buffer, "%x", UINT8_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ff", buffer, 3);

    TEST_ASSERT_EQUAL_INT(4, sprintf(buffer, "%x", UINT16_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ffff", buffer, 5);
    TEST_ASSERT_EQUAL_INT(8, sprintf(buffer, "%x", UINT32_MAX));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ffffffff", buffer, 9);
}

void test_sprintf_character() {
    TEST_ASSERT_EQUAL_INT(1, sprintf(buffer, "%c", 'a'));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("a", buffer, 2);
    TEST_ASSERT_EQUAL_INT(1, sprintf(buffer, "%c", '1'));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("1", buffer, 2);
}

void test_sprintf_string_ascii_table() {
    TEST_ASSERT_EQUAL_INT(0, sprintf(buffer, "%s", ""));
    TEST_ASSERT_EQUAL_INT(1, sprintf(buffer, "%s", "\n"));

    TEST_ASSERT_EQUAL_INT(16, sprintf(buffer, "%s", " !\"#$%&'()*+,-./"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY(" !\"#$%&'()*+,-./", buffer, 17);
    TEST_ASSERT_EQUAL_INT(7, sprintf(buffer, "%s", ":;<=>?@"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY(":;<=>?@", buffer, 8);

    TEST_ASSERT_EQUAL_INT(10, sprintf(buffer, "%s", "0123456789"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0123456789", buffer, 11);

    TEST_ASSERT_EQUAL_INT(26, sprintf(buffer, "%s", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ABCDEFGHIJKLMNOPQRSTUVWXYZ", buffer, 27);
    TEST_ASSERT_EQUAL_INT(26, sprintf(buffer, "%s", "abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("abcdefghijklmnopqrstuvwxyz", buffer, 27);

    TEST_ASSERT_EQUAL_INT(9, sprintf(buffer, "%s", "[]^_`{|}~"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("[]^_`{|}~", buffer, 10);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_sprintf_signed_integers);
    RUN_TEST(test_sprintf_unsigned_integers);
    RUN_TEST(test_sprintf_hexadecimals);
    RUN_TEST(test_sprintf_character);
    RUN_TEST(test_sprintf_string_ascii_table);
    return UNITY_END();
}
