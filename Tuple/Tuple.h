#pragma once

template<size_t index, typename Head>
struct HeadData;

template<size_t index, typename ...Tail>
struct TupleHelper;

template<typename ...Elements>
class Tuple;

template<size_t index, typename Head, typename ...Tail>
constexpr auto &GetHelper(TupleHelper<index, Head, Tail...> &t) {
    return TupleHelper<index, Head, Tail...>::get(t);
}

template<size_t index, typename Head, typename ...Tail>
constexpr auto &GetHelper(const TupleHelper<index, Head, Tail...> &t) {
    return TupleHelper<index, Head, Tail...>::get(t);
}

template<size_t index, typename Head, typename ...Tail>
constexpr auto &&GetHelper(TupleHelper<index, Head, Tail...> &&t) {
    return std::move(TupleHelper<index, Head, Tail...>::get(std::move(t)));
}

template<size_t index, typename Head, typename ...Tail>
constexpr auto &&GetHelper(const TupleHelper<index, Head, Tail...> &&t) {
    return std::move(TupleHelper<index, Head, Tail...>::get(std::move(t)));
}

template<size_t index, typename ...Elements>
constexpr auto &get(Tuple<Elements...> &t) {
    return GetHelper<index>(t);
}

template<size_t index, typename ...Elements>
constexpr auto &get(const Tuple<Elements...> &t) {
    return GetHelper<index>(t);
}

template<size_t index, typename ...Elements>
constexpr auto &&get(Tuple<Elements...> &&t) {
    return std::move(GetHelper<index>(std::move(t)));
}

template<size_t index, typename ...Elements>
constexpr auto &&get(const Tuple<Elements...> &&t) {
    return std::move(GetHelper<index>(std::move(t)));
}

template<typename T, size_t index, typename ...Tail>
constexpr auto &GetHelper(TupleHelper<index, T, Tail...> &t) {
    return TupleHelper<index, T, Tail...>::get(t);
}

template<typename T, size_t index, typename ...Tail>
constexpr auto &GetHelper(const TupleHelper<index, T, Tail...> &t) {
    return TupleHelper<index, T, Tail...>::get(t);
}

template<typename T, size_t index, typename ...Tail>
constexpr auto &&GetHelper(TupleHelper<index, T, Tail...> &&t) {
    return std::move(TupleHelper<index, T, Tail...>::get(std::move(t)));
}

template<typename T, size_t index, typename ...Tail>
constexpr auto &&GetHelper(const TupleHelper<index, T, Tail...> &&t) {
    return std::move(TupleHelper<index, T, Tail...>::get(std::move(t)));
}

template<typename T, typename ...Elements>
constexpr auto &get(Tuple<Elements...> &t) {
    return GetHelper<T>(t);
}

template<typename T, typename ...Elements>
constexpr auto &get(const Tuple<Elements...> &t) {
    return GetHelper<T>(t);
}

template<typename T, typename ...Elements>
constexpr auto &&get(Tuple<Elements...> &&t) {
    return std::move(GetHelper<T>(std::move(t)));
}

template<typename T, typename ...Elements>
constexpr auto &&get(const Tuple<Elements...> &&t) {
    return std::move(GetHelper<T>(std::move(t)));
}

template<size_t index, typename Head>
struct HeadData {
    Head value;

    constexpr HeadData() : value() {}
    ~HeadData() = default;

    template<typename T>
    constexpr HeadData(T &&arg) : value(std::forward<T>(arg)) {}

    constexpr HeadData(const HeadData &other) : value(other.value) {}

    constexpr HeadData(HeadData &&other) : value(std::move(other.value)) {}

    static constexpr auto &get(HeadData &d) {
        return d.value;
    }

    static constexpr auto &get(const HeadData &d) {
        return d.value;
    }

    static constexpr auto &&get(HeadData &&d) {
        return std::move(d.value);
    }

    static constexpr auto &&get(const HeadData &&d) {
        return std::move(d.value);
    }

};

template <size_t index, typename Head>
struct HeadData<index, Head&> {
    Head &value;

