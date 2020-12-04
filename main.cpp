#include <iostream>
#include <cassert>
#include "encstrset.h"

/*
int main() {


    unsigned long set1, set2;


    set1 = jnp1::encstrset_new();
    jnp1::encstrset_insert(set1, "foo", "123");
    assert(jnp1::encstrset_test(set1, "foo", "123"));
    assert(!jnp1::encstrset_test(set1, "bar", "5X"));
    jnp1::encstrset_insert(set1, "bar", "3x");
    assert(jnp1::encstrset_test(set1, "bar", "3x"));
    assert(jnp1::encstrset_size(set1) == 2);
    jnp1::encstrset_insert(set1, "bar", "3x3");
    assert(jnp1::encstrset_size(set1) == 2);
    set2 = jnp1::encstrset_new();
    jnp1::encstrset_insert(set2, "foo", "123zl");
    assert(jnp1::encstrset_test(set2, "foo", "123"));
    jnp1::encstrset_copy(set1,set2);
    assert(jnp1::encstrset_test(set2, "bar", "3x"));
    assert(jnp1::encstrset_size(set2) == 2);
    jnp1::encstrset_remove(set1, "foo", "123;");
    jnp1::encstrset_remove(set1, "foo", "123;");
    assert(jnp1::encstrset_test(set1, "bar", "3x3"));
    assert(jnp1::encstrset_size(set1) == 1);
    jnp1::encstrset_clear(set2);
    assert(jnp1::encstrset_size(set2) == 0);
    jnp1::encstrset_copy(set1, set2);
    jnp1::encstrset_delete(set1);
    jnp1::encstrset_copy(set2, set1);
    jnp1::encstrset_copy(set1, set2);
    jnp1::encstrset_insert(set1, "foo", NULL);
    assert(jnp1::encstrset_size(set1) == 0);
    assert(!jnp1::encstrset_test(set1, "foo", NULL));
    jnp1::encstrset_remove(set1,"foo", NULL);
    jnp1::encstrset_clear(set1);
    return 0;
}
*/

namespace {
    unsigned long test() {
        unsigned long id = ::jnp1::encstrset_new();
        ::jnp1::encstrset_insert(id, "testowy", "1538221");
        return id;
    }

    unsigned long id = test();
}

int main() {
    assert(!::jnp1::encstrset_insert(id, nullptr, nullptr));
    ::jnp1::encstrset_insert(id, nullptr, "ABC");
    assert(::jnp1::encstrset_test(id, "testowy", "1538221"));
    assert(!::jnp1::encstrset_test(id, "Testowy", "1538221"));
    assert(!::jnp1::encstrset_test(id, "testowy", ""));
    ::jnp1::encstrset_delete(id);
}
