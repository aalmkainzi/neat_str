
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "../cgs.h"

// Test counter
extern int test_count;
extern int passed_count;

#define TEST(name) \
do { \
    printf("Running test: %s\n", name); \
} while(0)

#define ASSERT_EQ(a, b) \
do { \
    test_count++; \
    if ((a) == (b)) { \
        passed_count++; \
    } else { \
        printf("  FAILED: Expected %d, got %d at line %d\n", (int)(b), (int)(a), __LINE__); \
    } \
} while(0)

#define ASSERT_TRUE(cond) \
do { \
    test_count++; \
    if (cond) { \
        passed_count++; \
    } else { \
        printf("  FAILED: Condition false at line %d\n", __LINE__); \
    } \
} while(0)

#define ASSERT_FALSE(cond) \
do { \
    test_count++; \
    if (!(cond)) { \
        passed_count++; \
    } else { \
        printf("  FAILED: Condition true at line %d\n", __LINE__); \
    } \
} while(0)

#define ASSERT_STR_EQ(a, b) \
ASSERT_TRUE(cgs_equal(a, b))

#define ASSERT_NULL(ptr) \
do { \
    test_count++; \
    if ((ptr) == NULL) { \
        passed_count++; \
    } else { \
        printf("  FAILED: Expected NULL at line %d\n", __LINE__); \
    } \
} while(0)

#define ASSERT_NOT_NULL(ptr) \
do { \
    test_count++; \
    if ((ptr) != NULL) { \
        passed_count++; \
    } else { \
        printf("  FAILED: Expected non-NULL at line %d\n", __LINE__); \
    } \
} while(0)
