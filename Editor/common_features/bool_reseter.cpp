#include "bool_reseter.h"

BoolReseter::BoolReseter(bool *_var, bool _to) {var=_var; to=_to; }
BoolReseter::~BoolReseter() { *var=to; }
