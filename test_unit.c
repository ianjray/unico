#include "unit.h"

#include <assert.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/// Fuzzy compare.
static bool fcmp(double x, double y)
{
    return fabs(x - y) < 0.00001;
}

static void test_symbol_of_unit(void)
{
    // symbol_of_unit.
    assert(NULL == symbol_of_unit(PresentationUnitNone));
    assert(NULL == symbol_of_unit(PresentationUnitUnknown));

#define u(symbol, name, base, scale) \
    assert(!wcscmp(symbol_of_unit(name), symbol));

#include "unit.hi"
}

/// Test that @c unit_to_base produces expected output.
static void wrap_unit_to_base(double quantity, enum unit unit, double expected_quantity, enum base expected_base_unit)
{
    enum base base;
    double v = unit_to_base(quantity, unit, &base);
    assert(base == expected_base_unit);
    assert(fcmp(v, expected_quantity));
}

static void test_unit_to_base(void)
{
    wrap_unit_to_base(42, PresentationUnitNone, 42, BaseUnitNone);
    wrap_unit_to_base(42, PresentationUnitUnknown, 42, BaseUnitNone);

#define u(symbol, name, base, scale) \
    wrap_unit_to_base(1, name, scale, base);

#define c(symbol, name, base, tobase, frombase) \
    { float  X=1; wrap_unit_to_base(1, name, tobase, base); }

#include "unit.hi"
}

static void wrap_base_to_unit(double quantity, enum base base, enum unit unit, double expected)
{
    double actual;
    assert(!base_to_unit(quantity, base, unit, &actual));
    assert(fcmp(expected, actual));
}

static void test_base_unit_to_unit(void)
{
    double actual;
    assert(-1 == base_to_unit(42, BaseUnitNone, PresentationUnitNone, &actual));
    assert(-1 == base_to_unit(42, BaseUnitNone, PresentationUnitUnknown, &actual));
    assert(-1 == base_to_unit(42, BaseUnitMetre, PresentationUnitKilogram, &actual));

#define u(symbol, name, base, scale) \
    wrap_base_to_unit(scale, base, name, 1);

#define c(symbol, name, base, tobase, frombase) \
    { float  X=1; wrap_base_to_unit(tobase, base, name, 1); }

#include "unit.hi"
}

/// Test that @c actual matches @c expected.
static void expect(char *actual, const char *expected)
{
    if (expected == NULL) {
        assert(!actual);
    } else {
        assert(actual && expected);
        assert(!strcmp(actual, expected));
        free(actual);
    }
}

