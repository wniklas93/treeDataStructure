#include <boost/ut.hpp>

#include "tree/nodes.hpp"

template<uint8_t I,NodeLike... N>
struct NodeHeaderImpl{
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

struct GetIDsOperation{
    public:
        
        template<NodeConcept N>
        static bool visitNode(N* n){
            value = n->getChildrenIDs();
            
            return false;
        }

        template<NodeLike N>
        static bool previsit(N* n){
          return true;
        }        
        
        inline static std::span<const uint8_t> value;
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

        // Test if error notification is correct
        uint8_t ID0 = 0; uint8_t ID1 = 1; uint8_t ID2 = 2;        
        expect(t_simple.accept<GetIDsOperation>()              == 0_i);                   // Must return no error (0), as node does exist
        expect(t_simple.traverse<GetIDsOperation>(0)           == 1_i);                   // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(1)           == 1_i);                   // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(2)           == 1_i);                   // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(3)           == 1_i);                   // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(4)           == 1_i);                   // Must return an error (1), as node does not exist
        expect(t_simple.traverse<GetIDsOperation>(ID0,ID1,ID2) == 1_i);                   // Must return an error (1), as node does not exist

        // Test if return value is correct
        for(int i = 0; i < 4; i++) expect(i == GetIDsOperation::value[i]);      // Must return no error, as IDs do exist
    };

   "getIDs_asymetric_tree"_test = [&] {
    
        // Test setupt
        AsymetricTree t_asym;

        expect(t_asym.accept<GetIDsOperation>() == 0_i);                        
        // Must return no error, as node does exist
        for(int i = 0; i < 2; i++) expect(i == GetIDsOperation::value[i]);      // Must return no error, as IDs do exist

        expect(t_asym.traverse<GetIDsOperation>(0) == 0_i);                     // Must return no error, as node does exist
        for(int i = 0; i < 4; i++) expect(i == GetIDsOperation::value[i]);      // Must return no error, as IDs do exist

        expect(t_asym.traverse<GetIDsOperation>(2)     == 1_i);
        expect(t_asym.traverse<GetIDsOperation>(1)     == 1_i);                 // Must return an error (0), as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,0)   == 1_i);                 // Must return an error (0), as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,1)   == 1_i);                 // Must return an error (0), as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,2)   == 1_i);                 // Must return an error (0), as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,3)   == 1_i);                 // Must return an error (0), as leafnodes haven't got any child
   };


}