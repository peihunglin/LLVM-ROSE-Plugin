int foo (int __attribute__((address_space(0)))* a,
         int __attribute__((address_space(1)))* b) {
    *a = 42;
    *b = 20;
    return *a;
}