static void test_base_render(void)
{
    expect(base_render(1, BaseUnitMetre, PresentationUnitNone),          NULL);
    expect(base_render(1, BaseUnitMetre, PresentationUnitUnknown),       NULL);

    expect(base_render(1, BaseUnitMetre, PresentationUnitMillimetre),    "1000 mm");
    expect(base_render(1, BaseUnitMetre, PresentationUnitCentimetre),     "100 cm");
    expect(base_render(1, BaseUnitMetre, PresentationUnitMetre),            "1 m");
    expect(base_render(1, BaseUnitMetre, PresentationUnitKilometre),        "0.001 km");
    expect(base_render(1000, BaseUnitMetre, PresentationUnitKilometre),     "1 km");
    expect(base_render(1000, BaseUnitMetre, PresentationUnitMile),          "0.621371 mi");
    expect(base_render(1, BaseUnitMetre, PresentationUnitYard),             "1.09361 yd");
    expect(base_render(1, BaseUnitMetre, PresentationUnitFeetAndInches),    "3 ' 3.37008 \"");
    expect(base_render(1, BaseUnitMetre, PresentationUnitFeet),             "3.28084 ft");
    expect(base_render(1, BaseUnitMetre, PresentationUnitInch),            "39.3701 in");

    expect(base_render(1, BaseUnitSquareMetre, PresentationUnitSquareMetre),  "1 m^2");
    expect(base_render(10000, BaseUnitSquareMetre, PresentationUnitHectare),  "1 ha");
    expect(base_render(4046.8564224, BaseUnitSquareMetre, PresentationUnitAcre),  "1 acre");
    expect(base_render(0.09290304, BaseUnitSquareMetre, PresentationUnitSquareFoot),  "1 ft^2");

    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitCubicMetre),  "1 m^3");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitCubicCentimetre),  "1e+06 cm^3");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitMillilitre), "1e+06 mL");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitCentilitre),  "100000 cL");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitDecilitre),    "10000 dL");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitLitre),         "1000 L");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitMillilitreAlt), "1e+06 ml");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitCentilitreAlt),  "100000 cl");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitDecilitreAlt),    "10000 dl");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitLitreAlt),         "1000 l");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitPint),       "1759.75 pt");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitPintUS),     "2113.38 US pt");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitCubicFoot),    "35.3147 ft^3");
    expect(base_render(1, BaseUnitCubicMetre, PresentationUnitCubicInch), "61023.7 in^3");

    expect(base_render(2, BaseUnitKilogram, PresentationUnitMilligram), "2e+06 mg");
    expect(base_render(2, BaseUnitKilogram, PresentationUnitGram),       "2000 g");
    expect(base_render(2, BaseUnitKilogram, PresentationUnitKilogram),      "2 kg");
    expect(base_render(2, BaseUnitKilogram, PresentationUnitMegagram),      "0.002 Mg");
    expect(base_render(2, BaseUnitKilogram, PresentationUnitTonne),         "0.002 t");
    expect(base_render(2000, BaseUnitKilogram, PresentationUnitShortTon),   "2.20462 tn");
    expect(base_render(2000, BaseUnitKilogram, PresentationUnitLongTon),    "1.96841 long ton");
    expect(base_render(2, BaseUnitKilogram, PresentationUnitPound),         "4.40925 lb");
    expect(base_render(2, BaseUnitKilogram, PresentationUnitOunce),        "70.5479 oz");

    expect(base_render(300, BaseUnitKelvin, PresentationUnitKelvin),            "300 K");
    expect(base_render(300, BaseUnitKelvin, PresentationUnitDegreesCelsius),     "26.85 °C");
    expect(base_render(300, BaseUnitKelvin, PresentationUnitDegreesFahrenheit),  "80.33 °F");

    expect(base_render(1000, DerivedUnitPressurePascal, PresentationUnitPascal),            "1000 Pa");
    expect(base_render(1000, DerivedUnitPressurePascal, PresentationUnitHectoPascal),         "10 hPa");
    expect(base_render(1000, DerivedUnitPressurePascal, PresentationUnitKiloPascal),           "1 kPa");
    expect(base_render(100000, DerivedUnitPressurePascal, PresentationUnitMillibar),        "1000 mbar");
    expect(base_render(100000, DerivedUnitPressurePascal, PresentationUnitBar),                "1 bar");
    expect(base_render(6894.757, DerivedUnitPressurePascal, PresentationUnitPoundPerSquareInch), "1 psi");
    expect(base_render(1000, DerivedUnitPressurePascal, PresentationUnitMillimetreMercury),    "7.50062 mmHg");
    expect(base_render(1000, DerivedUnitPressurePascal, PresentationUnitInchesMercury),        "0.2953 inHg");

    expect(base_render(3.1415926536, DerivedUnitAngleRadian, PresentationUnitRadian), "3.14159 rad");
    expect(base_render(3.1415926536, DerivedUnitAngleRadian, PresentationUnitDegree), "180 °");
}

int main(void)
{
    // This file is encoded as UTF-8.
    setlocale(LC_ALL, "en_US.UTF-8");

    test_symbol_of_unit();
    test_unit_to_base();
    test_base_unit_to_unit();
    test_base_render();
}
