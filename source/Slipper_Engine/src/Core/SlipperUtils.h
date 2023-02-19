#pragma once

#define IMPLEMENT_GETTER_SETTER(FnVariableName, ReturnType, Variable) \
    [[nodiscard]] ReturnType Get##FnVariableName() const              \
    {                                                                 \
        return Varible;                                               \
    }                                                                 \
                                                                      \
    void Set##FnVariableName(ReturnType Value)                        \
    {                                                                 \
        Variable = Value;                                             \
    }                                                                 \
    \