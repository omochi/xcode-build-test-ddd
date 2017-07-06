#pragma once

#include <stdio.h>
#include <bbb/bbb.h>
#include <ccc/ccc.h>

int main() {
    int x = bbb_world() + ccc_poyo();
    printf("%d\n", x);
    return 0;
}
