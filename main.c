#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "format.h"

extern int lineNumber;

FILE *fp;
FILE *outfp;

void panic(const char *info) {
    fprintf(stderr, "Line %d: %s", lineNumber, info);
    fclose(fp);
    fclose(outfp);
    exit(0);
}

int main() {
    fp = fopen("../files/input.txt", "r");
    outfp = fopen("../files/output.txt", "w");
    format(fp, outfp);
    fclose(fp);
    fclose(outfp);
    return 0;
}
