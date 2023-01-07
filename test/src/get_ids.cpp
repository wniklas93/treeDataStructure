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

struct GetIDsOperation{
    public:
        
        template<NodeConcept N>
        static int visitNode(N* n){
            value = n->getChildrenIDs();
            
            return NO_ERROR;
        }

        template<NodeLike N>
        static int previsit(N* n){
          return NO_ERROR;
        }        
        
        inline static std::span<const uint16_t> value;
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

        // Test if error notification is correct
        uint8_t ID0 = 0; uint8_t ID1 = 1; uint8_t ID2 = 2;        
        expect(t_simple.accept<GetIDsOperation>()              == NO_ERROR);                   // Must return no error, as node does exist
        expect(t_simple.traverse<GetIDsOperation>(0)           == VISITOR_NOT_ACCEPTED);       // Visitor not accepted, as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(1)           == VISITOR_NOT_ACCEPTED);       // Visitor not accepted, as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(2)           == VISITOR_NOT_ACCEPTED);       // Visitor not accepted, as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(3)           == VISITOR_NOT_ACCEPTED);       // Visitor not accepted, as leafnodes haven't got any childs
        expect(t_simple.traverse<GetIDsOperation>(4)           == ID_NOT_FOUND);               // Visitor not accepted, as node does not exist
        expect(t_simple.traverse<GetIDsOperation>(ID0,ID1,ID2) == ID_NOT_FOUND);               // Visitor not accepted, as node does not exist

        // Test if return value is correct
        for(int i = 0; i < 4; i++) expect(i == GetIDsOperation::value[i]);                     // Must return no error, as IDs do exist
    };

   "getIDs_asymetric_tree"_test = [&] {
    
        // Test setupt
        AsymetricTree t_asym;

        expect(t_asym.accept<GetIDsOperation>() == NO_ERROR);                        
        // Must return no error, as node does exist
        for(int i = 0; i < 2; i++) expect(i == GetIDsOperation::value[i]);         // Must return no error, as IDs do exist

        expect(t_asym.traverse<GetIDsOperation>(0) == NO_ERROR);                   // Must return no error, as node does exist
        for(int i = 0; i < 4; i++) expect(i == GetIDsOperation::value[i]);         // Must return no error, as IDs do exist

        expect(t_asym.traverse<GetIDsOperation>(2)     == ID_NOT_FOUND);           // Must return an error, as node does not exist
        expect(t_asym.traverse<GetIDsOperation>(1)     == VISITOR_NOT_ACCEPTED);   // Must return an error, as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,0)   == VISITOR_NOT_ACCEPTED);   // Must return an error, as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,1)   == VISITOR_NOT_ACCEPTED);   // Must return an error, as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,2)   == VISITOR_NOT_ACCEPTED);   // Must return an error, as leafnodes haven't got any child
        expect(t_asym.traverse<GetIDsOperation>(0,3)   == VISITOR_NOT_ACCEPTED);   // Must return an error, as leafnodes haven't got any child
   };


}