    constexpr HeadData() : value() {}
    ~HeadData() = default;

    template<typename T>
    constexpr HeadData(T &&arg) : value(arg) {}

    constexpr HeadData(const HeadData &other) : value(other.value) {}

    constexpr HeadData(HeadData &&other) : value(other.value) {}

    static constexpr auto &get(HeadData &d) {
        return d.value;
    }

    static constexpr auto &get(const HeadData &d) {
        return d.value;
    }

    static constexpr auto &&get(HeadData &&d) {
        return std::move(d.value);
    }

    static constexpr auto &&get(const HeadData &&d) {
        return std::move(d.value);
    }
};

template<size_t index, typename Head, typename ...Tail>
struct TupleHelper<index, Head, Tail...> : public TupleHelper<index + 1, Tail...>,
                                           private HeadData<index, Head> {
public:
    template<size_t, typename ...> friend
    class TupleHelper;

    constexpr TupleHelper<index, Head, Tail...>() : TupleHelper<index + 1, Tail...>(), HeadData<index, Head>() {}

    template<typename T, typename ...Args>
    constexpr TupleHelper<index, Head, Tail...>(T &&head, Args &&...tail)
            : TupleHelper<index + 1, Tail...>(std::forward<Args>(tail)...), HeadData<index, Head>(std::forward<T>(head)) {}

    template<typename ...Elements>
    constexpr TupleHelper<index, Head, Tail...>(Tuple<Elements...> &other)
            : TupleHelper<index + 1, Tail...>(other), HeadData<index, Head>(::get<index>(other)) {}

    template<typename ...Elements>
    constexpr TupleHelper<index, Head, Tail...>(const Tuple<Elements...> &other)
            : TupleHelper<index + 1, Tail...>(other), HeadData<index, Head>(::get<index>(other)) {}

    template<typename ...Elements>
    constexpr TupleHelper<index, Head, Tail...>(Tuple<Elements...> &&other)
            : TupleHelper<index + 1, Tail...>(std::move(other)), HeadData<index, Head>(std::move(::get<index>(std::move(other)))) {}

    template<typename ...Elements>
    constexpr TupleHelper<index, Head, Tail...>(const Tuple<Elements...> &&other)
            : TupleHelper<index + 1, Tail...>(std::move(other)), HeadData<index, Head>(std::move(::get<index>(std::move(other)))) {}

    ~TupleHelper<index, Head, Tail...>() = default;

    static constexpr auto &get(TupleHelper<index, Head, Tail...> &t) {
        return HeadData<index, Head>::get(t);
    }

    static constexpr auto &get(const TupleHelper<index, Head, Tail...> &t) {
        return HeadData<index, Head>::get(t);
    }

    static constexpr auto &&get(TupleHelper<index, Head, Tail...> &&t) {
        return std::move(HeadData<index, Head>::get(std::move(t)));
    }

    static constexpr auto &&get(const TupleHelper<index, Head, Tail...> &&t) {
        return std::move(HeadData<index, Head>::get(std::move(t)));
    }

protected:
    void assign(const TupleHelper<index, Head, Tail...> &t) {
        get(*this) = get(t);
        TupleHelper<index + 1, Tail...>::assign(t);
    }

    void assign(TupleHelper<index, Head, Tail...> &&t) {
        get(*this) = std::move(get(std::move(t)));
        TupleHelper<index + 1, Tail...>::assign(std::move(t));
    }

    void swap(TupleHelper<index, Head, Tail...> &t) {
        std::swap(get(*this), get(t));
        TupleHelper<index + 1, Tail...>::swap(t);
    }
};

template<size_t index>
struct TupleHelper<index> {
    constexpr TupleHelper<index>() {}

    template<typename ...Elements>
    constexpr TupleHelper<index>(Tuple<Elements...> &) {}

    template<typename ...Elements>
    constexpr TupleHelper<index>(const Tuple<Elements...> &) {}

    template<typename ...Elements>
    constexpr TupleHelper<index>(Tuple<Elements...> &&) {}

