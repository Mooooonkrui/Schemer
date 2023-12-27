#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std::map<std::string, ExprType> primitives;
extern std::map<std::string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {
    int n = bind.size();
    std::vector<Value> pre_eval;
    Assoc env_next = env;
    for (auto link: bind) pre_eval.push_back(link.second->eval(env));
    for (int i = 0; i < n; i++) {
        //std::cout << bind[i].first << " binded" << std::endl;
        env_next.ptr = SharedPtr<AssocList>(new AssocList(bind[i].first, std::move(pre_eval[i]), env_next));
    }
    auto ans = body->eval(env_next);
    return ans;
}

Value Lambda::eval(Assoc &env) {
    /*
    for (auto para: x) {
        std::cout << para << " binded, value: #<void> \n";
        env_next = Assoc(new AssocList(para, VoidV(), env_next));
    }
     */
    return ClosureV(x, e, env);
}

Value Apply::eval(Assoc &e) {
    auto func = rator->eval(e);
    if (func->v_type != V_PROC) {
        throw RuntimeError("Runtime Error: non-closure object as closure");
    } else {
        auto tmp = *dynamic_cast<Closure *>(func.get());
        if (tmp.parameters.size() != rand.size()) {
            throw RuntimeError("Runtime Error: closure parameters do not correspond with given instances");
        } else {
            Assoc env_next = tmp.env;
            std::vector<std::pair<std::string, Expr>> bind_list;
            for (int i = 0; i < tmp.parameters.size(); i++) {
                bind_list.push_back({tmp.parameters[i], rand[i]});
            }
            int n = bind_list.size();
            std::vector<Value> pre_eval;
            for (auto link: bind_list) pre_eval.push_back(link.second->eval(e));
            for (int i = 0; i < n; i++) {
                //std::cout << bind_list[i].first << " binded" << std::endl;
                env_next.ptr = SharedPtr<AssocList>(
                        new AssocList(bind_list[i].first, std::move(pre_eval[i]), env_next));
            }
            auto ans = tmp.e->eval(env_next);
            return ans;
        }
    }
}

Value Letrec::eval(Assoc &env) {
    int n = bind.size();
    Assoc env_bak = env;
    for (int i = 0; i < n; i++) {
        //std::cout << bind[i].first << " binded" << std::endl;
        env_bak.ptr = SharedPtr<AssocList>(new AssocList(bind[i].first, bind[i].second->eval(env_bak), env_bak));
        //env = Assoc(new AssocList(bind[i].first, bind[i].second->eval(env), env));
    }

    /*
    Assoc env_tmp = env_bak;
    while (env_tmp.get() != env.get()) {
        if (env_tmp->v->v_type == V_PROC) {
            Assoc env_tmp2 = env_bak;
            auto tmp = dynamic_cast<Closure *>(env_tmp->v.get());
            while (env_tmp2.get() != env.get()) {
                tmp->env.ptr = SharedPtr<AssocList>(new AssocList(env_tmp2->x, env_tmp2->v, tmp->env));
                env_tmp2 = env_tmp2->next;
            }
        }
        env_tmp = env_tmp->next;
    }
     */

    Assoc env_tmp = env_bak;
    while (env_tmp.get() != env.get()) {
        std::vector<Value> stack;
        auto root = env_tmp->v;
        while (root->v_type == V_PROC || root->v_type == V_PAIR) {
            stack.push_back(root);
            if (root->v_type == V_PAIR) root = dynamic_cast<Pair *>(root.get())->car;
            else break;
        }
        while (!stack.empty()) {
            auto tmp = stack.back();
            stack.pop_back();
            if (tmp->v_type == V_PROC) {
                auto tmp2 = dynamic_cast<Closure *>(tmp.get());
                Assoc tmp3 = env_bak;
                while (tmp3.get() != env.get()) {
                    tmp2->env = Assoc(new AssocList(tmp3->x, tmp3->v, tmp2->env));
                    tmp3 = tmp3->next;
                }
            } else {
                //stack.push_back(dynamic_cast<Pair*>(tmp.get())->car);
                auto stem = dynamic_cast<Pair *>(tmp.get())->cdr;
                while (stem->v_type == V_PROC || stem->v_type == V_PAIR) {
                    stack.push_back(stem);
                    if (stem->v_type == V_PAIR) stem = dynamic_cast<Pair *>(stem.get())->car;
                    else break;
                }
            }
        }
        env_tmp = env_tmp->next;
    }

    auto ans = body->eval(env_bak);
    if (ans->v_type == V_PROC) dynamic_cast<Closure *>(ans.get())->env = env_bak;
    return ans;
}

Value Var::eval(Assoc &e) {
    auto ptr = e;
    Value ans = VoidV();
    while (ptr.get()) {
        if (ptr->x == x && ptr->v->v_type != V_VOID) return ptr->v;
        ptr = ptr->next;
    }
    throw RuntimeError("Runtime Error: binding of " + x + " did not found");
}

