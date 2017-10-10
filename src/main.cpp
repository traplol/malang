#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>
#include <iostream>

#include "parser.hpp"
#include "visitors/ast_pretty_printer.hpp"

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
#if 1
    parse_tests();
#else
    std::string m_code =
        //R"(-(1 + 20) * (3 - 4 / 5);)";
        //R"( ((~(+(-(4)))) + 4); )";
        //R"( 4 % 5; )";
        //R"(1 - 2 - 3 - 4 - 5;)";
        //"print(1,2,3,4,5,6,7);";
        //"x := 42;";
        //"fn () -> int { x := 1 + 2 };";
        "100 * 2.5;";
    Parser parser;
    /*
    Ast_Node_Evaluator evaluator;
    while (1)
    {
        std::string repl_code;
        std::getline(std::cin, repl_code);
        auto ast = parser.parse("repl.ma", repl_code);
        if (parser.errors)
        {
            printf("there were errors...\n");
        }
        else
        {
            printf("AST:\n%s\n", ast.to_string().c_str());
            for (auto &&it : ast.roots)
            {
                printf("%s\n", it->accept(evaluator)->to_string().c_str());
            }
        }
    }
    */
#endif
    return 0;
}
