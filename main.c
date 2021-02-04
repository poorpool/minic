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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input-file output-file\n", argv[0]);
        return 0;
    }
    //fp = fopen("../files/input.c", "r");
    //outfp = fopen("../files/output.c", "w");
    fp = fopen(argv[1], "r");
    outfp = fopen(argv[2], "w");
    if (fp == NULL) {
        printf("Read file failed!\n");
        return 0;
    }
    if (outfp == NULL) {
        printf("Output file failed!\n");
        return 0;
    }
    format(fp, outfp);
    fclose(fp);
    fclose(outfp);
    return 0;
}
