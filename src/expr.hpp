#ifndef EXPRESSION
#define EXPRESSION

#include "Def.hpp"
#include "syntax.hpp"
#include "shared.hpp"
#include <memory>
#include <cstring>
#include <vector>

// The backend implement of Expr, [easy to maintain?]
struct ExprBase
{
    ExprType e_type;
    ExprBase(ExprType);
    virtual Value eval(Assoc &) = 0;
    virtual ~ExprBase() = default;
};

// The outer API for Expr, assignable and deref-able, use SharedPtr to keep memory safe and bind sound
struct Expr {
    SharedPtr<ExprBase> ptr;
    Expr(ExprBase *);
    virtual ExprType type() {return ptr->e_type;}
    ExprBase* operator -> () const;
    ExprBase& operator * ();
    ExprBase* get() const;
};

// The derived class of Expr base, containing bind-list and body [vector-style match? <> Convert to AssocList?]
struct Let : ExprBase {
    std::vector<std::pair<std::string, Expr>> bind;
    Expr body;
    Let(const std :: vector<std :: pair<std :: string, Expr>> &, const Expr &);
    virtual Value eval(Assoc &) override;
};

// core lambda implement, [convert to let?]
struct Lambda : ExprBase {
    std::vector<std::string> x;
    Expr e;
    Lambda(const std :: vector<std :: string> &, const Expr &);
    virtual Value eval(Assoc &) override;
};

// naive function call, AssocList based
struct Apply : ExprBase {
    Expr rator;
    std::vector<Expr> rand;
    Apply(const Expr &, const std :: vector<Expr> &);
    virtual Value eval(Assoc &) override;
}; // this is used to handle function calling, where rator is the operator and rands are operands

// [convert to let? <> insert let? <> remove void check?]
struct Letrec : ExprBase {
  std::vector<std::pair<std::string, Expr>> bind;
  Expr body;
  Letrec(const std :: vector<std :: pair<std :: string, Expr>> &, const Expr &);
  virtual Value eval(Assoc &) override;
};

// Variable
struct Var : ExprBase {
  std::string x;
  Var(const std :: string &);
  virtual Value eval(Assoc &) override;
};

// Immediate Integer
struct Fixnum : ExprBase {
  int n;
  Fixnum(int);
  virtual Value eval(Assoc &) override;
};

// trivial control sequence
struct If : ExprBase {
  Expr cond;
  Expr conseq;
  Expr alter;
  If(const Expr &, const Expr &, const Expr &);
  virtual Value eval(Assoc &) override;
};

// Immediate Boolean Value
struct True : ExprBase {
  True();
  virtual Value eval(Assoc &) override;
};

// Immediate Boolean Value
struct False : ExprBase {
  False();
  virtual Value eval(Assoc &) override;
};

// Parallel Expr List
struct Begin : ExprBase {
  std::vector<Expr> es;
  Begin(const std :: vector<Expr> &);
  virtual Value eval(Assoc &) override;
};

// Syntax Quote
struct Quote : ExprBase {
  Syntax s;
  Quote(const Syntax &);
  virtual Value eval(Assoc &) override;
};

// Assigned but not evaluable
struct MakeVoid : ExprBase {
    MakeVoid();
    virtual Value eval(Assoc &) override;
};

// Break Down
struct Exit : ExprBase {
    Exit();
    virtual Value eval(Assoc &) override;
};

// Binary Operator
struct Binary : ExprBase {
    Expr rand1;
    Expr rand2;
    Binary(ExprType, const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) = 0;
    virtual Value eval(Assoc &) override;
};

// Unary Operator
struct Unary : ExprBase {
    Expr rand;
    Unary(ExprType, const Expr &);
    virtual Value evalRator(const Value &) = 0;
    virtual Value eval(Assoc &) override;
};

// Multiplication
struct Mult : Binary {
    Mult(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Addition
struct Plus : Binary {
    Plus(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Subtraction
struct Minus : Binary {
    Minus(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Less
struct Less : Binary {
    Less(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Less Or Equivalent
struct LessEq : Binary {
    LessEq(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Equivalent
struct Equal : Binary {
    Equal(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Greater Or Equivalent
struct GreaterEq : Binary {
    GreaterEq(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Greater
struct Greater : Binary {
    Greater(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Equivalence Verification
struct IsEq : Binary {
    IsEq(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Pair
struct Cons : Binary {
    Cons(const Expr &, const Expr &);
    virtual Value evalRator(const Value &, const Value &) override;
};

// Boolean Verification
struct IsBoolean : Unary {
    IsBoolean(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Integer Verification
struct IsFixnum : Unary {
    IsFixnum(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Symbol Verification
struct IsSymbol : Unary {
    IsSymbol(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Null Verification
struct IsNull : Unary {
    IsNull(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Pair Verification
struct IsPair : Unary {
    IsPair(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Procedure Verification
struct IsProcedure : Unary {
    IsProcedure(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Boolean Inverse
struct Not : Unary {
    Not(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// First of Pair
struct Car : Unary {
    Car(const Expr &);
    virtual Value evalRator(const Value &) override;
};

// Second of Pair
struct Cdr : Unary {
    Cdr(const Expr &);
    virtual Value evalRator(const Value &) override;
};

#endif
