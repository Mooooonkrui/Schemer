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
    if (primitives.find(s) != primitives.end()) throw RuntimeError("Syntax Invalid");
    else return Expr(new Var(s));
}

Expr TrueSyntax::parse(Assoc &env) {
    return Expr(new True());
}

Expr FalseSyntax::parse(Assoc &env) {
    return Expr(new False());
}

Expr List::parse(Assoc &env) {
    if (stxs.empty()) return Expr(new MakeVoid());
    if (auto first_one = dynamic_cast<Identifier *>(stxs.front().get())) {
        auto match_reserved_word = reserved_words.find(first_one->s);
        auto match_primitive = primitives.find(first_one->s);
        ExprType flag;
        if (match_reserved_word != reserved_words.end()) {
            flag = match_reserved_word->second;
        } else if (match_primitive != primitives.end()) {
            flag = match_primitive->second;
        } else {
            std::vector<Expr> para;
            for (int i = 1; i < stxs.size(); i++) {
                para.push_back(stxs[i]->parse(env));
            }
            return Expr(new Apply(stxs[0].parse(env), std::move(para)));
        }
        switch (flag) {
            case E_LET: {
                std::vector<std::pair<std::string, Expr>> bind;
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: let parameters do not equal 3");
                if (auto second_one = dynamic_cast<List *>(stxs[1].get())) {
                    for (auto sub: second_one->stxs) {
                        if (auto sub_one = dynamic_cast<List *>(sub.get())) {
                            if (sub_one->stxs.size() == 2) {
                                if (sub_one->stxs[0]->type() == SynIdentifier) {
                                    if (auto ident = dynamic_cast<Var *>(sub_one->stxs[0]->parse(env).get())) {
                                        auto tmp = *ident;
                                        bind.push_back({tmp.x, sub_one->stxs[1]->parse(env)});
                                    } else
                                        throw RuntimeError("Syntax Invalid: primitive name as binding name");
                                } else
                                    throw RuntimeError("Syntax Invalid: non-symbol object as binding name");
                            } else
                                throw RuntimeError("Syntax Invalid: wrong binding parameters");
                        } else
                            throw RuntimeError("Syntax Invalid: binding pair not list");
                    }
                    return Expr(new Let(std::move(bind), stxs[2]->parse(env)));
                } else
                    throw RuntimeError("Syntax Invalid: binding list not list");
            }
            case E_LAMBDA: {
                std::vector<std::string> para;
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: lambda parameters do not equal 3");
                if (auto second_one = dynamic_cast<List *>(stxs[1].get())) {
                    for (auto sub: second_one->stxs) {
                        if (auto sub_one = dynamic_cast<Var *>(sub->parse(env).get())) {
                            para.push_back(sub_one->x);
                        } else
                            throw RuntimeError("Syntax Invalid: primitive name or non-symbol object as binding name");
                    }
                    return Expr(new Lambda(std::move(para), stxs[2]->parse(env)));
                } else
                    throw RuntimeError("Syntax Invalid: lambda parameters list not list");
            }
            case E_LETREC: {
                std::vector<std::pair<std::string, Expr>> bind;
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: letrec parameters do not equal 3");
                if (auto second_one = dynamic_cast<List *>(stxs[1].get())) {
                    for (auto sub: second_one->stxs) {
                        if (auto sub_one = dynamic_cast<List *>(sub.get())) {
                            if (sub_one->stxs.size() == 2) {
                                if (sub_one->stxs[0]->type() == SynIdentifier) {
                                    if (auto ident = dynamic_cast<Var *>(sub_one->stxs[0]->parse(env).get())) {
                                        auto tmp = *ident;
                                        bind.push_back({tmp.x, sub_one->stxs[1]->parse(env)});
                                    } else
                                        throw RuntimeError("Syntax Invalid: primitive name as binding name");
                                } else
                                    throw RuntimeError("Syntax Invalid: non-symbol object as binding name");
                            } else
                                throw RuntimeError("Syntax Invalid: wrong binding parameters");
                        } else
                            throw RuntimeError("Syntax Invalid: binding pair not list");
                    }
                    return Expr(new Letrec(std::move(bind), stxs[2]->parse(env)));
                } else
                    throw RuntimeError("Syntax Invalid: binding list not list");
            }
            case E_IF: {
                if (stxs.size() != 4) throw RuntimeError("Syntax Invalid: if parameters do not equal 4");
                return Expr(new If(stxs[1].parse(env), stxs[2].parse(env), stxs[3].parse(env)));
            }
            case E_BEGIN: {
                if (stxs.size() == 1) throw RuntimeError("Syntax Invalid: begin has no parameter");
                std::vector<Expr> seq;
                for (int i = 1; i < stxs.size(); i++) {
                    seq.push_back(stxs[i].parse(env));
                }
                return Expr(new Begin(std::move(seq)));
            }
            case E_QUOTE: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: quote parameters not 2");
                return Expr(new Quote(stxs[1]));
            }
            case E_VOID: {
                if (stxs.size() != 1) throw RuntimeError("Syntax Invalid: make void has parameter");
                return Expr(new MakeVoid());
            }
            case E_EXIT: {
                if (stxs.size() != 1) throw RuntimeError("Syntax Invalid: exit has parameter");
                return Expr(new Exit());
            }
            case E_MUL: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: mul parameters do not equal to 3");
                return Expr(new Mult(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_PLUS: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: add parameters do not equal to 3");
                return Expr(new Plus(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_MINUS: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: sub parameters do not equal to 3");
                return Expr(new Minus(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_LT: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: < parameters do not equal to 3");
                return Expr(new Less(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_LE: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: <= parameters do not equal to 3");
                return Expr(new LessEq(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_EQ: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: = parameters do not equal to 3");
                return Expr(new Equal(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_GE: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: >= parameters do not equal to 3");
                return Expr(new GreaterEq(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_GT: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: > parameters do not equal to 3");
                return Expr(new Greater(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_EQQ: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: == parameters do not equal to 3");
                return Expr(new IsEq(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_CONS: {
                if (stxs.size() != 3) throw RuntimeError("Syntax Invalid: cons parameters do not equal to 3");
                return Expr(new Cons(stxs[1].parse(env), stxs[2].parse(env)));
            }
            case E_BOOLQ: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: bool? parameters do not equal to 2");
                return Expr(new IsBoolean(stxs[1].parse(env)));
            }
            case E_INTQ: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: fixnum? parameters do not equal to 2");
                return Expr(new IsFixnum(stxs[1].parse(env)));
            }
            case E_SYMBOLQ: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: symbol? parameters do not equal to 2");
                return Expr(new IsSymbol(stxs[1].parse(env)));
            }
            case E_NULLQ: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: null? parameters do not equal to 2");
                return Expr(new IsNull(stxs[1].parse(env)));
            }
            case E_PAIRQ: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: pair? parameters do not equal to 2");
                return Expr(new IsPair(stxs[1].parse(env)));
            }
            case E_PROCQ: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: proc? parameters do not equal to 2");
                return Expr(new IsProcedure(stxs[1].parse(env)));
            }
            case E_NOT: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: not parameters do not equal to 2");
                return Expr(new Not(stxs[1].parse(env)));
            }
            case E_CAR: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: car parameters do not equal to 2");
                return Expr(new Car(stxs[1].parse(env)));
            }
            case E_CDR: {
                if (stxs.size() != 2) throw RuntimeError("Syntax Invalid: cdr parameters do not equal to 2");
                return Expr(new Cdr(stxs[1].parse(env)));
            }
            case E_VAR: {
                std::vector<Expr> para;
                for (int i = 1; i < stxs.size(); i++) {
                    para.push_back(stxs[i]->parse(env));
                }
                return Expr(new Apply(stxs[0].parse(env), std::move(para)));
            }
            default:
                throw RuntimeError("Syntax Invalid: unknown syntax");
        }
    } else if (auto first_one = dynamic_cast<List *>(stxs.front().get())) {
        std::vector<Expr> para;
        for (int i = 1; i < stxs.size(); i++) {
            para.push_back(stxs[i]->parse(env));
        }
        return Expr(new Apply(stxs[0].parse(env), std::move(para)));
    } else
        throw RuntimeError("Syntax Invalid: unknown syntax");
}

#endif