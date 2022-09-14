#include "tree/nodes.hpp"
#include <iostream>

// Specify Headers

struct RootHeader{
    static constexpr uint8_t ID = 0;

    static constexpr auto guard = [](const uint8_t queryID){
        return true;
    };
};

template<uint8_t I>
struct NodeHeaderImpl{
    static constexpr uint8_t ID = I;

    static constexpr auto guard = [](const uint8_t& queryID){
        return queryID == ID ? true : false; 
    };
};

template<uint8_t I, auto V, class T>
struct LeafHeaderImpl{
    static constexpr uint8_t ID = I;
    using type = T;
    static constexpr type defaultValue = V;
    static constexpr auto guard = [](const uint8_t& queryID){
        return queryID == ID ? true : false; 
    };
};

using BOOTSTRAP_SERVER = Node<
                           NodeHeaderImpl<0>,
                           LeafNode<LeafHeaderImpl<0,array<char,255>{"hello"},array<char,255>>>
                           >;

using SERVER_URI = Node<
                           NodeHeaderImpl<1>,
                           LeafNode<LeafHeaderImpl<0,true,bool>>
                           >;
using Tree = Node<
                   RootHeader,
                   SERVER_URI,
                   BOOTSTRAP_SERVER
               >;

int main(){
    Tree t;

    uint8_t id = 0;
    array<char,255> result;

    std::cout << t.read<array<char,255>>(result,id,id) << endl;
    std::cout << result[2] << endl;
    std::cout << "Hello world" << std::endl;
}