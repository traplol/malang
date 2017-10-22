#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>
#include <iostream>

#include "parser.hpp"
#include "visitors/ast_pretty_printer.hpp"
#include "vm/vm.hpp"
#include "vm/runtime/gc.hpp"
#include "codegen/codegen.hpp"
#include "codegen/disassm.hpp"
#include "codegen/ir_to_code.hpp"
#include "ir/ast_to_ir.hpp"

struct Parse_Test
{
    std::string input;
    std::vector<std::string> expected;
    Parse_Test(const std::string &input, const std::string &expected)
        : input(input)
        {
            this->expected.push_back(expected);
        }
    Parse_Test(const std::string &input, const std::vector<std::string> &expected)
        : input(input)
        , expected(expected)
        {}
};

std::vector<std::string> get_parse_test_output(Parse_Test &test)
{
    Type_Map types;
    Parser parser(&types);
    try
    {
        auto ast = parser.parse("test.ma", test.input);
        if (parser.errors)
        {
            printf("there were parsing errors...\n");
            return {""};
        }
        std::vector<std::string> result;
        Ast_Pretty_Printer pp;
        for (auto &&n : ast.roots)
        {
            result.push_back(pp.to_string(*n));
            pp.reset();
        }
        return result;
    }
    catch(...)
    {
        return {"<exception thrown>"};
    }
}

