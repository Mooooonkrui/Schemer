#ifndef DEF_HPP
#define DEF_HPP

// By luke36

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <map>

struct Syntax;
struct Expr;
struct Value;
struct AssocList;
struct Assoc;

enum ExprType
{
    E_LET,        // let method -> expand to lambda?
    E_LAMBDA,     // lambda calculus -> implement by let?
    E_APPLY,      // WHAT'S IT?
    E_LETREC,     // convert to let -> how to implement set?
    E_VAR,        // symbol 
    E_FIXNUM,     // constant
    E_IF,         // judge and branches
    E_TRUE, E_FALSE,    // boolean value -> implicit convert 
    E_BEGIN,      // seq
    E_QUOTE,      // implement in string or symbol 
    E_VOID,       // assigned but not evaluatable
    E_MUL, E_PLUS, E_MINUS,   // primitive arithmetic operation
    E_LT, E_LE, E_EQ, E_GE, E_GT,   // compare operator 
    E_CONS,       // pair forming
    E_NOT,        // boolean invert and convert 
    E_CAR, E_CDR, // destruct pair 
    E_EQQ, E_BOOLQ, E_INTQ, E_NULLQ, E_PAIRQ, E_PROCQ, E_SYMBOLQ, // verify -> !(AST Compare) <> address 
    E_EXIT        // safety
};
enum ValueType
{
    V_INT,        // pack in union -> int32_t
    V_BOOL,       // bool
    V_SYM,        // std::string
    V_NULL,       // empty 
    V_STRING,     // std::string
    V_PAIR,       // binary tree
    V_PROC,       // lambda:(map::symbol, AST)
    V_VOID,       // pure type
    V_PRIMITIVE,  // operators
    V_TERMINATE   // safety
};

void initPrimitives();
void initReservedWords();

#endif
