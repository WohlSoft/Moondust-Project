#include "bool_reseter.h"

BoolReseter::BoolReseter(bool *_var, bool _to) {var=_var; to=_to; atomic_var=nullptr; }

BoolReseter::BoolReseter(std::atomic<bool> *_var, bool _to) {var=nullptr; to=_to; atomic_var=_var; }

BoolReseter::~BoolReseter() { if(var) *var=to; if(atomic_var) *atomic_var=to;}
