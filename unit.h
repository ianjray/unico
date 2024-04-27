#pragma once

#include <wchar.h>

/// Base units.
/// @see https://en.wikipedia.org/wiki/International_System_of_Units
enum base {
    BaseUnitNone,

#define b(symbol, name) name,
#include "unit.hi"
};

/// Units.
enum unit {
    PresentationUnitNone,
    PresentationUnitUnknown,

#define u(symbol, name, base, scale) name,
#include "unit.hi"
};

/// @return Symbol of unit.
const wchar_t *symbol_of_unit(enum unit unit);

/// Convert @c quantity of @c unit to the base unit.
/// @return quantity of @c base.
double unit_to_base(double quantity, enum unit unit, enum base *base);

/// Convert @c quantity of @c base to the desired @c unit.
/// @return Zero on success, negative otherwise.
/// @return -EPERM If @c base cannot be converted to @c unit.
int base_to_unit(double quantity, enum base base, enum unit unit, double *quantity_out);

/// Render @c quantity of @c base as @c unit.
/// @return Reference to string, user deallocates.
char *base_render(double quantity, enum base base, enum unit unit);