    template<typename ...Elements>
    constexpr TupleHelper<index>(const Tuple<Elements...> &&) {}

    ~TupleHelper<index>() = default;

protected:
    void assign(const TupleHelper<index> &) {}
    void assign(TupleHelper<index> &&) {}
    void swap(TupleHelper<index> &) {}
};

template<typename ...Elements>
class Tuple : public TupleHelper<0, Elements...> {
public:
    template<typename ...Args>
    constexpr Tuple(Args &&...args) : TupleHelper<0, Elements...>(std::forward<Args>(args)...) {}
    constexpr Tuple() : TupleHelper<0, Elements...>() {}
    constexpr Tuple(Tuple &other) : TupleHelper<0, Elements...>(other) {}
    constexpr Tuple(const Tuple &other) : TupleHelper<0, Elements...>(other) {}
    constexpr Tuple(Tuple &&other) : TupleHelper<0, Elements...>(std::move(other)) {}
    constexpr Tuple(const Tuple &&other) : TupleHelper<0, Elements...>(std::move(other)) {}
    template<typename ...Types>
    constexpr Tuple(Tuple<Types...> &other) : TupleHelper<0, Elements...>(other) {}
    template<typename ...Types>
    constexpr Tuple(const Tuple<Types...> &other) : TupleHelper<0, Elements...>(other) {}
    template<typename ...Types>
    constexpr Tuple(Tuple<Types...> &&other) : TupleHelper<0, Elements...>(std::move(other)) {}
    template<typename ...Types>
    constexpr Tuple(const Tuple<Types...> &&other) : TupleHelper<0, Elements...>(std::move(other)) {}
    ~Tuple() = default;

    Tuple &operator=(const Tuple &other) {
        TupleHelper<0, Elements...>::assign(other);
        return *this;
    }

    Tuple &operator=(Tuple &&other) {
        TupleHelper<0, Elements...>::assign(std::move(other));
        return *this;
    }

    void swap(Tuple<Elements...> &other) {
        TupleHelper<0, Elements...>::swap(other);
    }

};

template<>
class Tuple<> {};



template<typename ...Args>
constexpr auto makeTuple(Args &&...args) {
    return Tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
}

template<typename ...Args>
constexpr size_t TupleSize(const Tuple<Args...> &t) {
    return sizeof...(Args);
}

template<typename ...Args, typename ...Tuples>
constexpr size_t FirstTupleSize(const Tuple<Args...> &, const Tuples &...) {
    return sizeof...(Args);
}

constexpr size_t FirstTupleSize() {
    return 0;
}

template<size_t ...Indexes>
struct IndexesContainer {};

template<typename T, size_t ...Indexes>
struct IndexesHelper {
    using type = IndexesHelper<void, Indexes...>;
    using container = IndexesContainer<Indexes...>;
};

template<size_t ...Indexes, size_t ...Concatenate>
struct IndexesHelper<IndexesHelper<void, Indexes...>, Concatenate...> {
    using type = IndexesHelper<void, Indexes..., Concatenate...>;
    using container = IndexesContainer<Indexes..., Concatenate...>;
};

template<size_t N>
struct CreateIndexesContainer {
    using type = typename IndexesHelper<typename CreateIndexesContainer<N - 1>::type, N>::type;
    using container = typename IndexesHelper<typename CreateIndexesContainer<N - 1>::type, N>::container;
};

template<>
struct CreateIndexesContainer<0> {
    using type = IndexesHelper<void, 0>::type;
    using container = IndexesHelper<void, 0>::container;
};

template<>
struct CreateIndexesContainer<SIZE_MAX> {
    using type = IndexesHelper<void>::type;
    using container = IndexesHelper<void>::container;
};

template<typename TupleType, typename Indexes, typename ...Tuples>
struct concatenate {};

