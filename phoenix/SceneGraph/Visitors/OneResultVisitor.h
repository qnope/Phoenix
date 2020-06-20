#pragma once

#include "WithResultVisitor.h"
#include <ltl/Tuple.h>
#include <optional>

namespace phx {

template <typename Result, typename F, typename... Ts>
class OneResultVisitor : public WithResultVisitor<Result> {
    static constexpr auto type = ltl::type_v<Result>;
    static constexpr auto optional_type = ltl::type_v<std::optional<Result>>;

  public:
    OneResultVisitor(F f) noexcept : m_f{FWD(f)} {}

    bool visit(AbstractNode &node) override {
        bool res = true;
        auto caller = [this, &node, &res](auto type) {
            if (auto ptr = dynamic_cast<decltype_t(type) *>(std::addressof(node))) {
                constexpr auto returnType = type_from(ltl::invoke(m_f, *ptr));
                if_constexpr(returnType == this->type) { //
                    res = false;
                    this->m_result = ltl::invoke(m_f, *ptr);
                }
                else_if_constexpr(returnType == optional_type) { //
                    if (auto result = ltl::invoke(m_f, *ptr)) {
                        res = false;
                        this->m_result = std::move(*result);
                    }
                }
                else_if_constexpr(returnType == ltl::type_v<bool>) { res = ltl::invoke(m_f, *ptr); }
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

  private:
    F m_f;
};

template <typename Result, typename F, typename... Ts>
auto make_one_result_visitor(ltl::type_list_t<Ts...>, F f) {
    return OneResultVisitor<Result, F, Ts...>{std::move(f)};
}

} // namespace phx
