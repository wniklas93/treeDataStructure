#include <string>
#include <chrono>
#include <functional>

#include "tree/nodes.hpp"

using namespace Tree;

template<uint16_t I,NodeLike... N>
struct NodeHeader{
    static constexpr uint16_t ID = I;

    using childrenTypes = std::tuple<N...>;

    int guard(){
      return NO_ERROR;
    }

};

template<uint16_t I, auto V, class T>
struct LeafnodeHeader{
    static constexpr uint16_t ID = I;
    using type = T;
    inline const static T defaultValue = T(V);

    int guard(){
      return NO_ERROR;
    }
};

using SimpleTree = Node<
                    NodeHeader<
                        0,
                        Leafnode<LeafnodeHeader<0,5,int>>,
                        Leafnode<LeafnodeHeader<1,5.5,double>>,
                        Leafnode<LeafnodeHeader<2,-4.5,float>>
                        // Leafnode<LeafnodeHeader<3,std::array<char,255>{"hello"},std::array<char,255>>>,
                        // Leafnode<LeafnodeHeader<4,5,std::chrono::seconds>>
                      >
                  >;

// using AsymetricTree = Node<
//                         NodeHeader<
//                             0,
//                             SimpleTree,
//                             Leafnode<LeafnodeHeader<1,2.5,double>>,
//                             Leafnode<LeafnodeHeader<2,nullptr,int(*)(int,int)>>,
//                             Leafnode<LeafnodeHeader<3,nullptr,std::function<int()>>>
//                           >
//                         >;

int main() {

    SimpleTree t_simple;
//    AsymetricTree t_asym;

//    t_simple.traverse<DecodeOperation>(0);
    return 0;
}