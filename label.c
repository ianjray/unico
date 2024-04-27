#include "label.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

struct lookup {
    /// Labels may use American or British English.
    /// Labels may use plurals.
    /// Labels may refer to SI or non-SI units in common use.
    const wchar_t *label;
    /// Unit.
    enum unit unit;
};

/// Lookup table used for parsing user description of a unit.
static struct lookup labels[] = {
    { L"", PresentationUnitNone },

    // Length.
    { L"mm", PresentationUnitMillimetre },
    { L"millimetre", PresentationUnitMillimetre },
    { L"millimetres", PresentationUnitMillimetre },
    { L"millimeter", PresentationUnitMillimetre },
    { L"millimeters", PresentationUnitMillimetre },
    { L"cm", PresentationUnitCentimetre },
    { L"centimetre", PresentationUnitCentimetre },
    { L"centimetres", PresentationUnitCentimetre },
    { L"centimeter", PresentationUnitCentimetre },
    { L"centimeters", PresentationUnitCentimetre },
    { L"m", PresentationUnitMetre },
    { L"metre", PresentationUnitMetre },
    { L"metres", PresentationUnitMetre },
    { L"meter", PresentationUnitMetre },
    { L"meters", PresentationUnitMetre },
    { L"km", PresentationUnitKilometre },
    { L"kilometre", PresentationUnitKilometre },
    { L"kilometres", PresentationUnitKilometre },
    { L"kilometer", PresentationUnitKilometre },
    { L"kilometers", PresentationUnitKilometre },
    // Non-SI.
    { L"mi", PresentationUnitMile },
    { L"mile", PresentationUnitMile },
    { L"miles", PresentationUnitMile },
    { L"yd", PresentationUnitYard },
    { L"yds", PresentationUnitYard },
    { L"yard", PresentationUnitYard },
    { L"yards", PresentationUnitYard },
    { L"'", PresentationUnitFeet },
    { L"ft", PresentationUnitFeet },
    { L"feet", PresentationUnitFeet },
    { L"foot", PresentationUnitFeet },
    { L"'\"", PresentationUnitFeetAndInches },
    { L"\"", PresentationUnitInch },
    { L"in", PresentationUnitInch },
    { L"inch", PresentationUnitInch },
    { L"inches", PresentationUnitInch },

    // Area.
    { L"m^2", PresentationUnitSquareMetre },
    { L"ha", PresentationUnitHectare },
    { L"hectare", PresentationUnitHectare },
    // Non-SI.
    { L"acre", PresentationUnitAcre },
    { L"acres", PresentationUnitAcre },
    { L"ft^2", PresentationUnitSquareFoot },
    { L"sq ft", PresentationUnitSquareFoot },
    { L"square foot", PresentationUnitSquareFoot },
    { L"square feet", PresentationUnitSquareFoot },

    // Volume.
    { L"m^3", PresentationUnitCubicMetre },
    { L"cm^3", PresentationUnitCubicCentimetre },
    { L"L", PresentationUnitLitre },
    { L"dL", PresentationUnitDecilitre },
    { L"cL", PresentationUnitCentilitre },
    { L"mL", PresentationUnitMillilitre },
    { L"l", PresentationUnitLitreAlt },
    { L"dl", PresentationUnitDecilitreAlt },
    { L"cl", PresentationUnitCentilitreAlt },
    { L"ml", PresentationUnitMillilitreAlt },
    // Non-SI.
    { L"cc", PresentationUnitCubicCentimetre },
    { L"pt", PresentationUnitPint },
    { L"pint", PresentationUnitPint },
    { L"US pt", PresentationUnitPintUS },
    { L"US pint", PresentationUnitPintUS },
    { L"ft^3", PresentationUnitCubicFoot },
    { L"cu ft", PresentationUnitCubicFoot },
    { L"cubic foot", PresentationUnitCubicFoot },
    { L"cubic feet", PresentationUnitCubicFoot },
    { L"in^3", PresentationUnitCubicInch },
    { L"cu in", PresentationUnitCubicInch },
    { L"cubic inch", PresentationUnitCubicInch },

