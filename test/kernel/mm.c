#include <unity/unity.h>

#include <kernel/mm.h>

/* Redefine constants to create a small memory map*/
#undef  LOW_MEMORY
#define LOW_MEMORY   8
#undef  PAGE_SIZE
#define PAGE_SIZE    8
#undef  PAGING_PAGES
#define PAGING_PAGES 5

#include <kernel/mm.c>

void setUp() {
    for (int i = 0; i < PAGING_PAGES; i++)
        memory_map[i] = PAGE_FREE;
}
void tearDown() {}

void test_get_free_pages() {
    void *page;

    page = get_free_pages(PAGING_PAGES * PAGE_SIZE);
    TEST_ASSERT_EQUAL_INT64(0, page);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("\x00\x00\x00\x00\x00", memory_map, 6);

    page = get_free_pages(5);
    TEST_ASSERT_EQUAL_INT64(8, page);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("\x02\x00\x00\x00\x00", memory_map, 6);

    memory_map[0] = memory_map[2] = PAGE_END;
    page = get_free_pages(9);
    TEST_ASSERT_EQUAL_INT64(32, page);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("\x02\x00\x02\x01\x02", memory_map, 6);
}

void test_free_pages() {
    memory_map[0] = memory_map[4] = PAGE_END;
    memory_map[2] = memory_map[3] = PAGE_CONS;

    free_pages((void *) PAGE_SIZE);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("\x00\x00\x01\x01\x02", memory_map, 6);

    free_pages((void *) (3 * PAGE_SIZE));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("\x00\x00\x00\x00\x00", memory_map, 6);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_get_free_pages);
    RUN_TEST(test_free_pages);
    return UNITY_END();
}
