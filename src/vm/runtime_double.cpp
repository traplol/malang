#include <cmath>
#include "runtime_double.hpp"
#include "../type_map.hpp"
#include "primitive_helpers.hpp"
#include "vm.hpp"

static void d_pos(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    vm.push_data(+a);
}

static void d_neg(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    vm.push_data(-a);
}

static void dd_add(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a + b);
}

static void dd_sub(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a - b);
}

static void dd_mul(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a * b);
}

static void dd_div(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a / b);
}

static void dd_mod(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(std::fmod(a, b));
}

static void dd_less(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a < b);
}

static void dd_greater(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a > b);
}

static void dd_less_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a <= b);
}

static void dd_greater_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a >= b);
}

static void dd_eql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a == b);
}

static void dd_neql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_double();
    vm.push_data(a != b);
}

static void di_add(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a + b);
}

static void di_sub(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a - b);
}

static void di_mul(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a * b);
}

static void di_div(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a / b);
}

static void di_mod(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(std::fmod(a, b));
}

static void di_less(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a < b);
}

static void di_greater(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a > b);
}

static void di_less_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a <= b);
}

static void di_greater_eq(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a >= b);
}

static void di_eql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a == b);
}

static void di_neql(Malang_VM &vm)
{
    auto a = vm.pop_data().as_double();
    auto b = vm.pop_data().as_fixnum();
    vm.push_data(a != b);
}

void runtime_double_init(Type_Map *m)
{
    auto _int    = m->get_int();
    auto _double = m->get_double();
    auto _bool   = m->get_bool();
    add_una_op_method(m, _double, "+",  _double, d_pos);
    add_una_op_method(m, _double, "-",  _double, d_neg);

    add_bin_op_method(m, _double, "+",  _double, _double, dd_add);
    add_bin_op_method(m, _double, "-",  _double, _double, dd_sub);
    add_bin_op_method(m, _double, "*",  _double, _double, dd_mul);
    add_bin_op_method(m, _double, "/",  _double, _double, dd_div);
    add_bin_op_method(m, _double, "%",  _double, _double, dd_mod);
    add_bin_op_method(m, _double, "<",  _double, _bool, dd_less);
    add_bin_op_method(m, _double, ">",  _double, _bool, dd_greater);
    add_bin_op_method(m, _double, "<=", _double, _bool, dd_less_eq);
    add_bin_op_method(m, _double, ">=", _double, _bool, dd_greater_eq);
    add_bin_op_method(m, _double, "==", _double, _bool, dd_eql);
    add_bin_op_method(m, _double, "!=", _double, _bool, dd_neql);

    add_bin_op_method(m, _double, "+",  _int, _double, di_add);
    add_bin_op_method(m, _double, "-",  _int, _double, di_sub);
    add_bin_op_method(m, _double, "*",  _int, _double, di_mul);
    add_bin_op_method(m, _double, "/",  _int, _double, di_div);
    add_bin_op_method(m, _double, "%",  _int, _double, di_mod);
    add_bin_op_method(m, _double, "<",  _int, _bool, di_less);
    add_bin_op_method(m, _double, ">",  _int, _bool, di_greater);
    add_bin_op_method(m, _double, "<=", _int, _bool, di_less_eq);
    add_bin_op_method(m, _double, ">=", _int, _bool, di_greater_eq);
    add_bin_op_method(m, _double, "==", _int, _bool, di_eql);
    add_bin_op_method(m, _double, "!=", _int, _bool, di_neql);
}
