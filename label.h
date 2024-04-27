#include "unit.h"

#include <wchar.h>

/// Print synonyms for @c unit to stdout.
void label_synonyms(enum unit unit);

/// Parse unit label in @c s.
/// @return unit
/// @return @c p is updated to point to tail of @c s after the matching unit label.
enum unit label_lookup(wchar_t *s, wchar_t **p);
