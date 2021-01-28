#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "format.h"

extern int lineNumber;

void panic(const char *info) {
    fprintf(stderr, "Line %d: %s", lineNumber, info);
    exit(0);
}

int main() {
    FILE *fp = fopen("../files/input.txt", "r");
    FILE *outfp = fopen("../files/output.txt", "w");
    format(fp, outfp);
    return 0;
}
