#include <boost/ut.hpp>
#include "tree/nodes.hpp"

template<uint8_t I,NodeLike... N>
struct NodeHeaderImpl{
    static constexpr uint8_t ID = I;

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


using SimpleTree = Node<
                    NodeHeaderImpl<
                        0,
                        Leafnode<LeafnodeHeaderImpl<0,5,int>>,
                        Leafnode<LeafnodeHeaderImpl<1,5.5,double>>,
                        Leafnode<LeafnodeHeaderImpl<2,-4.5,float>>,
                        Leafnode<LeafnodeHeaderImpl<3,std::array<char,255>{"hello"},std::array<char,255>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeaderImpl<
                            0,
                            SimpleTree,
                            Leafnode<LeafnodeHeaderImpl<1,2.5,double>>
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