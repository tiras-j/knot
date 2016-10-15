#include <stdio.h>
#include <stdlib.h>

#define assert_eq(a, b) if(!((a) == (b))){ \
                            fprintf(stderr, "%s - %s:%d.\n", "Failed Assert!", __FILE__, __LINE__); \
                            exit(1); \
                        }

typedef unsigned long node_id_t;

extern int within_interval_ri(node_id_t, node_id_t, node_id_t);
extern int within_interval_li(node_id_t, node_id_t, node_id_t);
extern int within_interval_ex(node_id_t, node_id_t, node_id_t);
extern int within_interval_in(node_id_t, node_id_t, node_id_t);

int main()
{
    node_id_t a = 0, b = 100;
    node_id_t c = 99, d = 25;
    // Test right_inclusive:
    assert_eq(1, within_interval_ri(50, a, b));
    assert_eq(1, within_interval_ri(100, a, b));
    assert_eq(1, within_interval_ri(10, c, d));
    assert_eq(1, within_interval_ri(25, c, d));
    assert_eq(0, within_interval_ri(26, c, d));

    // Test left inclusive
    assert_eq(1, within_interval_li(0, a, b));
    assert_eq(1, within_interval_li(50, a, b));
    assert_eq(1, within_interval_li(99, c, d));
    assert_eq(1, within_interval_li(10, c, d));
    assert_eq(0, within_interval_li(50, c, d));

    // Test exclusive
    assert_eq(0, within_interval_ex(0, a, b));
    assert_eq(1, within_interval_ex(50, a, b));
    assert_eq(0, within_interval_ex(99, c, d));
    assert_eq(1, within_interval_ex(10, c, d));
    assert_eq(0, within_interval_ex(50, c, d));
    assert_eq(1, within_interval_ex(100, c, d));
    printf("PASSED\n");
}
