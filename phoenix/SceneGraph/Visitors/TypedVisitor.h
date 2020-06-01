#pragma once

#include <ltl/Tuple.h>
#include "NodeVisitor.h"

namespace phx {
template <typename F, typename... Ts>
struct TypedVisitor : public NodeVisitor {
    TypedVisitor(ltl::type_list_t<Ts...>, F f) noexcept : m_f{std::move(f)} {}

    bool visit(phx::AbstractNode &node) override {
        bool res = true;
        auto caller = [this, &node, &res](auto type) {
            if (auto ptr = dynamic_cast<decltype_t(type) *>(std::addressof(node))) {
                constexpr auto returnType = type_from(ltl::invoke(m_f, *ptr));
                if_constexpr(returnType == ltl::type_v<bool>) { //
                    res = ltl::invoke(m_f, *ptr);
                }
                else {
                    ltl::invoke(m_f, *ptr);
                }
                return true;
            }
            return false;
        };

        (... || (caller(ltl::type_v<Ts>)));
        return res;
    }

    F m_f;
};
}
