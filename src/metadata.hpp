#ifndef MALANG_METADATA_HPP
#define MALANG_METADATA_HPP

#include <string>

#define METADATA_TYPE_NAME_IMPL(class_name) std::string class_name::type_name() const { return #class_name; }

#define METADATA_TYPE_ID_IMPL(class_name)                               \
    Type_Id class_name::_type_id() {                                    \
        static Type_Id id = Metadata::next_type_id();                   \
        return id;}                                                     \
    Type_Id class_name::type_id() const { return class_name::_type_id(); }

#define METADATA_OVERRIDES_IMPL(class_name)     \
    METADATA_TYPE_NAME_IMPL(class_name)         \
    METADATA_TYPE_ID_IMPL(class_name)           \

#define METADATA_OVERRIDES                          \
    virtual std::string type_name() const override; \
    virtual Type_Id type_id() const override;       \
    static Type_Id _type_id();

using Type_Id = size_t;

struct Metadata
{
    virtual std::string to_string() const;
    virtual std::string type_name() const = 0;
    virtual Type_Id type_id() const = 0;
    static inline Type_Id next_type_id()
    {
        static Type_Id current_type_id = 0;
        return current_type_id++;
    }
};

#endif

