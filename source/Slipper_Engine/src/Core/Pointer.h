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
    NonOwningPtr() : m_ptr(nullptr)
    {
    }

    NonOwningPtr(std::nullptr_t) : m_ptr(nullptr)
    {
    }

    template<ConvertiblePtr<T> U> NonOwningPtr(U *Ptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U> NonOwningPtr(const NonOwningPtr<U> &Other) : m_ptr(Other.get())
    {
    }

    template<ConvertiblePtr<T> U>
    NonOwningPtr(const OwningPtr<U> &OwningPtr) : m_ptr(OwningPtr.get())
    {
    }

    T &operator*() const
    {
        return *m_ptr;
    }

    T *operator->() const
    {
        return m_ptr;
    }

    T *get() const
    {
        return m_ptr;
    }

    operator T *() const
    {
        return m_ptr;
    }

    template<ConvertiblePtr<T> U> NonOwningPtr<T> &operator=(U *Ptr)
    {
        m_ptr = Ptr;
        return *this;
    }

    template<ConvertiblePtr<T> U> NonOwningPtr<T> &operator=(NonOwningPtr<U> &Other)
    {
        m_ptr = Other.m_ptr;
        return *this;
    }

    explicit operator bool() const noexcept
    {
        return IsValid();
    }

    bool IsValid() const noexcept
    {
        return bool(m_ptr);
    }

    template<ConvertibleFromPtr<T> U>
    NonOwningPtr<U> Cast()
    {
        return NonOwningPtr<U>(m_ptr);
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
    OwningPtr() : m_ptr(nullptr)
    {
    }

    template<ConvertiblePtr<T> U> OwningPtr(U *Ptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U> explicit OwningPtr(const NonOwningPtr<U> &Ptr) : m_ptr(Ptr)
    {
    }

    template<ConvertiblePtr<T> U> explicit OwningPtr(const SharedPtr<T> &Ptr) = delete;
    template<ConvertiblePtr<T> U> explicit OwningPtr(const OwningPtr<T> &Ptr) = delete;

    template<ConvertiblePtr<T> U>
    OwningPtr(OwningPtr<U> &&Other) noexcept : m_ptr(std::move(Other.m_ptr))
    {
    }

    OwningPtr(OwningPtr<T> &Other) = delete;

    void reset(T *Ptr = nullptr)
    {
        m_ptr.reset(Ptr);
    }

    T &operator*() const
    {
        return *m_ptr;
    }

    T *operator->() const
    {
        return m_ptr.get();
    }

    OwningPtr<T> &operator=(std::nullptr_t)
    {
        m_ptr.reset();
        return *this;
    }

    template<ConvertiblePtr<T> U> OwningPtr<T> &operator=(U *Ptr)
    {
        m_ptr.reset(Ptr);
        return *this;
    }

    OwningPtr<T> &operator=(const OwningPtr<T> &Other) = delete;

    // Returns the underlying ptr
    T *get() const
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

    template<ConvertiblePtr<T> U>
    SharedPtr<T> &operator=(SharedPtr<U> &&Other) noexcept
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

    operator T* () const
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