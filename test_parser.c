#include "parser.h"

#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>

/// Fuzzy compare.
static bool fcmp(double x, double y)
{
    return fabs(x - y) < 0.000001;
}

static struct parser *parser_;
static wchar_t *term_;
static struct parser_data data_;

static void add(enum parser_ret ret, wchar_t *s)
{
    assert(ret == parser_add(parser_, s, &term_, &data_));
}

static void fail(void)
{
    add(PARSE_UNKNOWN_UNIT, L"@");
}

/// Verify expected value when converting quantity @c in with unit @c from to unit @c to.
static void pass(double in, enum unit from, enum unit to, double expected)
{
    double quantity;
    assert(!base_to_unit(data_.quantity, data_.base, data_.from, &quantity));
    assert(fcmp(quantity, in));
    assert(data_.from == from);
    assert(data_.to == to);

    double actual;
    assert(!base_to_unit(data_.quantity, data_.base, data_.to, &actual));
    assert(fcmp(expected, actual));
}

int main(void)
{
    // This file is encoded as UTF-8.
    setlocale(LC_ALL, "en_US.UTF-8");

    parser_ = parser_new();

    assert(PARSE_INVALID_ARGUMENT == parser_add(NULL, L"123", &term_, &data_));
    assert(PARSE_INVALID_ARGUMENT == parser_add(parser_, NULL, &term_, &data_));
    assert(PARSE_INVALID_ARGUMENT == parser_add(parser_, L"123", NULL, &data_));
    assert(PARSE_INVALID_ARGUMENT == parser_add(parser_, L"123", &term_, NULL));

    // Bad input.
    add(PARSE_INVALID_NUMBER, L"abc");
    add(PARSE_INVALID_NUMBER, L"xyz");

    // Bad number.
    add(PARSE_INVALID_NUMBER, L"@");
    assert(!wcscmp(L"@", term_));

    add(PARSE_AGAIN, L"1");
    // Unknown unit.
    add(PARSE_UNKNOWN_UNIT, L"");
    assert(!wcscmp(L"", term_));

    add(PARSE_AGAIN, L"1");
    add(PARSE_AGAIN, L"m");
    // Incomplete input.
    parser_delete(parser_);
    parser_ = parser_new();

    add(PARSE_AGAIN, L"1ft");
    // Unknown unit.
    add(PARSE_UNKNOWN_UNIT, L"@");
    assert(!wcscmp(L"@", term_));

    add(PARSE_AGAIN, L"1ft");
    // Unknown unit.
    add(PARSE_UNKNOWN_UNIT, L"2@");
    assert(!wcscmp(L"@", term_));

    add(PARSE_AGAIN, L"1ft");
    // Incompatible unit.
    add(PARSE_INVALID_COMPOUND, L"2cm");
    assert(!wcscmp(L"cm", term_));

    add(PARSE_AGAIN, L"1lb");
    // Incompatible unit.
    add(PARSE_INVALID_COMPOUND, L"2mm");
    assert(!wcscmp(L"mm", term_));

    add(PARSE_AGAIN, L"1");
    // Missing units.
    fail();

    add(PARSE_AGAIN, L"1");
    add(PARSE_AGAIN, L"'");
    add(PARSE_AGAIN, L"2");
    // Missing units.
    fail();

    add(PARSE_AGAIN, L"1000mm");
    add(PARSE_COMPLETE, L"m");
    pass(1000, PresentationUnitMillimetre, PresentationUnitMetre, 1);

    add(PARSE_AGAIN, L"1.093613");
    add(PARSE_AGAIN, L"yd");
    add(PARSE_COMPLETE, L"m");
    pass(1.093613, PresentationUnitYard, PresentationUnitMetre, 1);

    add(PARSE_AGAIN, L"5ft8in");
    add(PARSE_COMPLETE, L"m");
    pass(5.666666, PresentationUnitFeet, PresentationUnitMetre, 1.7272);

    add(PARSE_AGAIN, L"5ft");
    add(PARSE_AGAIN, L"8in");
    add(PARSE_COMPLETE, L"m");
    pass(5.666666, PresentationUnitFeet, PresentationUnitMetre, 1.7272);

    add(PARSE_AGAIN, L"5");
    add(PARSE_AGAIN, L"ft");
    add(PARSE_AGAIN, L"8");
    add(PARSE_AGAIN, L"in");
    add(PARSE_COMPLETE, L"m");
    pass(5.666666, PresentationUnitFeet, PresentationUnitMetre, 1.7272);

    add(PARSE_AGAIN, L"5'8\"");
    add(PARSE_COMPLETE, L"m");
    pass(5.666666, PresentationUnitFeet, PresentationUnitMetre, 1.7272);

    add(PARSE_AGAIN, L"5'");
    add(PARSE_AGAIN, L"8\"");
    add(PARSE_COMPLETE, L"m");
    pass(5.666666, PresentationUnitFeet, PresentationUnitMetre, 1.7272);

    add(PARSE_AGAIN, L"5.666666'");
    add(PARSE_COMPLETE, L"m");
    pass(5.666666, PresentationUnitFeet, PresentationUnitMetre, 1.7272);

    add(PARSE_AGAIN, L"-5kg");
    add(PARSE_COMPLETE, L"g");
    pass(-5, PresentationUnitKilogram, PresentationUnitGram, -5000);

    add(PARSE_AGAIN, L"7lb14oz");
    add(PARSE_COMPLETE, L"kg");
    pass(7.875, PresentationUnitPound, PresentationUnitKilogram, 3.57204);

    add(PARSE_AGAIN, L"7lb");
    add(PARSE_AGAIN, L"14oz");
    add(PARSE_COMPLETE, L"kg");
    pass(7.875, PresentationUnitPound, PresentationUnitKilogram, 3.57204);

    add(PARSE_AGAIN, L"7");
    add(PARSE_AGAIN, L"lb");
    add(PARSE_AGAIN, L"14");
    add(PARSE_AGAIN, L"oz");
    add(PARSE_COMPLETE, L"kg");
    pass(7.875, PresentationUnitPound, PresentationUnitKilogram, 3.57204);

    add(PARSE_AGAIN, L"7.875lbs");
    add(PARSE_COMPLETE, L"kg");
    pass(7.875, PresentationUnitPound, PresentationUnitKilogram, 3.57204);

    add(PARSE_AGAIN, L"26.85°C");
    add(PARSE_COMPLETE, L"K");
    pass(26.85, PresentationUnitDegreesCelsius, PresentationUnitKelvin, 300);

    add(PARSE_AGAIN, L"80.33°F");
    add(PARSE_COMPLETE, L"K");
    pass(80.33, PresentationUnitDegreesFahrenheit, PresentationUnitKelvin, 300);

    add(PARSE_AGAIN, L"7.5006157585mmHg");
    add(PARSE_COMPLETE, L"Pa");
    pass(7.5006157585, PresentationUnitMillimetreMercury, PresentationUnitPascal, 1000);
}
