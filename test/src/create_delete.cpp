#include <boost/ut.hpp>
#include "tree/nodes.hpp"

#include <iostream>


template<uint8_t I,NodeLike... N>
struct NodeHeaderImpl{

    static constexpr uint8_t ID = I;

    bool active = false;

    template<Visitor Vi>
    bool guard(const uint8_t& queryID){
        return (queryID == ID) && (active == true) ? true : false;
    }

};

template<uint8_t I, auto V, class T>
struct LeafnodeHeaderImpl{

    static constexpr uint8_t ID = I;

    bool active = false;

    template<Visitor Vi>
    bool guard(const uint8_t& queryID){
        return (queryID == ID) && (active == true) ? true : false;
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

int main ()
{

    using namespace boost::ut;

    "create_delete_simple_tree"_test = [&] {
        
        // Test setup
        SimpleTree t_simple;

        uint8_t ID0 = 0; uint8_t ID1 = 1; uint8_t ID2 = 2;      
        expect(t_simple.traverse<ReadOperation>(ID0)                    == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
        expect(t_simple.traverse<ReadOperation>(ID1)                    == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
        expect(t_simple.traverse<ReadOperation>(ID2)                    == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
        expect(t_simple.traverse<CreateOperation>()                     == 1_i);
        t_simple.header.active = true;
        CreateOperation::ID = 0;
        expect(t_simple.traverse<CreateOperation>()                     == 0_i);
        CreateOperation::ID = 1;
        expect(t_simple.traverse<CreateOperation>()                     == 0_i);    
        // expect(t_simple.traverse<CreateOperation>(ID0)                  == 0_i);        // Must return no error (0), as leafnode does exist
        // expect(t_simple.traverse<CreateOperation>(ID1)                  == 0_i);        // Must return no error (0), as leafnode does exist
        // expect(t_simple.traverse<CreateOperation>(ID2)                  == 0_i);        // Must return no error (0), as leafnode does exist
        // WriteOperation::setValue<int>(1);
        // expect(t_simple.traverse<WriteOperation>(0)                     == 0_i);        // Must return no error (0), as leafnode does exist and is active
        // expect(t_simple.traverse<ReadOperation>(0)                      == 0_i);        // Must return no error (0), as leafnode does exist and is active
        // expect(ReadOperation::getValue<int>()                           == 1_i);        // Variable must equal 1, as 1 was written to leafnode
        // expect(t_simple.traverse<DeleteOperation>(ID0)                  == 0_i);        // Must return no error (0), as leafnode is active  
        // expect(t_simple.traverse<WriteOperation>(0)                     == 1_i);        // Must return no error (0), as leafnode is active
        // expect(t_simple.traverse<DeleteOperation>(ID0)                  == 1_i);        // Must return error (0), as leafnode is not active
        // expect(t_simple.traverse<ReadOperation>(20)                     == 1_i);        // Must return error (1), as leafnode does not exist
    };

    "create_delete_asym_tree"_test = [&] {
        
        // Test setupt
        //AsymetricTree t_asym;

    //     uint8_t ID0 = 0; uint8_t ID1 = 1; uint8_t ID2 = 2;      
    //     expect(t_asym.traverse<ReadOperation>(0,0)                      == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
    //     expect(t_asym.traverse<ReadOperation>(1)                        == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
    //     expect(t_asym.traverse<DeleteOperation>(0,0)                    == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
    //     expect(t_asym.traverse<DeleteOperation>(1)                      == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
    //     expect(t_asym.traverse<CreateOperation>(0,0)                    == 0_i);        // Must return no error (0), as leafnode does exist
    //     expect(t_asym.traverse<ReadOperation>(0,0)                      == 1_i);        // Must return error (0), as path is not activated entirely
    //     expect(t_asym.traverse<CreateOperation>(0)                      == 0_i);        // Must return no error (0), as node does exist
    //     expect(t_asym.traverse<ReadOperation>(0,0)                      == 0_i);        // Must return error (0), as path is activated entirely
    //     expect(t_asym.traverse<CreateOperation>(1)                      == 0_i);
    //     expect(t_asym.traverse<ReadOperation>(1)                        == 0_i);
    //     expect(t_asym.traverse<DeleteOperation>(1)                      == 0_i);
    //     expect(t_asym.traverse<ReadOperation>(1)                        == 1_i);
    //     expect(t_asym.traverse<ReadOperation>(0,0)                      == 0_i);
    //     expect(t_asym.traverse<ReadOperation>(0,2)                      == 1_i);
    //     expect(t_asym.traverse<DeleteOperation>(0)                      == 0_i);
    //     expect(t_asym.traverse<ReadOperation>(0,0)                      == 1_i);
    //     expect(t_asym.traverse<DeleteOperation>(0,0)                    == 1_i);
    };

}