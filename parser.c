#include "parser.h"
#include "label.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

struct compound {
    enum unit first;
    enum unit second;
};

/// Compound units.
static const struct compound compounds[] = {
    { PresentationUnitFeet,  PresentationUnitInch  },
    { PresentationUnitPound, PresentationUnitOunce }
};

/// @return True if unit is the first part of possible compound unit.
static bool is_compound_first(enum unit unit)
{
    for (size_t i = 0; i < sizeof(compounds)/sizeof(*compounds); i++) {
        if (compounds[i].first == unit) {
            return true;
        }
    }
    return false;
}

/// @return True if unit forms the second part of a compound unit.
static bool is_compound_second(enum unit first, enum unit second)
{
    for (size_t i = 0; i < sizeof(compounds)/sizeof(*compounds); i++) {
        if (compounds[i].first == first && compounds[i].second == second) {
            return true;
        }
    }
    return false;
}

enum state {
    S_QUANTITY,
    S_FROM,
    S_SUB_QUANTITY,
    S_SUB_FROM,
    S_TO
};

struct parser {
    /// Parser state.
    enum state state;
    /// Scratch space for number parsing.
    double scratch;
    /// Results.
    struct parser_data data;
};

/// Reset parser context.
static void reset(struct parser *pa)
{
    if (pa) {
        memset(pa, 0, sizeof(*pa));
    }
}

struct parser *parser_new(void)
{
    return calloc(1, sizeof(struct parser));
}

void parser_delete(struct parser *pa)
{
    free(pa);
}

static enum parser_ret add(struct parser *pa, wchar_t *arg, wchar_t **out)
{
    wchar_t *p = NULL;

    *out = NULL;

    switch (pa->state) {
        default:
        case S_QUANTITY:
            pa->scratch = wcstod(arg, &p);
            if (p == arg) {
                *out = arg;
                return PARSE_INVALID_NUMBER;
            }

            pa->state++;
            break;

        case S_FROM:
            pa->data.from = label_lookup(arg, &p);

            if (!symbol_of_unit(pa->data.from)) {
                *out = arg;
                return PARSE_UNKNOWN_UNIT;
            }

            if (is_compound_first(pa->data.from)) {
                // Possible compound-unit.
                pa->state = S_SUB_QUANTITY;
            } else {
                pa->state = S_TO;
            }

            pa->data.quantity = unit_to_base(pa->scratch, pa->data.from, &pa->data.base);
            break;

        case S_SUB_QUANTITY:
            pa->scratch = wcstod(arg, &p);
            if (p == arg) {
                // Not a number, no compound-unit.
                pa->state = S_TO;
            } else {
                pa->state++;
            }
            break;

        case S_SUB_FROM:
        {
            enum unit second = label_lookup(arg, &p);
            if ((p && *p) || !symbol_of_unit(second)) {
                *out = arg;
                return PARSE_UNKNOWN_UNIT;
            }

            if (!is_compound_second(pa->data.from, second)) {
                *out = arg;
                return PARSE_INVALID_COMPOUND;
            }

            pa->data.quantity += unit_to_base(pa->scratch, second, &pa->data.base);
            pa->state++;
            return PARSE_AGAIN;
        }

        case S_TO:
            pa->data.to = label_lookup(arg, &p);
            if ((p && *p) || !symbol_of_unit(pa->data.to)) {
                *out = arg;
                return PARSE_UNKNOWN_UNIT;
            }
            return PARSE_COMPLETE;
    }

    if (!p || !*p) {
        return PARSE_AGAIN;
    }

    return add(pa, p, out);
}

enum parser_ret parser_add(struct parser *pa, wchar_t *arg, wchar_t **term, struct parser_data *data)
{
    enum parser_ret ret;

    if (!pa || !arg || !term || !data) {
        ret = PARSE_INVALID_ARGUMENT;

    } else {
        ret = add(pa, arg, term);
        if (ret == PARSE_COMPLETE) {
            *data = pa->data;
        }
    }

    if (ret != PARSE_AGAIN) {
        reset(pa);
    }

    return ret;
}
