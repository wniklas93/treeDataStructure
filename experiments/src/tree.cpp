#include <string>
#include <chrono>
#include <functional>

#include "tree/nodes.hpp"


template<uint16_t I,NodeLike... N>
struct NodeHeaderImpl{
    static constexpr uint16_t ID = I;

    using childrenTypes = std::tuple<N...>;

    bool guard(const uint16_t& queryID){
      return queryID == ID ? true : false;
    }

};

template<uint16_t I, auto V, class T>
struct LeafnodeHeaderImpl{
    static constexpr uint16_t ID = I;
    using type = T;
    inline const static T defaultValue = T(V);

    bool guard(const uint16_t& queryID){
      return queryID == ID ? true : false;
    }
};

using SimpleTree = Node<
                    NodeHeaderImpl<
                        0,
                        Leafnode<LeafnodeHeaderImpl<0,5,int>>,
                        Leafnode<LeafnodeHeaderImpl<1,5.5,double>>,
                        Leafnode<LeafnodeHeaderImpl<2,-4.5,float>>,
                        Leafnode<LeafnodeHeaderImpl<3,std::array<char,255>{"hello"},std::array<char,255>>>,
                        Leafnode<LeafnodeHeaderImpl<4,5,std::chrono::seconds>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeaderImpl<
                            0,
                            SimpleTree,
                            Leafnode<LeafnodeHeaderImpl<1,2.5,double>>,
                            Leafnode<LeafnodeHeaderImpl<2,nullptr,int(*)(int,int)>>,
                            Leafnode<LeafnodeHeaderImpl<3,nullptr,std::function<int()>>>
                          >
                        >;

int main() {

    SimpleTree t_simple;
    AsymetricTree t_asym;

    //t_simple.traverse<DecodeOperation>(0);
  
}