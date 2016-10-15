#include "node.h"

// Within right inclusive.
// True if id in (a, b]
int within_interval_ri(node_id_t id, node_id_t a, node_id_t b)
{
    if(id > a && (id <= b || b < a))
        return 1;
    if(b < a && id <= b)
        return 1;
    return 0;
}

// True if id in [a, b)
int within_interval_li(node_id_t id, node_id_t a, node_id_t b)
{
    if(id >= a && (id < b || b < a))
        return 1;
    if(b < a && id < b)
        return 1;
    return 0;
}

// True if id in (a, b)
int within_interval_ex(node_id_t id, node_id_t a, node_id_t b)
{
    if(id > a && (id < b || b < a))
        return 1;
    if(b < a && id < b)
        return 1;
    return 0;
}

// True if id in [a, b]
int within_interval_in(node_id_t id, node_id_t a, node_id_t b)
{
    if(id >= a && (id <= b || b < a))
        return 1;
    if(b < a && id <= b)
        return 1;
    return 0;
}

