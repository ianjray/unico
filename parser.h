#include "unit.h"

#include <wchar.h>

struct parser_data {
    /// Quantity in base units.
    double quantity;
    /// The base unit.
    enum base base;
    /// The source unit.
    enum unit from;
    /// The destination unit.
    enum unit to;
};

enum parser_ret {
    /// Parsing incomplete, more input needed.
    PARSE_AGAIN,
    /// Parsing complete.
    PARSE_COMPLETE,
    /// Parsing failed due to invalid arguments.
    PARSE_INVALID_ARGUMENT,
    /// Parsing failed due to incompatible compound unit.
    PARSE_INVALID_COMPOUND,
    /// Parsing failed due to invalid number.
    PARSE_INVALID_NUMBER,
    /// Parsing failed due to unknown unit.
    PARSE_UNKNOWN_UNIT,
};

/// Parser object.
struct parser;

/// Constructor.
struct parser *parser_new(void);

/// Destructor.
void parser_delete(struct parser *);

/// Add @c word.
/// Accepts QUANTITY | QUANTITY UNIT | UNIT.
/// @param term Contains the failed term (number or unit) if this function returns an error.
/// @return enum parser_ret.
enum parser_ret parser_add(struct parser *, wchar_t *arg, wchar_t **term, struct parser_data *data);