template<typename TupleType, size_t... Indexes, typename Tuple, typename ...Tuples>
struct concatenate<TupleType, IndexesContainer<Indexes...>, Tuple, Tuples...> {
    template<typename ...Args>
    static constexpr auto get(Tuple &&tuple, Tuples &&...tuples, Args &&...args) {
        using container = typename CreateIndexesContainer<FirstTupleSize(tuples...) - 1>::container;
        return concatenate<TupleType, container, Tuples...>::get(std::forward<Tuples>(tuples)...,
                                                                 std::forward<Args>(args)...,
                                                                 ::get<Indexes>(std::forward<Tuple>(tuple))...);
    }
};

template<typename TupleType>
struct concatenate<TupleType, IndexesContainer<> > {
    template<typename ...Args>
    static constexpr auto get(Args &&...args) {
        return TupleType(std::forward<Args>(args)...);
    }
};

template<typename ...Tuples>
struct ConcatenateType {
    using type = Tuple<>;
};

template<typename ...First, typename ...Second, typename ...Tuples>
struct ConcatenateType<Tuple<First...>, Tuple<Second...>, Tuples...> {
    using type = typename ConcatenateType<Tuple<First..., Second...>, Tuples...>::type;
};

template<typename ...Elements>
struct ConcatenateType<Tuple<Elements...> > {
    using type = Tuple<Elements...>;
};

template<typename ...Tuples>
constexpr auto tupleCat(Tuples &&...tuples) {
    using TupleType = typename ConcatenateType<std::remove_const_t<std::remove_reference_t<Tuples> >...>::type;
    using container = typename CreateIndexesContainer<FirstTupleSize(tuples...) - 1>::container;
    return concatenate<TupleType, container, Tuples...>::get(std::forward<Tuples>(tuples)...);
}

template<size_t Index, size_t Size, typename FirstTuple, typename SecondTuple>
struct CompareTuple {
    static constexpr bool equal(const FirstTuple &a, const SecondTuple &b) {
        return get<Index>(a) == get<Index>(b) && CompareTuple<Index + 1, Size, FirstTuple, SecondTuple>::equal(a, b);
    }
    static constexpr bool less(const FirstTuple &a, const SecondTuple &b) {
        return get<Index>(a) < get<Index>(b) ||
               (!(get<Index>(b) < get<Index>(a)) && CompareTuple<Index + 1, Size, FirstTuple, SecondTuple>::less(a, b));
    }
};

template<size_t Size, typename FirstTuple, typename SecondTuple>
struct CompareTuple<Size, Size, FirstTuple, SecondTuple> {
    static constexpr bool equal(const FirstTuple &, const SecondTuple &) {
        return true;
    }
    static constexpr bool less(const FirstTuple &, const SecondTuple &) {
        return false;
    }
};

template<typename ...FirstElements, typename ...SecondElements>
constexpr bool operator==(const Tuple<FirstElements...> &a, const Tuple<SecondElements...> &b) {
    return CompareTuple<0, sizeof...(FirstElements), Tuple<FirstElements...>, Tuple<SecondElements...> >::equal(a, b);
}

template<typename ...FirstElements, typename ...SecondElements>
constexpr bool operator!=(const Tuple<FirstElements...> &a, const Tuple<SecondElements...> &b) {
    return !(a == b);
}

template<typename ...FirstElements, typename ...SecondElements>
constexpr bool operator<(const Tuple<FirstElements...> &a, const Tuple<SecondElements...> &b) {
    return CompareTuple<0, sizeof...(FirstElements), Tuple<FirstElements...>, Tuple<SecondElements...> >::less(a, b);
}

template<typename ...FirstElements, typename ...SecondElements>
constexpr bool operator>(const Tuple<FirstElements...> &a, const Tuple<SecondElements...> &b) {
    return b < a;
}

template<typename ...FirstElements, typename ...SecondElements>
constexpr bool operator<=(const Tuple<FirstElements...> &a, const Tuple<SecondElements...> &b) {
    return !(b < a);
}

template<typename ...FirstElements, typename ...SecondElements>
constexpr bool operator>=(const Tuple<FirstElements...> &a, const Tuple<SecondElements...> &b) {
    return !(a < b);
}