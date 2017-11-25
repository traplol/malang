#include <stdio.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <streambuf>
#include <vector>
#include <iostream>

#include "system_args.hpp"
#include "platform.hpp"
#include "parser.hpp"
#include "visitors/ast_pretty_printer.hpp"
#include "vm/vm.hpp"
#include "vm/runtime.hpp"
#include "codegen/codegen.hpp"
#include "codegen/disassm.hpp"
#include "codegen/ir_to_code.hpp"
#include "ir/ast_to_ir.hpp"
#include "ir/scope_lookup.hpp"

struct Parse_Test
{
    std::string input;
    std::vector<std::string> expected;
    Parse_Test(const std::string &input, const char *expected)
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
    Bound_Function_Map builtins;
    Type_Map types;
    Module_Map modules;
    Malang_Runtime::init_types(builtins, types);
    Parser parser(&types, &modules);
    try
    {
        auto src = new Source_Code("test.a", test.input);
        auto ast = parser.parse(src);
        if (parser.errors)
        {
            printf("there were parsing errors...\n");
            return {""};
        }
        Ast_Pretty_Printer pp;
        return pp.to_strings(ast);
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

        {"true", "true"},
        {"false", "false"},

        {"x := true", "x : bool = true"},
        {"x := false", "x : bool = false"},

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
         "    x := 5\n"
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

        {"x()()", "x()()"},
        {"y(1)(2)", "y(1)(2)"},
        {"z(y(1)(2))", "z(y(1)(2))"},
        {"y(1)[44](2)", "y(1)[44](2)"},

        {"while 1 { }",
         "while 1 {\n"
         "}"},

        {"while true { print(42) }",
         "while true {\n"
         "    print(42)\n"
         "}"},

        {"while 1 && 2 { print(42) }",
         "while (1 && 2) {\n"
         "    print(42)\n"
         "}"},

        // Array literal
        {"[1,2,3,4]", "[1, 2, 3, 4]"},
        {"[4]", "[4]"},
        {"[[4]]", "[[4]]"},
        {"[[4],[n+1],[1,2,3]]",
         "[[4], [(n + 1)], [1, 2, 3]]"},
        {"x := [1,2,3,4]", "x : = [1, 2, 3, 4]"},
        {"x := [4]", "x : = [4]"},
        {"x := [[4]]", "x : = [[4]]"},
        // New array
        {"x : [][]int = [10][]int",
         "x : [][]int = [10][]int"},
        {"y : []int = [n]int",
         "y : []int = [n]int"},
        {"y := [n]int",
         "y : []int = [n]int"},

        {"x.y", "x.y"},
        {"x.y.z", "x.y.z"},
        {"x . y . z", "x.y.z"},
        {"x.y().z.a", "x.y().z.a"},
        {"x(1).y[2].z", "x(1).y[2].z"},

        {"fn () {}()", // Immediatly Invoked Function Execution
         "fn () -> void {\n"
         "}()"},

        {"fn bound_func() -> void {}",
         "fn bound_func() -> void {\n"
         "}"},

        {"extend int {}",
         "extend int {\n"
         "}"},
        {"extend string {\n"
         "    fn +@ () -> string {\n"
         "        return self\n"
         "    }\n"
         "}",
         "extend string {\n"
         "    fn +@() -> string {\n"
         "        return self\n"
         "    }\n"
         "}"},

        {"x += 1", "x = (x + 1)"},
        {"x -= 1", "x = (x - 1)"},
        {"x *= 1", "x = (x * 1)"},
        {"x /= 1", "x = (x / 1)"},
        {"x %= 1", "x = (x % 1)"},
        {"x <<= 1", "x = (x << 1)"},
        {"x >>= 1", "x = (x >> 1)"},
        {"x &= 1", "x = (x & 1)"},
        {"x |= 1", "x = (x | 1)"},
        {"x ^= 1", "x = (x ^ 1)"},

        {"x += n * y", "x = (x + (n * y))"},

        {"type alias Greeting = string", "type alias Greeting = string"},

        {"type Greeter = { }",
         "type Greeter = {\n"
         "}"},
        {"type Math = {\n"
         "    PI :: 3.14159\n"
         "}",
         "type Math = {\n"
         "    PI : double : 3.14159\n"
         "}"},

        {"type Vec3 = {\n"
         "    x := 0.0\n"
         "    y := 0.0\n"
         "    z := 0.0\n"
         "    new (x: double, y: double, z: double) {\n"
         "        self.x = x\n"
         "        self.y = y\n"
         "        self.z = z\n"
         "    }\n"
         "}",
         "type Vec3 = {\n"
         "    x : double = 0\n"
         "    y : double = 0\n"
         "    z : double = 0\n"
         "    new (x : double, y : double, z : double) {\n"
         "        self.x = x\n"
         "        self.y = y\n"
         "        self.z = z\n"
         "    }\n"
         "}"},

        {"x := 1\n"
         "   + 2\n"
         "   + 3",
         {"x : int = 1", "(+2)", "(+3)"}},

        {"x := 1 \\ \n"
         "   + 2 \\ \n"
         "   + 3",
         "x : int = ((1 + 2) + 3)"},

        {"println(1+2)\n"
         "(3*4).print()",
         {"println((1 + 2))", "(3 * 4).print()"}},

        {"println(1+2) \\ \n"
         "(3*4).print()",
         "println((1 + 2))((3 * 4)).print()"},

        {"continue break continue",
         {"continue", "break", "continue"}},

        {"break", "break"},
        {"break aa", "break aa"},
        {"break 1,2,3", "break 1, 2, 3"},

        {"continue", "continue"},

        {"return", "return"},
        {"return aa", "return aa"},
        {"return 1,2,3", "return 1, 2, 3"},

        {"for thing { }",
         "for it in thing {\n"
         "}"},

        {"for thing { println(it) }",
         "for it in thing {\n"
         "    println(it)\n"
         "}"},

        {"for thing { println(it) break }",
         "for it in thing {\n"
         "    println(it)\n"
         "    break\n"
         "}"},
         
        {"for i in Range(0, 10) { }",
         "for i in Range(0, 10) {\n"
         "}"},

        {"import x",
         "import x"},

        {"import x$y",
         "import x$y"},

        {"import x $ y $ z",
         "import x$y$z"},

        {"import std $foo$ bar$ baz",
         "import std$foo$bar$baz"},

        {"import x\n"
         "import x\n"
         "import x\n"
         "import x\n"
         "import x\n" ,
         {"import x",
          "import x",
          "import x",
          "import x",
          "import x"}},

        {"a := x$ y$ z", "a : = x$y$z"},

        {"a :: x$ y$ z()", "a : : x$y$z()"},

        {"x $y $z()", "x$y$z()"},

        {"unalias x", "unalias x"},
        {"unalias(x)", "unalias x"},
        {"unalias (1 + 2)", "unalias (1 + 2)"},
        {"unalias 1 + 2", "(unalias 1 + 2)"},

    };
    int total_run = 0;
    int errors = 0;
    for (auto &&it : tests)
    {
        auto actual = get_parse_test_output(it);
        if (actual.size() != it.expected.size())
        {
            //printf("!(a:%i,e:%i)", (int)actual.size(), (int)it.expected.size());
            printf("x");
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
                errors++;
                printf("\nexpected: %s\nactual:   %s\n",
                       it.expected[i].c_str(), actual[i].c_str());
            }
            if ((total_run+1) % 40 == 0)
            {
                printf(" %d\n", (int)total_run+1);
            }
        }
    }
    printf(" %d/%d\n", total_run-errors, total_run);
}

