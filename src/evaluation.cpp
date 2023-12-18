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
    for (auto link: bind) pre_eval.push_back(link.second->eval(env));
    for (int i = 0; i < n; i++) {
        env = Assoc(new AssocList(bind[i].first, std::move(pre_eval[i]), env));
    }
    auto ans = body->eval(env);
    for (int i = 0; i < n; i++) {
        auto tmp = env;
        env = tmp->next;
    }
    return ans;
}

Value Lambda::eval(Assoc &env) {
    throw RuntimeError("Function to impl");
} // TODO: lambda expression

Value Apply::eval(Assoc &e) {
    throw RuntimeError("Function to impl");
} // TODO: for function calling

Value Letrec::eval(Assoc &env) {
    int n = bind.size();
    for (int i = 0; i < n; i++) {
        env = Assoc(new AssocList(bind[i].first, std::move(VoidV()), env));
    }
    for (int i = 0; i < n; i++) {
        env = Assoc(new AssocList(bind[i].first, std::move(bind[i].second->eval(env)), env));
    }
    auto ans = body->eval(env);
    for (int i = 0; i < n; i++) {
        auto tmp = env;
        env = tmp->next;
    }
    for (int i = 0; i < n; i++) {
        auto tmp = env;
        env = tmp->next;
    }
    return ans;
}

Value Var::eval(Assoc &e) {
    auto ptr = e;
    Value ans = VoidV();
    while (ptr.get()) {
        if (ptr->x == x && ptr->v->v_type != V_VOID) return ptr->v;
        ptr = ptr->next;
    }
    throw RuntimeError("Runtime Error");
}

Value Fixnum::eval(Assoc &e) {
    return IntegerV(n);
} // evaluation of a fixnum

Value If::eval(Assoc &e) {
    auto judge = cond->eval(e);
    if (dynamic_cast<Boolean *>(judge.get())->b == false) {
        return alter->eval(e);
    } else return conseq->eval(e);
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
    throw RuntimeError("Function to impl");
} // TODO: quote expression

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
        throw RuntimeError("Runtime Error");
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return IntegerV(dynamic_cast<Integer *>(rand1.get())->n + dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return IntegerV(dynamic_cast<Integer *>(rand1.get())->n - dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n < dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n <= dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n == dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n >= dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV(dynamic_cast<Integer *>(rand1.get())->n > dynamic_cast<Integer *>(rand2.get())->n);
    } else
        throw RuntimeError("Runtime Error");
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
    }
} // car

Value Cdr::evalRator(const Value &rand) {
    if (rand->v_type == V_PAIR) {
        return dynamic_cast<Pair *>(rand.get())->cdr;
    }
} // cdr
