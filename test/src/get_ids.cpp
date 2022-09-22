#include <boost/ut.hpp>

#include "tree/nodes.hpp"
#include "tree/nodeFactory.hpp"

#include <string>

// Definition of headers according to tree api
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

using SimpleTree = Node<
                      NodeHeaderImpl<0>,
                      LeafNode<LeafHeaderImpl<0,5,int>>,
                      LeafNode<LeafHeaderImpl<1,5.5,double>>,
                      LeafNode<LeafHeaderImpl<2,-4.5,float>>,
                      LeafNode<LeafHeaderImpl<3,array<char,255>{"hello"},array<char,255>>>
                      >;

using AsymetricTree = Node<
                        RootHeader,
                        SimpleTree,
                        LeafNode<LeafHeaderImpl<1,2.5,double>>
                        >;

int main() {
  
    using namespace boost::ut;
   
    "getIDs_simple_tree"_test = [&] {
    
        // Test setupt
        SimpleTree t_simple;

        // Test child size
        std::size_t size;
        expect(t_simple.getChildNum(size) == 0_i);      // Must return no error (0), as node does exist
        expect(t_simple.getChildNum(size,0) == 1_i);    // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getChildNum(size,1) == 1_i);    // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getChildNum(size,2) == 1_i);    // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getChildNum(size,3) == 1_i);    // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getChildNum(size,4) == 1_i);    // Must return an error (1), as node does exist


        // Test if error notification is correct
        t_simple.getChildNum(size);
        std::cout << size << std::endl;
        uint8_t IDs[size];
        expect(t_simple.getIDs(IDs) == 0_i);        // Must return no error (0), as node does exist
        expect(t_simple.getIDs(IDs,0) == 1_i);      // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getIDs(IDs,1) == 1_i);      // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getIDs(IDs,2) == 1_i);      // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getIDs(IDs,3) == 1_i);      // Must return an error (1), as leafnodes haven't got any childs
        expect(t_simple.getIDs(IDs,4) == 1_i);      // Must return an error (1), as node does exist

        // Test if return value is correct
        t_simple.getIDs(IDs);
        //expect(IDs == array<uint8_t, 4>{0,1,2,3});

    };

    "getIDs_asymetric_tree"_test = [&] {
    
        // Test setupt
        AsymetricTree t_asym;
        array<uint8_t,2> IDs_firstLayer{};
        array<uint8_t,4> IDs_secondLayer{};

        // Test if error notification is correct
        expect(t_asym.getIDs(IDs_firstLayer)      == 0_i);        // Must return no error (0), as node does exist
        expect(t_asym.getIDs(IDs_secondLayer,0)   == 0_i);        // Must return no error (0), as node does exist
        expect(t_asym.getIDs(IDs_secondLayer,2)   == 1_i);        // Must return an error (1), as node does not exist
        expect(t_asym.getIDs(IDs_secondLayer,1)   == 1_i);        // Must return an error (0), as leafnodes haven't got any child

        // Test if return value is correct
        t_asym.getIDs(IDs_firstLayer);
        t_asym.getIDs(IDs_secondLayer,0);
        expect(IDs_firstLayer == array<uint8_t, 2>{0,1});
        expect(IDs_secondLayer == array<uint8_t, 4>{0,1,2,3});
  };




}