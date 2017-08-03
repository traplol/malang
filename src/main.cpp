#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>

#include "parser.hpp"
#include "ast/ast_value.hpp"
#include "type_info.hpp"

#define foreach(iteratable) for (auto &&it : iteratable)

int main()
{
    std::string m_code =
        //R"(-(1 + 20) * (3 - 4 / 5))";
        //R"( ((~(+(-(4)))) + 4) )";
        //R"( 4 % 5 )";
        //R"(1 - 2 - 3 - 4 - 5)";
        "print(1,2,3,4,5,6,7)";
    Parser parser;
    auto ast = parser.parse("test.ma", m_code);
    if (parser.errors)
    {
        printf("there were errors... exiting\n");
    }
    else
    {
        Execution_Context ctx;
        foreach (ast.roots)
        {
            //printf("%s\n", it->to_string().c_str());
            auto r = it->execute(ctx);
            //if (r)
            //{
            //    if (r->node_type_id() == Integer_Node::type_id())
            //    {
            //        auto ri = reinterpret_cast<Integer_Node*>(r);
            //        printf("%s (%ld)\n", ri->node_name().c_str(), ri->value);
            //    }
            //    else
            //    {
            //        printf("%s\n", r->node_name().c_str());
            //    }
            //}
        }
    }
    return 0;
}
