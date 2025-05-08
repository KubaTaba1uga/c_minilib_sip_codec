#include <stdio.h>
#include <stddef.h>

struct A {
    int x;
};

struct B {
    int header;
    struct A a;
    int footer;
};

int main() {
    struct B b;
    struct A *a = &b.a;

    // Compute offset in bytes from &b to &b.a
    size_t offset = (char *)a - (char *)&b;
    printf("Offset: %zu\n", offset);

    // Initialize another struct B
    struct B c = {.a = {.x = 1}};

    // Use offset to access 'a' in 'c'
    struct A *ac = (void *)((char *)&c + offset);
    printf("ac->x = %d\n", ac->x);

    return 0;
}
