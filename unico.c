#include "label.h"
#include "parser.h"
#include "unit.h"

#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @return wide string of @c arg.
static wchar_t *str_to_wcs(const char *arg)
{
    wchar_t *wcs = NULL;

    if (arg) {
        size_t len = strlen(arg) + 1 /*NUL*/;

        wcs = (wchar_t *)malloc(sizeof(wchar_t) * len);
        if (wcs) {
            size_t ret = mbstowcs(wcs, arg, len);
            if (ret == (size_t)-1) {
                free(wcs);
            }
        }
    } else {
        errno = EFAULT;
    }

    return wcs;
}

__attribute__((noreturn))
static void synopsis(void)
{
    fprintf(stderr, "usage: unico [-hl] [QUANTITY FROM TO]...\n");
    exit(EXIT_SUCCESS);
}

__attribute__((noreturn))
static void help(void)
{
    fprintf(stderr,
        "unico [OPTIONS...] [QUANTITY FROM TO]...\n"
        "\n"
        "Convert QUANTITY in FROM unit to TO unit.\n"
        "\n"
        "Options:\n"
        "	-h, --help		Show this help and exit.\n"
        "	-l, --list		List known units and exit.\n"
        );
    exit(EXIT_SUCCESS);
}

__attribute__((noreturn))
static void list(void)
{
#define s(name) \
    printf("%ls.\n", name);

#define u(symbol, name, base, scale) \
    printf("\t"); \
    label_synonyms(name);

#include "unit.hi"
    exit(EXIT_SUCCESS);
}

/// Report failure.
/// @return False if parsing failed.
static bool report(enum parser_ret ret, const wchar_t *term)
{
    switch (ret) {
        case PARSE_AGAIN:
            return true;
        case PARSE_COMPLETE:
            return true;
        case PARSE_INVALID_COMPOUND:
            fprintf(stderr, "Incompatible unit '%ls'.\n", term);
            break;
        default:
        case PARSE_INVALID_ARGUMENT:
            fprintf(stderr, "Internal error.\n");
            break;
        case PARSE_UNKNOWN_UNIT:
            fprintf(stderr, "Unknown unit '%ls'.\n", term);
            break;
        case PARSE_INVALID_NUMBER:
            fprintf(stderr, "Bad number '%ls'.\n", term);
            break;
    }

    return false;
}

/// Process arguments.
/// @return False if processing failed.
static bool process(int argc, char **argv)
{
    struct parser *parser;
    enum parser_ret ret = PARSE_COMPLETE;

    parser = parser_new();

    while (argc-- > 0) {
        char *arg;
        wchar_t *warg;
        wchar_t *term;
        struct parser_data data;

        arg = *argv++;
        warg = str_to_wcs(arg);
        if (!warg) {
            perror(arg);
            parser_delete(parser);
            return false;
        }

        ret = parser_add(parser, warg, &term, &data);
        if (ret == PARSE_COMPLETE) {
            char *in = base_render(data.quantity, data.base, data.from);
            char *out = base_render(data.quantity, data.base, data.to);

            if (in && out) {
                printf("%s is %s\n", in, out);
            } else {
                fprintf(stderr, "Cannot convert '%ls' to '%ls'.\n", symbol_of_unit(data.from), symbol_of_unit(data.to));
            }

            free(in);
            free(out);
        }

        if (!report(ret, term)) {
            parser_delete(parser);
            return false;
        }

        free(warg);
    }

    parser_delete(parser);

    if (ret != PARSE_COMPLETE) {
        fprintf(stderr, "Incomplete input.\n");
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    struct option longopts[] = {
        { "help", no_argument, NULL, 'h' },
        { "list", no_argument, NULL, 'l' },
        { NULL, 0, NULL, 0 }
    };

    int ch;

    setlocale(LC_ALL, "");

    while ((ch = getopt_long(argc, argv, "hl", longopts, NULL)) != -1) {
        switch (ch) {
            case 'h':
                help();
            case 'l':
                list();
            default:
                synopsis();
        }
    }

    argc -= optind;
    argv += optind;

    if (argc == 0) {
        synopsis();
    }

    if (!process(argc, argv)) {
        exit(EXIT_FAILURE);
    }
}
