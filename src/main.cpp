#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>
#include <iostream>

#include "parser.hpp"
#include "visitors/ast_pretty_printer.hpp"
#include "vm/vm.hpp"
#include "vm/gc.hpp"
#include "codegen/codegen.hpp"
#include "codegen/disassm.hpp"

struct Parse_Test
{
    std::string input;
    std::string output;
};

std::string get_parse_test_output(Parse_Test &test)
{
    Parser parser;
    try
    {
        auto ast = parser.parse("test.ma", test.input);
        if (parser.errors)
        {
            printf("there were parsing errors...\n");
            return "";
        }
        std::stringstream ss;
        Ast_Pretty_Printer pp;
        for (size_t i = 0; i < ast.roots.size(); ++i)
        {
            ss << pp.to_string(*ast.roots[i]);
            if (i + 1 < ast.roots.size()) ss << std::endl;
        }
        return ss.str();
    }
    catch(...)
    {
        return "<exception thrown>";
    }
}

void parse_tests()
{
    std::vector<Parse_Test> tests =
    {
        {"1", "1"},

        {"123456", "123456"},

        {"1 2 3", "1\n2\n3"},

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
         "d : = 42"},

        {"e := 1 + 2",
         "e : = (1 + 2)"},

        {"fn () -> int {}",
         "fn () -> int {\n"
         "}"},

        {
            "fn () -> int {\n"
            "    x := 5;\n"
            "}",
            "fn () -> int {\n"
            "    x : = 5\n"
            "}"
        }
    };
    for (auto &&it : tests)
    {
        auto result = get_parse_test_output(it);
        if (result == it.output)
        {
            printf(".");
        }
        else
        {
            //printf("x");
            printf("\nexpected: %s\nactual:   %s\n", it.output.c_str(), result.c_str());
        }
    }
}

int main()
{
    Codegen cg;
    cg.push_back_fixnum_add(42, 5);
    cg.push_back_literal_value(999);
    cg.push_back_fixnum_multiply();

    auto s = Disassembler::dis(cg.code);
    printf("%s", s.c_str());

    cg.code.push_back(0xff);

    Malang_VM vm;
    Malang_GC gc(&vm);

    Type_Info str;
    str.name = "string";

    Type_Info fix;
    fix.name = "fixnum";

    Type_Info obj;
    obj.name = "object";
    
    vm.add_local(99999);
    vm.add_local(3.14159);
    vm.add_data(42.5);
    vm.add_global(696969);
    auto a = gc.allocate(&str); vm.add_local(a);
    auto b = gc.allocate(&str); vm.add_local(b);
    auto c = gc.allocate(&str); vm.add_local(c);
    auto d = gc.allocate(&str); vm.add_global(d);
    auto e = gc.allocate(&str); vm.add_data(e);
    auto f = gc.allocate(&str); vm.add_global(f);
    auto g = gc.allocate(&str);
    auto h = gc.allocate(&str); vm.add_global(h);
    gc.disable_automatic();
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);
    gc.allocate(&str);

    gc.manual_run();
    gc.manual_run();
    gc.manual_run();
    gc.manual_run();

    //vm.load_code(cg.code);
    //vm.run();
    //if (vm.data_stack[vm.data_top-1].as_fixnum() == (42 + 5) * 999)
    //{
    //    printf("good!\n");
    //}
    //else
    //{
    //    printf("bad!\n");
    //}
    
//#if 1
//    parse_tests();
//#else
//    std::string m_code =
//        //R"(-(1 + 20) * (3 - 4 / 5);)";
//        //R"( ((~(+(-(4)))) + 4); )";
//        //R"( 4 % 5; )";
//        //R"(1 - 2 - 3 - 4 - 5;)";
//        //"print(1,2,3,4,5,6,7);";
//        //"x := 42;";
//        //"fn () -> int { x := 1 + 2 };";
//        "100 * 2.5;";
//    Parser parser;
//    /*
//    Ast_Node_Evaluator evaluator;
//    while (1)
//    {
//        std::string repl_code;
//        std::getline(std::cin, repl_code);
//        auto ast = parser.parse("repl.ma", repl_code);
//        if (parser.errors)
//        {
//            printf("there were errors...\n");
//        }
//        else
//        {
//            printf("AST:\n%s\n", ast.to_string().c_str());
//            for (auto &&it : ast.roots)
//            {
//                printf("%s\n", it->accept(evaluator)->to_string().c_str());
//            }
//        }
//    }
//    */
//#endif
//    return 0;
}