int parse_to_code(Args *args)
{
    int res = 0;
    std::vector<String_Constant> string_constants;
    Type_Map types;
    Module_Map modules;
    // Search order:
    //  0. Relative to source file containing the import
    //  1. Relative to CWD
    //  2. Relative to mal_exe
    //  3. Relative to mal_exe/lib
    auto exe = plat::get_mal_exe_path();
    auto exe_dir = plat::get_directory(exe);

    modules.add_search_directory(plat::get_cwd());
    modules.add_search_directory(exe_dir);
    modules.add_search_directory(exe_dir + "/lib");

    Malang_IR ir{&types};
    Scope_Lookup global_scope{&ir};
    
    Malang_Runtime::init_types(global_scope.current().bound_functions(), types);
    Malang_Runtime::init_builtins(global_scope.current().bound_functions(), types);
    Malang_Runtime::init_modules(global_scope.current().bound_functions(), types, modules);

    Parser parser(&types, &modules);
    if (args->noisy)
    {
        printf("Input source:\n%s\n", args->code.c_str());
    }
    auto src = new Source_Code{args->filename};
    auto ast = parser.parse(src);

    if (args->noisy)
    {
        printf("\nGenerated AST\n");
        Ast_Pretty_Printer pp;
        auto strings = pp.to_strings(ast);
        for (auto &&s : strings)
        {
            printf("%s\n", s.c_str());
        }
        printf("\n");
    }

    if (parser.errors)
    {
        printf("there were parsing errors...\n");
        res = -1;
    }
    else
    {
        Ast_To_IR ast_to_ir;
        ast_to_ir.is_noisy(args->noisy);
        ast_to_ir.convert(ast, &ir, &modules, &global_scope, &string_constants);
        IR_To_Code ir_to_code;
        auto cg = ir_to_code.convert(ir);
        if (args->noisy)
        {
            auto disassembly = Disassembler::dis(cg->code);
            printf("Generated bytecode disassembly:\n%s\n", disassembly.c_str());
        }
        Malang_VM vm{args,
                     &types,
                     global_scope.current().bound_functions().natives(),
                     string_constants,
                     500, 100000};
        vm.load_code(cg->code);
        vm.run();
        if (args->noisy)
        {
            printf("code ran successfully.\n");
            vm.stack_trace();
        }
        delete cg;
    }
    delete src;
    return res;
}


int main(int argc, char **argv)
{
    --argc; ++argv;
    if (argc == 0)
    {
        printf("no input file, running parse tests.\n");
        parse_tests();
        return 0;
    }

    Args args;
    for (int i = 0; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg == "-q" || arg == "--quiet")
        {
            args.noisy = false;
        }
        else
        {
            args.filename = arg;
        }
    }

    
    if (!args.filename.empty())
    {
        return parse_to_code(&args);
    }
    else
    {
        printf("no input file.");
        return -1;
    }
}
