#ifndef SYNTAX 
#define SYNTAX

#include <cstring>
#include <memory>
#include <vector>
#include "Def.hpp"
#include "shared.hpp"

enum SyntaxType{
    SynNumber, SynTrueSyntax, SynFalseSyntax, SynIdentifier, SynList
};

// Backend implement of Syntax
struct SyntaxBase {
  virtual Expr parse(Assoc &) = 0;
  virtual void show(std::ostream &) = 0;
  virtual SyntaxType type() = 0;
  virtual ~SyntaxBase() = default;
};

// Outer API for syntax [SharedPointer reversed for quote?]
struct Syntax {
    SharedPtr<SyntaxBase> ptr;
    // std :: shared_ptr<SyntaxBase> ptr;
    Syntax(SyntaxBase *);
    SyntaxBase* operator -> () const; 
    SyntaxBase& operator * ();
    SyntaxBase* get() const;
    Expr parse(Assoc &);
};

// Integer always been separated?
struct Number : SyntaxBase {
    int n;
    Number(int);
    virtual Expr parse(Assoc &) override;
    virtual void show(std::ostream &) override;
    virtual SyntaxType type() {return SynNumber;}
};

// Literal Boolean
struct TrueSyntax : SyntaxBase {
    // TrueSyntax();
    virtual Expr parse(Assoc &) override;
    virtual void show(std :: ostream &) override;
    virtual SyntaxType type() {return SynTrueSyntax;}
};

struct FalseSyntax : SyntaxBase {
    // FalseSyntax();
    virtual Expr parse(Assoc &) override;
    virtual void show(std :: ostream &) override;
    virtual SyntaxType type() {return SynFalseSyntax;}
};

// Identifier
struct Identifier : SyntaxBase {
    std::string s;
    Identifier(const std::string &);
    virtual Expr parse(Assoc &) override;
    virtual void show(std::ostream &) override;
    virtual SyntaxType type() {return SynIdentifier;}
};

// List of Objects
struct List : SyntaxBase {
    std :: vector<Syntax> stxs;
    List();
    virtual Expr parse(Assoc &) override;
    virtual void show(std::ostream &) override;
    virtual SyntaxType type() {return SynList;}
};

Syntax readSyntax(std::istream &);

std::istream &operator>>(std::istream &, Syntax);
#endif