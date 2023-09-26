#include <unity/unity.h>

// Mocks
#define kprintf(format, ...)

#include <kernel/tar.c>

const tar_header arr[2] = {
    {
        .name  = "/bin/file",
        .magic = "ustar",
        .size  = "000000031630",
    }, {
        .name   = "/usr/bin/file",
        .magic  = "ustar",
        .size   = "777777777777",
    }
};

const tar_header *archive = (void *) &arr;

void setUp() {}
void tearDown() {}

void test_tar_get_size() {
    TEST_ASSERT_EQUAL_INT(13208, tar_get_size((tar_header *) archive));
    TEST_ASSERT_EQUAL_INT(68719476735, tar_get_size((tar_header *) archive + 1));
}

void test_tar_lookup() {
    TEST_IGNORE_MESSAGE("WARNING: No tar_lookup tests");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_tar_get_size);
    RUN_TEST(test_tar_lookup);
    return UNITY_END();
}
