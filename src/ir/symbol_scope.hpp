#ifndef MALANG_IR_SYMBOL_SCOPE
#define MALANG_IR_SYMBOL_SCOPE

enum class Symbol_Scope
{
    None,    // arrays
    Global,
    Local,
    Field
};

#endif /* MALANG_IR_SYMBOL_SCOPE */
