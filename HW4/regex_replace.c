#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <regex> <text> <replacement>\n", argv[0]);
        return 1;
    }

    const char *pattern     = argv[1];
    const char *text        = argv[2];
    const char *replacement = argv[3];

    regex_t re;
    int ret = regcomp(&re, pattern, REG_EXTENDED);
    if (ret != 0) {
        char errbuf[256];
        regerror(ret, &re, errbuf, sizeof(errbuf));
        fprintf(stderr, "Failed to compile regex: %s\n", errbuf);
        return 1;
    }

    size_t buf_size = strlen(text) * 2 + strlen(replacement) + 1;
    char *output = malloc(buf_size);
    if (!output) {
        fprintf(stderr, "Memory allocation failed\n");
        regfree(&re);
        return 1;
    }
    output[0] = '\0';
    size_t out_len = 0;

    const char *cursor = text;
    regmatch_t match;

    while (regexec(&re, cursor, 1, &match, 0) == 0) {
        size_t prefix_len     = (size_t)match.rm_so;
        size_t replacement_len = strlen(replacement);
        size_t needed          = out_len + prefix_len + replacement_len + 1;

        if (needed > buf_size) {
            buf_size = needed * 2;
            char *tmp = realloc(output, buf_size);
            if (!tmp) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(output);
                regfree(&re);
                return 1;
            }
            output = tmp;
        }

        memcpy(output + out_len, cursor, prefix_len);
        out_len += prefix_len;

        memcpy(output + out_len, replacement, replacement_len);
        out_len += replacement_len;

        output[out_len] = '\0';

        cursor += match.rm_eo;

        if (match.rm_eo == match.rm_so) {
            if (*cursor == '\0') break;
            if (out_len + 1 >= buf_size) {
                buf_size = (out_len + 1) * 2;
                char *tmp = realloc(output, buf_size);
                if (!tmp) {
                    fprintf(stderr, "Memory reallocation failed\n");
                    free(output);
                    regfree(&re);
                    return 1;
                }
                output = tmp;
            }
            output[out_len++] = *cursor++;
            output[out_len]   = '\0';
        }
    }

    size_t tail_len = strlen(cursor);
    if (out_len + tail_len + 1 > buf_size) {
        buf_size = out_len + tail_len + 1;
        char *tmp = realloc(output, buf_size);
        if (!tmp) {
            fprintf(stderr, "Memory reallocation failed\n");
            free(output);
            regfree(&re);
            return 1;
        }
        output = tmp;
    }
    memcpy(output + out_len, cursor, tail_len);
    out_len += tail_len;
    output[out_len] = '\0';

    printf("%s\n", output);

    free(output);
    regfree(&re);
    return 0;
}