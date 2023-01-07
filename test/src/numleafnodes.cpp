#include <boost/ut.hpp>
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
    inline constexpr static T defaultValue = T(V);

    int guard(){
      return NO_ERROR;
    }
};


using SimpleTree = Node<
                    NodeHeader<
                        0,
                        Leafnode<LeafnodeHeader<0,5,int>>,
                        Leafnode<LeafnodeHeader<1,5.5,double>>,
                        Leafnode<LeafnodeHeader<2,-4.5,float>>,
                        Leafnode<LeafnodeHeader<3,std::array<char,255>{"hello"},std::array<char,255>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeader<
                            0,
                            SimpleTree,
                            Leafnode<LeafnodeHeader<1,2.5,double>>
                          >
                        >;

int main() {
  
    using namespace boost::ut;
   
    "getIDs_simple_tree"_test = [&] {
    
        // Test setupt
        SimpleTree t_simple;
        expect(t_simple.getNumLeafnodes() == 4_i);
    };

   "getIDs_asymetric_tree"_test = [&] {
    
        // Test setupt
        AsymetricTree t_asym;
        expect(t_asym.getNumLeafnodes() == 5_i);                        

   };


}