#ifndef PARSER
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include <map>
#include <cstring>
#include <iostream>

#define mp make_pair
using std::string;
using std::vector;
using std::pair;

extern std::map<std::string, ExprType> primitives;
extern std::map<std::string, ExprType> reserved_words;

Expr Syntax::parse(Assoc &env) {
    return ptr->parse(env);
}

Expr Number::parse(Assoc &env) {
    return Expr(new Fixnum(n));
}

Expr Identifier::parse(Assoc &env) {
    if (primitives.find(s) != primitives.end()) TRE;
    else return Expr(new Var(s));
}

Expr TrueSyntax::parse(Assoc &env) {
    return Expr(new True());
}

Expr FalseSyntax::parse(Assoc &env) {
    return Expr(new False());
}

Expr List::parse(Assoc &env) {
    if (stxs.empty()) TRE;
    if (auto first_one = dynamic_cast<Identifier *>(stxs.front().get())) {
        auto match_reserved_word = reserved_words.find(first_one->s);
        auto match_primitive = primitives.find(first_one->s);
        ExprType flag;
        if (match_reserved_word != reserved_words.end()) {
            flag = match_reserved_word->second;
        } else if (match_primitive != primitives.end()) {
            flag = match_primitive->second;
        } else
            TRE;
        switch (flag) {
            case E_LET: {
                std::vector<std::pair<std::string, Expr>> bind;
                if (stxs.size() != 3) TRE;
                if (auto second_one = dynamic_cast<List *>(stxs[1].get())) {
                    for (auto sub: second_one->stxs) {
                        if (auto sub_one = dynamic_cast<List *>(sub.get())) {
                            if (sub_one->stxs.size() == 2) {
                                if (sub_one->stxs[0]->type() == SynIdentifier) {
                                    if (auto ident = dynamic_cast<Var *>(sub_one->stxs[0]->parse(env).get())) {
                                        bind.push_back({ident->x, sub_one->stxs[1]->parse(env)});
                                    } else
                                        TRE;
                                } else
                                    TRE;
                            } else
                                TRE;
                        } else
                            TRE;
                    }
                    return Expr(new Let(std::move(bind), stxs[2]->parse(env)));
                } else
                    TRE;
            }
            case E_LAMBDA: {
                std::vector<std::string> para;
                if (stxs.size() != 3) TRE;
                if (auto second_one = dynamic_cast<List *>(stxs[1].get())) {
                    for (auto sub: second_one->stxs) {
                        if (auto sub_one = dynamic_cast<Var *>(sub->parse(env).get())) {
                            para.push_back(sub_one->x);
                        } else
                            TRE;
                    }
                    return Expr(new Lambda(std::move(para), stxs[2]->parse(env)));
                } else
                    TRE;
            }
            case E_LETREC: {
                std::vector<std::pair<std::string, Expr>> bind;
                if (stxs.size() != 3) TRE;
                if (auto second_one = dynamic_cast<List *>(stxs[1].get())) {
                    for (auto sub: second_one->stxs) {
                        if (auto sub_one = dynamic_cast<List *>(sub.get())) {
                            if (sub_one->stxs.size() == 2) {
                                if (sub_one->stxs[0]->type() == SynIdentifier) {
                                    if (auto ident = dynamic_cast<Var *>(sub_one->stxs[0]->parse(env).get())) {
                                        bind.push_back({ident->x, sub_one->stxs[1]->parse(env)});
                                    } else
                                        TRE;
                                } else
                                    TRE;
                            } else
                                TRE;
                        } else
                            TRE;
                    }
                    return Expr(new Letrec(std::move(bind), stxs[2]->parse(env)));
                } else
                    TRE;
            }
            case E_IF: {
                if (stxs.size() != 4) TRE;
                return Expr(new If(stxs[1].parse(env), stxs[2].parse(env), stxs[3].parse(env)));
            }
            case E_BEGIN: {
                if (stxs.size() == 1) TRE;
                std::vector<Expr> seq;
                for (int i = 1; i < stxs.size(); i++) {
                    seq.push_back(stxs[i].parse(env));
                }
                return Expr(new Begin(std::move(seq)));
            }
            case E_QUOTE: {
                if (stxs.size() != 2) TRE;
                return Expr(new Quote(stxs[1]));
            }
            case E_VOID: {
                if (stxs.size() != 1) TRE;
                return Expr(new MakeVoid());
            }
            case E_EXIT: {
                if (stxs.size() != 1) TRE;
                return Expr(new Exit());
            }
            case E_MUL: {
                if (stxs.size() != 3) TRE;
                return Expr(new Mult(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_PLUS: {
                if (stxs.size() != 3) TRE;
                return Expr(new Plus(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_MINUS: {
                if (stxs.size() != 3) TRE;
                return Expr(new Minus(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_LT: {
                if (stxs.size() != 3) TRE;
                return Expr(new Less(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_LE: {
                if (stxs.size() != 3) TRE;
                return Expr(new LessEq(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_EQ: {
                if (stxs.size() != 3) TRE;
                return Expr(new Equal(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_GE: {
                if (stxs.size() != 3) TRE;
                return Expr(new GreaterEq(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_GT: {
                if (stxs.size() != 3) TRE;
                return Expr(new Greater(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_EQQ: {
                if (stxs.size() != 3) TRE;
                return Expr(new IsEq(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_CONS: {
                if (stxs.size() != 3) TRE;
                return Expr(new Cons(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_BOOLQ: {
                if (stxs.size() != 2) TRE;
                return Expr(new IsBoolean(stxs[1].parse(env)));
            }
            case E_INTQ: {
                if (stxs.size() != 2) TRE;
                return Expr(new IsFixnum(stxs[1].parse(env)));
            }
            case E_SYMBOLQ: {
                if (stxs.size() != 2) TRE;
                return Expr(new IsSymbol(stxs[1].parse(env)));
            }
            case E_NULLQ: {
                if (stxs.size() != 2) TRE;
                return Expr(new IsNull(stxs[1].parse(env)));
            }
            case E_PAIRQ: {
                if (stxs.size() != 2) TRE;
                return Expr(new IsPair(stxs[1].parse(env)));
            }
            case E_PROCQ: {
                if (stxs.size() != 2) TRE;
                return Expr(new IsProcedure(stxs[1].parse(env)));
            }
            case E_NOT: {
                if (stxs.size() != 2) TRE;
                return Expr(new Not(stxs[1].parse(env)));
            }
            case E_CAR: {
                if (stxs.size() != 2) TRE;
                return Expr(new Car(stxs[1].parse(env)));
            }
            case E_CDR: {
                if (stxs.size() != 2) TRE;
                return Expr(new Cdr(stxs[1].parse(env)));
            }
            case E_VAR: {
                if (stxs.size() == 1) TRE;
                std::vector<Expr> para;
                for (int i = 1; i < stxs.size(); i++) {
                    para.push_back(stxs[i]->parse(env));
                }
                return Expr(new Apply(stxs[0].parse(env), std::move(para)));
            }
            default:
                TRE;
        }
    } else if (auto first_one = dynamic_cast<List *>(stxs.front().get())) {
        //FIXME: Apply (let ([op (lambda (x) (* x x))]) (op 7))
        if (stxs.size() == 1) TRE;
        std::vector<Expr> para;
        for (int i = 1; i < stxs.size(); i++) {
            para.push_back(stxs[i]->parse(env));
        }
        return Expr(new Apply(stxs[0].parse(env), std::move(para)));
    } else
        TRE;
}

#endif