Value Fixnum::eval(Assoc &e) {
    return IntegerV(n);
} // evaluation of a fixnum

Value If::eval(Assoc &e) {
    auto judge = cond->eval(e);
    if (auto tmp = dynamic_cast<Boolean *>(judge.get())) {
        if (tmp->b == false) return alter->eval(e);
    }
    return conseq->eval(e);
} // if expression

Value True::eval(Assoc &e) {
    return BooleanV(true);
} // evaluation of #t

Value False::eval(Assoc &e) {
    return BooleanV(false);
} // evaluation of #f

Value Begin::eval(Assoc &e) {
    return es.back()->eval(e);
} // begin expression

Value Quote::eval(Assoc &e) {
    switch (s->type()) {
        case SynIdentifier:
            return SymbolV(dynamic_cast<Identifier *>(s.get())->s);
        case SynNumber:
            return IntegerV(dynamic_cast<Number *>(s.get())->n);
        case SynTrueSyntax:
            return BooleanV(true);
        case SynFalseSyntax:
            return BooleanV(false);
        case SynList:
            auto tmp = *dynamic_cast<List *>(s.get());
            if (tmp.stxs.empty()) return NullV();
            else {
                Value slot = NullV();
                for (auto x = tmp.stxs.rbegin(); x != tmp.stxs.rend(); ++x) {
                    slot = PairV(Quote(*x).eval(e), slot);
                }
                return slot;
            }
    }
}

Value MakeVoid::eval(Assoc &e) {
    return VoidV();
} // (void)

Value Exit::eval(Assoc &e) {
    return TerminateV();
} // (exit)

Value Binary::eval(Assoc &e) {
    return evalRator(rand1->eval(e), rand2->eval(e));
} // evaluation of two-operators primitive

Value Unary::eval(Assoc &e) {
    return evalRator(rand->eval(e));
} // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return IntegerV(dynamic_cast<Integer *>(rand1.get())->n * dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: multiplication failed");
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return IntegerV(dynamic_cast<Integer *>(rand1.get())->n + dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: addition failed");
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return IntegerV(dynamic_cast<Integer *>(rand1.get())->n - dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: subtraction failed");
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n < dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: < compare failed");
} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n <= dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: <= compare failed");
} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n == dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: == compare failed");
} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n >= dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: >= compare failed");
} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n > dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error: > compare failed");
} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        if (dynamic_cast<Integer *>(rand1.get())->n == dynamic_cast<Integer *>(rand2.get())->n) {
            return BooleanV(true);
        } else return BooleanV(false);
    } else if (rand1->v_type == V_BOOL && rand2->v_type == V_BOOL) {
        if (dynamic_cast<Boolean *>(rand1.get())->b == dynamic_cast<Boolean *>(rand2.get())->b) {
            return BooleanV(true);
        } else return BooleanV(false);
    } else if (rand1->v_type == V_SYM && rand2->v_type == V_SYM) {
        if (dynamic_cast<Symbol *>(rand1.get())->s == dynamic_cast<Symbol *>(rand2.get())->s) {
            return BooleanV(true);
        } else return BooleanV(false);
    } else if (rand1.get() == rand2.get()) {
        return BooleanV(true);
    } else return BooleanV(false);
} // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) {
    return PairV(rand1, rand2);
} // cons

Value IsBoolean::evalRator(const Value &rand) {
    if (rand->v_type == V_BOOL) return BooleanV(true);
    else return BooleanV(false);
} // boolean?

Value IsFixnum::evalRator(const Value &rand) {
    if (rand->v_type == V_INT) return BooleanV(true);
    else return BooleanV(false);
} // fixnum?

Value IsSymbol::evalRator(const Value &rand) {
    if (rand->v_type == V_SYM) return BooleanV(true);
    else return BooleanV(false);
} // symbol?

Value IsNull::evalRator(const Value &rand) {
    if (rand->v_type == V_NULL) return BooleanV(true);
    else return BooleanV(false);
} // null?

Value IsPair::evalRator(const Value &rand) {
    if (rand->v_type == V_PAIR) return BooleanV(true);
    else return BooleanV(false);
} // pair?

Value IsProcedure::evalRator(const Value &rand) {
    if (rand->v_type == V_PROC) return BooleanV(true);
    else return BooleanV(false);
} // procedure?

Value Not::evalRator(const Value &rand) {
    if (dynamic_cast<Boolean *>(rand.get())->b == false) {
        return BooleanV(true);
    }
    return BooleanV(false);
} // not

Value Car::evalRator(const Value &rand) {
    if (rand->v_type == V_PAIR) {
        return dynamic_cast<Pair *>(rand.get())->car;
    } else throw RuntimeError("Runtime Error: can not destruct a non-pair object");
} // car

Value Cdr::evalRator(const Value &rand) {
    if (rand->v_type == V_PAIR) {
        return dynamic_cast<Pair *>(rand.get())->cdr;
    } else throw RuntimeError("Runtime Error: can not destruct a non-pair object");
} // cdr
