#ifndef TYPES_H
#define TYPES_H

#include <string>
using namespace std;

enum class TypeKind {
    I16,
    I8,
    I32,
    I64,
    F32,
    F64,
    String,
    List,
    Unknown
};

struct Type{
    TypeKind kind;
    Type* element = nullptr;
};

inline Type i8Type(){ return {TypeKind::I8}; }
inline Type i16Type(){ return {TypeKind::I16}; }
inline Type i32Type(){ return {TypeKind::I32}; }
inline Type i64Type(){ return {TypeKind::I64}; }
inline Type f32Type(){ return {TypeKind::F32}; }
inline Type f64Type(){ return {TypeKind::F64}; }
inline Type stringType(){ return {TypeKind::String}; }
inline Type listType(Type* element){ return {TypeKind::List, element}; }
inline Type unknownType(){ return {TypeKind::Unknown}; }

#endif