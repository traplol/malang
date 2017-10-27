#include "ir.hpp"
#include "label_map.hpp"

METADATA_OVERRIDES_IMPL(IR_Node);

Malang_IR::~Malang_IR()
{
    types = nullptr;
    delete labels;
    labels = nullptr;
    roots.clear();
}
