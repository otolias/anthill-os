#include <unity/unity.h>

#include <kernel/string.h>

void setUp() {}
void tearDown() {}

void test_memcpy(void) {
    const char src[] = "\xff\xff\xff\xff\xff";
    char dest[] = "\x00\x00\x00\x00\x00";

    char *result = memcpy(&dest, &src, 5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("\xff\xff\xff\xff\xff", result, 5);
    TEST_ASSERT_NOT_NULL(result);
}

void test_memset(void) {
    char s1[] = "12345";
    memset(s1, '0', 3);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("00045", s1, 6);

    char s2[] = "abcd";
    memset(s2, 'w', 4);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("wwww", s2, 5);
}

void test_strncmp() {
    TEST_ASSERT_EQUAL_INT(0, strncmp("", "", 1));
    TEST_ASSERT_EQUAL_INT(0, strncmp("Entomology", "Entomology", 10));
    TEST_ASSERT_EQUAL_INT(0, strncmp("Ent", "Entomology", 3));
    TEST_ASSERT_LESS_THAN_INT(0, strncmp("Entomo", "Entomology", 10));
    TEST_ASSERT_GREATER_THAN_INT(0, strncmp("Entomology", "En", 10));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_memcpy);
    RUN_TEST(test_memset);
    RUN_TEST(test_strncmp);
    return UNITY_END();
}
