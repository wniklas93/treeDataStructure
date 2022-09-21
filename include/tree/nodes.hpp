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
concept LeafNodeConcept = requires (T t) {
                            t.data;
                            typename T::Header;
};

template<class T>
concept NodeConcept = requires (T t){
                            t.childs;
                            typename T::Header;
};

template<class T>
concept NodeLike = LeafNodeConcept<T> || NodeConcept<T>;

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
    
    template<class... Ts>
    struct overloaded : Ts... {
         using Ts::operator()...;
     };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>; 
    
};

// Include template implementation
#include "nodes.tpp"

#endif