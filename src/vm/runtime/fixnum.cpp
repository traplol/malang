#include <cmath>
#include "fixnum.hpp"
#include "primitive_helpers.hpp"
#include "../../type_map.hpp"
#include "../vm.hpp"

#include <stdio.h>

static void i_invert(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    vm.push_data(~a);
}

static void i_pos(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    vm.push_data(+a);
}

static void i_neg(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    vm.push_data(-a);
}

static void ii_add(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a + b);
}

static void ii_sub(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a - b);
}

static void ii_mul(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a * b);
}

static void ii_div(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a / b);
}

static void ii_mod(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(std::fmod(a, b));
}

static void ii_and(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a & b);
}

static void ii_or(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a | b);
}

static void ii_xor(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a ^ b);
}

static void ii_lshift(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a << b);
}

static void ii_rshift(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a >> b);
}

static void ii_less(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a < b);
}

static void ii_greater(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a > b);
}

static void ii_less_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a <= b);
}

static void ii_greater_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a >= b);
}

static void ii_eql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a == b);
}

static void ii_neql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a != b);
}

static void id_add(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a + b);
}

static void id_sub(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a - b);
}

static void id_mul(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a * b);
}

static void id_div(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a / b);
}

static void id_mod(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(std::fmod(a, b));
}

static void id_less(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a < b);
}

static void id_greater(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a > b);
}

static void id_less_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a <= b);
}

static void id_greater_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a >= b);
}

static void id_eql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a == b);
}

static void id_neql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_fixnum();
    auto b = vm.pop_data().as_double();
    vm.push_data(a != b);
}

void Malang_Runtime::runtime_fixnum_init(Primitive_Function_Map &b, Type_Map &m)
{
    auto _int    = m.get_int();
    auto _double = m.get_double();
    auto _bool   = m.get_bool();
    add_una_op_method(b, m, _int, "~",  _int, i_invert);
    add_una_op_method(b, m, _int, "+",  _int, i_pos);
    add_una_op_method(b, m, _int, "-",  _int, i_neg);

    add_bin_op_method(b, m, _int, "+",  _int, _int, ii_add);
    add_bin_op_method(b, m, _int, "-",  _int, _int, ii_sub);
    add_bin_op_method(b, m, _int, "*",  _int, _int, ii_mul);
    add_bin_op_method(b, m, _int, "/",  _int, _int, ii_div);
    add_bin_op_method(b, m, _int, "%",  _int, _int, ii_mod);
    add_bin_op_method(b, m, _int, "&",  _int, _int, ii_and);
    add_bin_op_method(b, m, _int, "|",  _int, _int, ii_or);
    add_bin_op_method(b, m, _int, "^",  _int, _int, ii_xor);
    add_bin_op_method(b, m, _int, "<<", _int, _int, ii_lshift);
    add_bin_op_method(b, m, _int, ">>", _int, _int, ii_rshift);
    add_bin_op_method(b, m, _int, "<",  _int, _bool, ii_less);
    add_bin_op_method(b, m, _int, ">",  _int, _bool, ii_greater);
    add_bin_op_method(b, m, _int, "<=", _int, _bool, ii_less_eq);
    add_bin_op_method(b, m, _int, ">=", _int, _bool, ii_greater_eq);
    add_bin_op_method(b, m, _int, "==", _int, _bool, ii_eql);
    add_bin_op_method(b, m, _int, "!=", _int, _bool, ii_neql);

    add_bin_op_method(b, m, _int, "+",  _double, _double, id_add);
    add_bin_op_method(b, m, _int, "-",  _double, _double, id_sub);
    add_bin_op_method(b, m, _int, "*",  _double, _double, id_mul);
    add_bin_op_method(b, m, _int, "/",  _double, _double, id_div);
    add_bin_op_method(b, m, _int, "%",  _double, _double, id_mod);
    add_bin_op_method(b, m, _int, "<",  _double, _bool, id_less);
    add_bin_op_method(b, m, _int, ">",  _double, _bool, id_greater);
    add_bin_op_method(b, m, _int, "<=", _double, _bool, id_less_eq);
    add_bin_op_method(b, m, _int, ">=", _double, _bool, id_greater_eq);
    add_bin_op_method(b, m, _int, "==", _double, _bool, id_eql);
    add_bin_op_method(b, m, _int, "!=", _double, _bool, id_neql);
}
