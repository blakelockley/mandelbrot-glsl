#include "loader.h"

char *load_file(char *filename) {
    long int len;
    char *buffer = NULL;

    FILE *fp;
    fp = fopen(filename, "r");

    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    buffer = (char *) malloc(len + 1);

    if (buffer) {
        fseek(fp, 0, SEEK_SET);
        fread(buffer, sizeof(char), len, fp);
        buffer[len] = '\0';
    }

    return buffer;
}