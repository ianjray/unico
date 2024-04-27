const wchar_t *symbol_of_unit(enum unit unit)
{
    switch (unit) {
        case PresentationUnitNone:
        case PresentationUnitUnknown:
            break;

#define u(symbol, name, base_, scale)            case name : return symbol ;
#define c(symbol, name, base_, tobase, frombase) case name : return symbol ;
#include "unit.hi"
    }

    return NULL;
}

double unit_to_base(double quantity, enum unit unit, enum base *base)
{
    double X = quantity;

    switch (unit) {
        case PresentationUnitNone:
        case PresentationUnitUnknown:
            *base = BaseUnitNone;
            break;

#define u(symbol, name, base_, scale)            case name : *base = base_ ; X *= scale  ; break ;
#define c(symbol, name, base_, tobase, frombase) case name : *base = base_ ; X  = tobase ; break ;
#include "unit.hi"
    }

    return X;
}

int base_to_unit(double quantity, enum base base, enum unit unit, double *quantity_out)
{
    double X = quantity;
    int r = -EPERM;

    switch (unit) {
        case PresentationUnitNone:
        case PresentationUnitUnknown:
            break;

#define u(symbol, name, base_, scale)            case name : if (base == base_) { X /= scale   ; r = 0; } break ;
#define c(symbol, name, base_, tobase, frombase) case name : if (base == base_) { X  = frombase; r = 0; } break ;
#include "unit.hi"
    }

    if (quantity_out && !r) {
        *quantity_out = X;
    }

    return r;
}

char *base_render(double quantity, enum base base, enum unit unit)
{
    double X = quantity;
    char *s = NULL;

    switch (unit) {
        case PresentationUnitNone:
        case PresentationUnitUnknown:
            break;

#define u(symbol, name, base_, scale)            case name : if (base == base_) { X /= scale    ; asprintf(&s, "%g %ls", X, symbol); } break ;
#define c(symbol, name, base_, tobase, frombase) case name : if (base == base_) { X  = frombase ; asprintf(&s, "%g %ls", X, symbol); } break ;
#include "unit.hi"
    }

    if (unit == PresentationUnitFeetAndInches) {
        // Exception.
        const double ScaleFractionalFeetToInch = 12;
        double y = fmod(X, 1) * ScaleFractionalFeetToInch;
        free(s);
        asprintf(&s, "%ld ' %g \"", (long)X, y);
    }

    return s;
}
