#ifndef NODES_H
#define NODES_H

#include <cstdint>
#include <tuple>
#include <initializer_list>
#include <concepts>
#include <string>
#include <type_traits>
#include <array>
#include <utility>

#include <iostream>

using namespace std;

template<class T>
concept NodeHeader = requires (){
    {T::ID} -> convertible_to<uint8_t>;
    T::guard;
};

template<class T>
concept LeafHeader = requires () {
    {T::ID} -> convertible_to<uint8_t>;
    T::guard;
    typename T::type;
    T::defaultValue;
};

template<class T>
concept NodeLike = is_same_v<T, bool> ||
                   is_same_v<T, string> ||
                   is_same_v<T, uint8_t> ||
                   is_same_v<T, uint16_t> ||
                   requires (T t) {         
                       t.childs;
                       typename T::Header;
                    } || 
                   requires(T t) {
                       t.data;
                       typename T::Header;
                   };

template<LeafHeader H>
struct LeafNode{
    using Header = H;
    H::type data = H::defaultValue;
};


template<NodeHeader H, NodeLike... N>
struct Node{
    using Header = H;
    tuple<N...> childs{};

    template<uint8_t ID>
    struct id2idx {
        static constexpr uint8_t getIndex();
    };

    template<class T, class... Args>
    bool read(auto& result, const uint8_t& ID, const Args&... residualIDs);

    //*************** Type filter for leafnodes ***************\\
    // Tuple assembler
    template <typename, typename> struct Cons{};

    template <typename  T, typename ...Args>
    struct Cons<T, tuple<Args...>>
    {
        using type = tuple<T, Args...>;
    };


    // Type filter filtering non matching types and returning tuple with
    // matching
    template <typename DataType, typename...> struct filter{};

    template <typename DataType> struct filter<DataType> { using type = tuple<>; };

    template <typename DataType,
              typename First,
              typename ...Tail>
    struct filter<DataType,First,Tail...>
    {
        using type = typename std::conditional<
                                    is_same_v<DataType, typename First::Header::type>,
                                    typename Cons<First, typename filter<DataType,Tail...>::type>::type,
                                    typename filter<DataType,Tail...>::type
                              >::type;
    }; 
};

// Include template implementation
#include "nodes.tpp"

#endif