    // Mass.
    { L"mg", PresentationUnitMilligram },
    { L"milligram", PresentationUnitMilligram },
    { L"milligrams", PresentationUnitMilligram },
    { L"g", PresentationUnitGram },
    { L"gram", PresentationUnitGram },
    { L"grams", PresentationUnitGram },
    { L"kg", PresentationUnitKilogram },
    { L"kilogram", PresentationUnitKilogram },
    { L"kilograms", PresentationUnitKilogram },
    { L"Mg", PresentationUnitMegagram },
    { L"megagram", PresentationUnitMegagram },
    { L"megagrams", PresentationUnitMegagram },
    // Non-SI.
    { L"t", PresentationUnitTonne },
    { L"tonne", PresentationUnitTonne },
    { L"tonnes", PresentationUnitTonne },
    { L"tn", PresentationUnitShortTon },
    { L"short ton", PresentationUnitShortTon },
    { L"short tons", PresentationUnitShortTon },
    { L"US ton", PresentationUnitShortTon },
    { L"US tons", PresentationUnitShortTon },
    { L"long ton", PresentationUnitLongTon },
    { L"long tons", PresentationUnitLongTon },
    { L"imperial ton", PresentationUnitLongTon },
    { L"imperial tons", PresentationUnitLongTon },
    { L"lb", PresentationUnitPound },
    { L"lbs", PresentationUnitPound },
    { L"pound", PresentationUnitPound },
    { L"pounds", PresentationUnitPound },
    { L"oz", PresentationUnitOunce },
    { L"ounce", PresentationUnitOunce },
    { L"ounces", PresentationUnitOunce },

    // Thermodynamic temperature.
    { L"K", PresentationUnitKelvin },
    { L"kelvin", PresentationUnitKelvin },
    { L"°C", PresentationUnitDegreesCelsius },
    { L"'C", PresentationUnitDegreesCelsius },
    { L"degree Celsius", PresentationUnitDegreesCelsius },
    { L"degrees Celsius", PresentationUnitDegreesCelsius },
    // Non-SI.
    { L"°F", PresentationUnitDegreesFahrenheit },
    { L"'F", PresentationUnitDegreesFahrenheit },
    { L"degree Fahrenheit", PresentationUnitDegreesFahrenheit },
    { L"degrees Fahrenheit", PresentationUnitDegreesFahrenheit },

    // Pressure.
    { L"Pa", PresentationUnitPascal },
    { L"pascal", PresentationUnitPascal },
    { L"hPa", PresentationUnitHectoPascal },
    { L"hectopascal", PresentationUnitHectoPascal },
    { L"hectopascals", PresentationUnitHectoPascal },
    { L"kPa", PresentationUnitKiloPascal },
    { L"kilopascal", PresentationUnitKiloPascal },
    { L"kilopascals", PresentationUnitKiloPascal },
    // Not SI, but used especially in meterology.
    { L"mbar", PresentationUnitMillibar },
    { L"millibar", PresentationUnitMillibar },
    { L"millibars", PresentationUnitMillibar },
    { L"bar", PresentationUnitBar },
    // Not SI, but commonly used.
    { L"psi", PresentationUnitPoundPerSquareInch },
    // Non-SI.
    { L"mmHg", PresentationUnitMillimetreMercury },
    { L"mm Hg", PresentationUnitMillimetreMercury },
    { L"inHg", PresentationUnitInchesMercury },
    { L"\"Hg", PresentationUnitInchesMercury },

    // Plane angle.
    { L"rad", PresentationUnitRadian },
    { L"radian", PresentationUnitRadian },
    { L"radians", PresentationUnitRadian },
    { L"°", PresentationUnitDegree },
    { L"degree", PresentationUnitDegree },
    { L"degrees", PresentationUnitDegree },
};

/// @return True if end of word.
static bool is_eow(const wchar_t *s)
{
    return !s || !*s || iswspace(*s) || iswdigit(*s);
}

/// @return True if string @c s matches, or begins with, string @c w.
static bool accept_word(wchar_t *s, const wchar_t *w, wchar_t **p)
{
    if (!wcsncmp(s, w, wcslen(w))) {
        s += wcslen(w);

        if (is_eow(s)) {
            if (p) {
                *p = s;
            }
            return true;
        }
    }

    return false;
}

enum unit label_lookup(wchar_t *s, wchar_t **p)
{
    size_t candidate = 0;
    size_t candidates = 0;
    size_t length = 0;

    *p = s;

    if (!*s) {
        return PresentationUnitNone;
    }

    for (size_t i = 0; i < sizeof(labels) / sizeof(*labels); ++i) {
        if (accept_word(s, labels[i].label, NULL)) {
            // Find longest match.
            if (wcslen(labels[i].label) > length) {
                candidate = i;
                candidates++;;
                length = wcslen(labels[i].label);
            }
        }
    }

    if (candidates) {
        accept_word(s, labels[candidate].label, p);
        return labels[candidate].unit;
    }

    return PresentationUnitUnknown;
}

void label_synonyms(enum unit unit)
{
    bool output = false;

    for (size_t i = 0; i < sizeof(labels) / sizeof(*labels); ++i) {
        if (unit == labels[i].unit) {
            if (output) {
                printf(", ");
            }
            printf("%ls", labels[i].label);
            output = true;
        }
    }

    if (output) {
        printf("\n");
    }
}
