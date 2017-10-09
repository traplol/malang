#ifndef MALANG_METADATA_HPP
#define MALANG_METADATA_HPP

#include <string>

#define NAME_FUNC(class_name) std::string class_name::node_name() const { return #class_name; }

#define TYPE_ID_FUNC(class_name)                                        \
    Type_Id class_name::_type_id() {                                    \
        static Type_Id id = Metadata::next_type_id();                   \
        return id;}                                                     \
    Type_Id class_name::type_id() const { return class_name::_type_id(); }

#define METADATA_OVERRIDES_IMPL(class_name)                             \
    NAME_FUNC(class_name)                                               \
    TYPE_ID_FUNC(class_name)                                            \

#define METADATA_OVERRIDES                          \
    virtual std::string node_name() const override; \
    virtual std::string to_string() const override; \
    virtual Type_Id type_id() const override;       \
    static Type_Id _type_id();

using Type_Id = size_t;

struct Metadata
{
    static inline Type_Id next_type_id()
    {
        static Type_Id current_type_id = 0;
        return current_type_id++;
    }
};

#endif

