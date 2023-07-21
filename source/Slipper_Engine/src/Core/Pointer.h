#pragma once

template<typename T> class OwningPtr;
template<typename T> class SharedPtr;

template<typename To, typename From>
concept ConvertiblePtr = std::is_convertible_v<To *, From *>;

template<typename From, typename To>
concept ConvertibleFromPtr = std::is_convertible_v<To *, From *>;

template<typename T> struct NonOwningPtr
{
    friend std::hash<NonOwningPtr<T>>;

 public:
    constexpr NonOwningPtr() : m_ptr(nullptr)
    {
    }

    constexpr NonOwningPtr(std::nullptr_t) : m_ptr(nullptr)
    {
    }

    template<ConvertiblePtr<T> U> constexpr NonOwningPtr(U *Ptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U>
    constexpr NonOwningPtr(const NonOwningPtr<U> &Other) : m_ptr(Other.get())
    {
    }

    template<ConvertiblePtr<T> U>
    constexpr NonOwningPtr(const OwningPtr<U> &OwningPtr) : m_ptr(OwningPtr.get())
    {
    }

    constexpr T &operator*() const
    {
        return *m_ptr;
    }

    constexpr T *operator->() const
    {
        return m_ptr;
    }

    constexpr T *get() const
    {
        return m_ptr;
    }

    constexpr operator T *() const
    {
        return m_ptr;
    }

    template<ConvertiblePtr<T> U> constexpr NonOwningPtr<T> &operator=(U *Ptr)
    {
        m_ptr = Ptr;
        return *this;
    }

    template<ConvertiblePtr<T> U> constexpr NonOwningPtr<T> &operator=(NonOwningPtr<U> &Other)
    {
        m_ptr = Other.m_ptr;
        return *this;
    }

    constexpr explicit operator bool() const noexcept
    {
        return IsValid();
    }

    constexpr bool IsValid() const noexcept
    {
        return bool(m_ptr);
    }

    template<ConvertibleFromPtr<T> U> constexpr NonOwningPtr<U> Cast() const
    {
        return NonOwningPtr<U>(m_ptr);
    }

    template<typename U> constexpr NonOwningPtr<U> TryCast() const
    {
        return dynamic_cast<U *>(m_ptr);
    }

 private:
    T *m_ptr;
};

namespace std
{
template<typename T> struct hash<NonOwningPtr<T>>
{
    size_t operator()(NonOwningPtr<T> const &Ptr) const noexcept
    {
        return std::hash<std::uintptr_t>()(reinterpret_cast<std::uintptr_t>(Ptr.m_ptr));
    }
};
}  // namespace std

template<typename T> struct OwningPtr
{
    friend std::hash<OwningPtr<T>>;

 public:
    constexpr OwningPtr() : m_ptr(nullptr)
    {
    }

    template<ConvertiblePtr<T> U> constexpr OwningPtr(U *Ptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U>
    constexpr explicit OwningPtr(const NonOwningPtr<U> &Ptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U> constexpr explicit OwningPtr(const SharedPtr<T> &Ptr) = delete;
    template<ConvertiblePtr<T> U> constexpr explicit OwningPtr(const OwningPtr<T> &Ptr) = delete;

    template<ConvertiblePtr<T> U>
    constexpr OwningPtr(OwningPtr<U> &&Other) noexcept : m_ptr(std::move(Other.m_ptr))
    {
    }

    OwningPtr(OwningPtr<T> &Other) = delete;

    constexpr void reset(T *Ptr = nullptr)
    {
        m_ptr.reset(Ptr);
    }

    constexpr T &operator*() const
    {
        return *m_ptr;
    }

    constexpr T *operator->() const
    {
        return m_ptr.get();
    }

    constexpr OwningPtr<T> &operator=(std::nullptr_t)
    {
        m_ptr.reset();
        return *this;
    }

    template<ConvertiblePtr<T> U> constexpr OwningPtr<T> &operator=(U *Ptr)
    {
        m_ptr.reset(Ptr);
        return *this;
    }

    // Returns the underlying ptr
    constexpr T *get() const
    {
        return m_ptr.get();
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return IsValid();
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return bool(m_ptr);
    }

 private:
    std::unique_ptr<T> m_ptr;
};

namespace std
{
template<typename T> struct hash<OwningPtr<T>>
{
    size_t operator()(OwningPtr<T> const &Ptr) const noexcept
    {
        return std::hash<std::unique_ptr<T>>(Ptr.m_ptr);
    }
};
}  // namespace std

template<typename T> struct SharedPtr
{
    friend std::hash<SharedPtr<T>>;

 public:
    SharedPtr() : m_ptr(nullptr)
    {
    }

    template<ConvertiblePtr<T> U> SharedPtr(U *Ptr = nullptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U> SharedPtr(const SharedPtr<U> &Other)
    {
        m_ptr = Other.m_ptr;
    }

    template<ConvertiblePtr<T> U>
    SharedPtr(SharedPtr<U> &&Other) noexcept : m_ptr(std::move(Other.m_ptr))
    {
    }

    template<ConvertiblePtr<T> U> SharedPtr<T> &operator=(const SharedPtr<U> &Other)
    {
        m_ptr = Other.m_ptr;
        return *this;
    }

    template<ConvertiblePtr<T> U> SharedPtr<T> &operator=(SharedPtr<U> &&Other) noexcept
    {
        m_ptr = Other.m_ptr;
        return *this;
    }

    T &operator*() const
    {
        return *m_ptr;
    }

    T *operator->() const
    {
        return m_ptr.get();
    }

    T *get() const
    {
        return m_ptr.get();
    }

    operator T *() const
    {
        return m_ptr.get();
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return IsValid();
    }

    [[nodiscard]] bool IsValid() const noexcept
    {
        return bool(m_ptr);
    }

 private:
    std::shared_ptr<T> m_ptr;
};

namespace std
{
template<typename T> struct hash<SharedPtr<T>>
{
    size_t operator()(SharedPtr<T> const &Ptr) const noexcept
    {
        return std::hash<std::shared_ptr<T>>(Ptr.m_ptr);
    }
};
}  // namespace std

template<typename T> using Ref = std::reference_wrapper<T>;

// template<typename From, typename To>
// concept ConvertibleRef = std::is_convertible_v<From, To &>;
//
// template<typename T> struct Ref
//{
//     friend std::hash<NonOwningPtr<T>>;
//
//  public:
//     // This should cover all use cases since it does not allow construction using wrappers
//     template<typename U>
//         requires(!std::is_same_v<Ref, std::remove_cvref_t<U>>) && ConvertibleRef<U, T>
//     constexpr Ref(U &&Object) : m_ref(std::addressof(std::forward<U>(Object)))
//     {
//     }
//
//     // Access
//     constexpr operator T &() const noexcept
//     {
//         return *m_ref;
//     }
//
//     constexpr T &get() noexcept
//     {
//         return *m_ref;
//     }
//
//     constexpr T &get() const noexcept
//     {
//         return *m_ref;
//     }
//
//     // Not quite like real refs but at least we know it has to hold a valid value
//     constexpr T *operator->() const
//     {
//         return m_ref;
//     }
//
//     template<class... ArgTypes>
//     constexpr std::invoke_result_t<T &, ArgTypes...> operator()(ArgTypes &&...args) const
//         noexcept(std::is_nothrow_invocable_v<T &, ArgTypes...>)
//     {
//         return std::invoke(get(), std::forward<ArgTypes>(args)...);
//     }
//
//  private:
//     T *m_ref;
// };