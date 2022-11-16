#include <boost/ut.hpp>
#include "tree/nodes.hpp"

// Todo: Use inheritance to avoid redundant code
// Define header implementations
template<uint8_t I, bool B, NodeLike... N>
struct NodeHeader{
    static constexpr uint8_t ID = I;

    using childrenTypes = std::tuple<N...>;

    bool guard(const uint8_t& queryID){
      return queryID == ID ? true : false;
    }

};

template<uint8_t I, NodeLike... N>
struct AnotherNodeHeader{
    static constexpr uint8_t ID = I;

    using childrenTypes = std::tuple<N...>;

    bool guard(const uint8_t& queryID){
        return queryID == ID ? true : false;
    }
};

template<uint8_t I, auto V, class T>
struct LeafnodeHeaderImpl{
    static constexpr uint8_t ID = I;
    
    bool guard(const uint8_t& queryID){
      return queryID == ID ? true : false;
    }

};

// Create tree type
template<class... N> // why no NodeLike?
using h0 = NodeHeader<0,true,N...>;
template<uint8_t I>
using l0 = Leafnode<LeafnodeHeaderImpl<I,3,int>>;
using n0 = nodeFactory<h0,l0,2>::type;

template<class... N>
using h1 = AnotherNodeHeader<1,N...>;
template<uint8_t I>
using l1 = Leafnode<LeafnodeHeaderImpl<I,4.3,double>>;
using n1 = nodeFactory<h1,l1,3>::type;

using tree0 = Node<NodeHeader<0,true,n0,n1>>;

using tree1 = Node<NodeHeader<0,
                              true,
                              Node<h0<l0<0>,l0<1>>>,
                              Node<h1<l1<0>,l1<1>,l1<2>>>
                            >
                    >;


int main() {

    using namespace boost::ut;

    expect(type<tree0> == type<tree1>);
    return 0;
}