void parse_tests()
{
    std::vector<Parse_Test> tests =
    {
        {"", std::vector<std::string>()},

        {"1", "1"},

        {"123456", "123456"},

        {"1 2 3", {"1", "2", "3"}},

        {"-1", "(-1)"},

        {"+-1",
         "(+(-1))"},

        {"~+-1",
         "(~(+(-1)))"},

        { "! ~ + -1",
         "(!(~(+(-1))))"},

        {"  -1  + 4",
         "((-1) + 4)"},

        {"  -1  +  -4",
         "((-1) + (-4))"},

        {" 1 + 2",
         "(1 + 2)"},

        {" 1 +  2 * 3",
         "(1 + (2 * 3))"},

        {" (1 + 2) * 3",
         "((1 + 2) * 3)"},

        {"  1 +   2 * 3  / 4   - 5",
         "((1 + ((2 * 3) / 4)) - 5)"},

        {"  1 +  2 * 3   - 4  / 5",
         "((1 + (2 * 3)) - (4 / 5))"},

        {"_",
         "_"},

        {"print",
         "print"},

        {"print()",
         "print()"},

        {"print(1,2,3)",
         "print(1, 2, 3)"},

        {"a[42,5]",
         "a[42, 5]"},

        {" true == false",
         "(true == false)"},

        {"  true != false  &&  true == false",
         "((true != false) && (true == false))"},

        {"  1 != 0  &&  1 == 0",
         "((1 != 0) && (1 == 0))"},
        

        {"a : int",
         "a : int"},

        {"b : int = 1",
         "b : int = 1"},

        {"c : int = 1 + 2",
         "c : int = (1 + 2)"},

        {"d := 42",
         "d : int = 42"},

        {"e := 1 + 2",
         "e : int = (1 + 2)"},

        {"fn () -> int {}",
         "fn () -> int {\n"
         "}"},

        {"fn (a:int, b : double) -> void {}",
         "fn (a : int, b : double) -> void {\n"
         "}"},

        {"fn () -> int {\n"
         "    x := 5;\n"
         "}",
         "fn () -> int {\n"
         "    x : int = 5\n"
         "}"},

        {"fn() {}",
         "fn () -> void {\n"
         "}"},

        { "PI :: 3.14159",
          "PI : double : 3.14159" },

        {"y : fn()->xxx = fn () -> xxx {}",
         "y : fn () -> xxx = fn () -> xxx {\n"
         "}"},

        {"z : fn(fn(int)->zzz)->yyy = fn (cb: fn (int) -> zzz) -> yyy {}",
         "z : fn (fn (int) -> zzz) -> yyy = fn (cb : fn (int) -> zzz) -> yyy {\n"
         "}"},

        {"x := fn () -> int {}",
         "x : fn () -> int = fn () -> int {\n"
         "}"},

        {"x : fn () -> int ",
         "x : fn () -> int"},

        {"x :: fn () -> int {}",
         "x : fn () -> int : fn () -> int {\n"
         "}"},

        {"class MyClass {}",
         "class MyClass {\n"
         "}"},

        {"class Vec3 {\n"
         "    x : double\n"
         "    y : double\n"
         "    z : double\n"
         "}",
         "class Vec3 {\n"
         "    x : double\n"
         "    y : double\n"
         "    z : double\n"
         "}"},

        {"class MyDouble : double {}",
         "class MyDouble : double {\n"
         "}"},

        {"a[0] = 5", "a[0] = 5"},

        {"x:=-1", "x : int = (-1)"},
        {"x:=+1", "x : int = (+1)"},
        {"x:=~1", "x : int = (~1)"},

        {"x:=1+2", "x : int = (1 + 2)"},
        {"x:=1-2", "x : int = (1 - 2)"},
        {"x:=1*2", "x : int = (1 * 2)"},
        {"x:=1/2", "x : int = (1 / 2)"},
        {"x:=1%2", "x : int = (1 % 2)"},
        {"x:=1<<2", "x : int = (1 << 2)"},
        {"x:=1>>2", "x : int = (1 >> 2)"},
        {"x:=1<2", "x : bool = (1 < 2)"},
        {"x:=1>2", "x : bool = (1 > 2)"},
        {"x:=1<=2", "x : bool = (1 <= 2)"},
        {"x:=1>=2", "x : bool = (1 >= 2)"},
        {"x:=1==2", "x : bool = (1 == 2)"},
        {"x:=1!=2", "x : bool = (1 != 2)"},
        {"x:=1&2", "x : int = (1 & 2)"},
        {"x:=1|2", "x : int = (1 | 2)"},
        {"x:=1^2", "x : int = (1 ^ 2)"},

        {"x:=-1.1", "x : double = (-1.1)"},
        {"x:=+1.1", "x : double = (+1.1)"},

        {"x:=1+2.1", "x : double = (1 + 2.1)"},
        {"x:=1-2.1", "x : double = (1 - 2.1)"},
        {"x:=1*2.1", "x : double = (1 * 2.1)"},
        {"x:=1/2.1", "x : double = (1 / 2.1)"},
        {"x:=1%2.1", "x : double = (1 % 2.1)"},
        {"x:=1<2.1", "x : bool = (1 < 2.1)"},
        {"x:=1>2.1", "x : bool = (1 > 2.1)"},
        {"x:=1<=2.1", "x : bool = (1 <= 2.1)"},
        {"x:=1>=2.1", "x : bool = (1 >= 2.1)"},
        {"x:=1==2.1", "x : bool = (1 == 2.1)"},
        {"x:=1!=2.1", "x : bool = (1 != 2.1)"},

        {"x:=1.1+2.1", "x : double = (1.1 + 2.1)"},
        {"x:=1.1-2.1", "x : double = (1.1 - 2.1)"},
        {"x:=1.1*2.1", "x : double = (1.1 * 2.1)"},
        {"x:=1.1/2.1", "x : double = (1.1 / 2.1)"},
        {"x:=1.1%2.1", "x : double = (1.1 % 2.1)"},
        {"x:=1.1<2.1", "x : bool = (1.1 < 2.1)"},
        {"x:=1.1>2.1", "x : bool = (1.1 > 2.1)"},
        {"x:=1.1<=2.1", "x : bool = (1.1 <= 2.1)"},
        {"x:=1.1>=2.1", "x : bool = (1.1 >= 2.1)"},
        {"x:=1.1==2.1", "x : bool = (1.1 == 2.1)"},
        {"x:=1.1!=2.1", "x : bool = (1.1 != 2.1)"},

        {"x:=1.1+2", "x : double = (1.1 + 2)"},
        {"x:=1.1-2", "x : double = (1.1 - 2)"},
        {"x:=1.1*2", "x : double = (1.1 * 2)"},
        {"x:=1.1/2", "x : double = (1.1 / 2)"},
        {"x:=1.1%2", "x : double = (1.1 % 2)"},
        {"x:=1.1<2", "x : bool = (1.1 < 2)"},
        {"x:=1.1>2", "x : bool = (1.1 > 2)"},
        {"x:=1.1<=2", "x : bool = (1.1 <= 2)"},
        {"x:=1.1>=2", "x : bool = (1.1 >= 2)"},
        {"x:=1.1==2", "x : bool = (1.1 == 2)"},
        {"x:=1.1!=2", "x : bool = (1.1 != 2)"},

    };
    size_t total_run = 1;
    for (auto &&it : tests)
    {
        auto actual = get_parse_test_output(it);
        if (actual.size() != it.expected.size())
        {
            printf("!(a:%i,e:%i)", (int)actual.size(), (int)it.expected.size());
        }
        auto n = std::min(actual.size(), it.expected.size());
        for (size_t i = 0; i < n; ++i, ++total_run)
        {
            if (actual[i] == it.expected[i])
            {
                printf(".");
            }
            else
            {
                printf("\nexpected: %s\nactual:   %s\n",
                       it.expected[i].c_str(), actual[i].c_str());
            }
            if (total_run % 40 == 0)
            {
                printf(" %d\n", (int)total_run);
            }
        }
    }
    printf(" %d\n", (int)total_run);
}

void parse_one()
{
    Parse_Test pt =
        {"x := 5 + 2",
         "x : int = (5 + 2)"};
    auto actual = get_parse_test_output(pt);
    if (actual.size() != pt.expected.size())
    {
        printf("!(a:%i,e:%i)", (int)actual.size(), (int)pt.expected.size());
    }
    auto n = std::min(actual.size(), pt.expected.size());
    for (size_t i = 0; i < n; ++i)
    {
        if (actual[i] == pt.expected[i])
        {
        printf(".");
        }
        else
        {
            printf("\nexpected: %s\nactual:   %s\n",
                    pt.expected[i].c_str(), actual[i].c_str());
        }
    }
}

void codegen_stuff();
void gc_stuff();
void parse_stuff();
void parse_to_code();

int main()
{
    parse_to_code();
    //parse_tests();
    //parse_one();
    //gc_stuff();
    return 0;
}

void codegen_stuff()
{
    Codegen cg;
    cg.push_back_fixnum_add(42, 5);
    cg.push_back_literal_value(999);
    cg.push_back_fixnum_multiply();

    auto s = Disassembler::dis(cg.code);
    printf("%s", s.c_str());

    cg.code.push_back(0xff);
    Malang_VM vm{{}};
    vm.load_code(cg.code);
    vm.run();
}

void gc_stuff()
{
    Malang_VM vm{{}, 5};

    Type_Info obj(nullptr, 0, "object");
    Type_Info str(&obj, 1, "string");
    Type_Info fix(&obj, 2, "fixnum");
    
    vm.add_local(99999);
    vm.add_local(3.14159);
    vm.add_data(42.5);
    vm.add_global(696969);
    auto a = vm.gc->allocate(&str); vm.add_local(a); printf("%p\n", a);
    auto b = vm.gc->allocate(&obj); vm.add_local(b); printf("%p\n", b);
    auto c = vm.gc->allocate(&str); vm.add_local(c); printf("%p\n", c);
    auto d = vm.gc->allocate(&str); vm.add_global(d); printf("%p\n", d);
    auto e = vm.gc->allocate(&obj); vm.add_data(e); printf("%p\n", e);
    auto f = vm.gc->allocate(&str); vm.add_global(f); printf("%p\n", f);
    auto g = vm.gc->allocate(&fix); printf("%p\n", g);
    auto h = vm.gc->allocate(&obj); vm.add_global(h); printf("%p\n", h);
    for (int i = 0; i < 100; ++i)
    {
        printf("%p\n", vm.gc->allocate(&str));
    }
    vm.gc->manual_run();
}

void parse_stuff()
{
    parse_tests();
}

std::string to_string(const Malang_Value &value)
{
    std::stringstream ss;
    if (value.is_fixnum())
    {
        ss << value.as_fixnum();
    }
    else if (value.is_double())
    {
        ss << value.as_double();
    }
    else if (value.is_object())
    {
        ss << "Object(" << value.as_object() << ")";
    }
    else if (value.is_pointer())
    {
        ss << "Pointer(" << value.as_pointer<void>() << ")";
    }
    else
    {
        ss << "?(" << std::hex << value.bits() << ")";
    }
    return ss.str();
}

void dump(const std::vector<byte> &code, int width)
{
    for (size_t i = 0; i < code.size(); ++i)
    {
        if (i % width == 0)
        {
            printf("%08lx  ", i);
        }
        printf("%02x ", code[i]);
        if ((i+1) % width == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void parse_to_code()
{
    Type_Map types;
    Parser parser(&types);
    auto ast = parser.parse("test.ma", R"(
1+1;
sum :: fn(a: int, b: int) -> int { 
  return a + b;
}
sum(5, 10)
)"
);

    if (parser.errors)
    {
        printf("there were parsing errors...\n");
    }
    else
    {
        Ast_To_IR ast_to_ir{&types};
        auto ir = ast_to_ir.convert(ast);
        IR_To_Code ir_to_code;
        auto cg = ir_to_code.convert(*ir);
        auto disassembly = Disassembler::dis(cg->code);
        printf("%s\n", disassembly.c_str());
        dump(cg->code, 16);
        Malang_VM vm{types.primitives()};
        vm.load_code(cg->code);
        vm.run();
        printf("code ran successfully.\n");
        printf("~>%s<~\n", to_string(vm.pop_data()).c_str());
    }
}


