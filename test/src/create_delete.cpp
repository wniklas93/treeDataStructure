#include <boost/ut.hpp>
#include "tree/nodes.hpp"

#include <iostream>


template<uint8_t I,NodeLike... N>
struct NodeHeaderImpl{

    static constexpr uint8_t ID = I;

    bool active = false;
    uint8_t activeChildren = 0;

    bool guard(const uint8_t& queryID){
        return (queryID == ID) && (active == true) ? true : false;
    }

};

template<uint8_t I, auto V, class T>
struct LeafnodeHeaderImpl{

    static constexpr uint8_t ID = I;

    bool active = false;

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
        expect(t_simple.traverse<ReadOperation>(ID0)             == 1_i);       // Must return error (1), as leafnode and parent node haven't been activated yet
        expect(t_simple.traverse<ReadOperation>(ID1)             == 1_i);       // Must return error (1), as leafnode and parent node haven't been activated yet
        expect(t_simple.traverse<ReadOperation>(ID2)             == 1_i);       // Must return error (1), as leafnode and parent node haven't been activated yet
        expect(t_simple.traverse<CreateOperation>()              == 1_i);       // Must return error (1), parent node hasn't been activated yet
        expect(t_simple.traverse<DeleteOperation>()              == 1_i);       // Must return error (1), parent node hasn't been activated yet
        t_simple.header.active = true;
        expect(t_simple.traverse<ReadOperation>(ID0)             == 1_i);       // Must return error (1), as leafnode hasn't been activated
        CreateOperation::ID = 0;
        expect(t_simple.traverse<CreateOperation>()              == 0_i);       // Must return no error(0), as leafnode does exist and parent has been activated       
        CreateOperation::ID = 1;
        expect(t_simple.traverse<CreateOperation>()              == 0_i);       // Must return no error(0), as leafnode does exist and parent has been activated
        CreateOperation::ID = 6;
        expect(t_simple.traverse<CreateOperation>()              == 1_i);       // Must return an error(1), as leafnode doesn't exist
        WriteOperation::setValue<int>(3);
        expect(t_simple.traverse<WriteOperation>(ID0)            == 0_i);       // Must return no error (0), as leafnode does exist and is active
        expect(t_simple.traverse<ReadOperation>(ID0)             == 0_i);       // Must return no error (0), as leafnode does exist and is active
        expect(ReadOperation::getValue<int>()                    == 3_i);       // Variable must equal 3, as 3 was written to leafnode
        expect(t_simple.traverse<ReadOperation>(ID1)             == 0_i);       // Must return no error (0), as leafnode does exist and is active
        expect(t_simple.traverse<ReadOperation>(ID2)             == 1_i);       // Must return an error (1), as leafnode isn't active
        expect(t_simple.header.activeChildren                    == 2_i);       // Now, the root node has got 2 active children
        CreateOperation::ID = 1;
        expect(t_simple.traverse<CreateOperation>()              == 0_i);       // Must return no error(0), even if this node has already been activated 
        expect(t_simple.header.activeChildren                    == 2_i);       // Still, the root node has got 2 active children
        CreateOperation::ID = 10;
        expect(t_simple.traverse<CreateOperation>()              == 1_i);       // Must return an error (1), as leafnode does not exist 
        CreateOperation::ID = 2;
        expect(t_simple.traverse<CreateOperation>()              == 0_i);       // Must return no error (0), as leafnode does exist 
        expect(t_simple.header.activeChildren                    == 3_i);       // Now, the root node has got 3 active children 
        DeleteOperation::ID = 2;
        expect(t_simple.traverse<DeleteOperation>()              == 0_i);       // Must return no error (0), as leafnode does exist
        expect(t_simple.header.activeChildren                    == 2_i);       // Now, the root node has got 2 active children
        expect(t_simple.traverse<DeleteOperation>()              == 0_i);       // Must return no error(0), even if leafnode has already been deactivated
        expect(t_simple.header.activeChildren                    == 2_i);       // Still, the root node must have got 2 active children
        expect(t_simple.traverse<ReadOperation>(2)               == 1_i);       // Must return an error (1), as leafnode is not active anymore
        expect(t_simple.traverse<ReadOperation>(1)               == 0_i);       // Must return no error (0), as leafnode is still active
        DeleteOperation::ID = 2;
        expect(t_simple.traverse<DeleteOperation>()              == 0_i);       // Must return no error (0), eveb if leafnode has been deactivated
        expect(t_simple.header.activeChildren                    == 2_i);       // Still, the root node has got 2 active children
        DeleteOperation::ID = 2;
        expect(t_simple.traverse<DeleteOperation>(1)             == 1_i);       // Must return an error (1), as leafnodes haven't got any children which can be activated
        CreateOperation::ID = 2;
        expect(t_simple.traverse<CreateOperation>(0)             == 1_i);       // Must return an error (1), as leafnodes haven't got any children which can be activated
    };

    "create_delete_asym_tree"_test = [&] {
        
        // Test setupt
        AsymetricTree t_asym;
        SimpleTree* t_simple = t_asym.getChild<SimpleTree>(0);

        uint8_t ID0 = 0; uint8_t ID1 = 1; uint8_t ID2 = 2;      
        expect(t_asym.traverse<ReadOperation>(0,0)               == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
        expect(t_asym.traverse<ReadOperation>(1)                 == 1_i);        // Must return error (1), as leafnode hasn't been activated yet
        expect(t_asym.traverse<CreateOperation>(0,0)             == 1_i);        // Must return no error (0), as leafnode does exist
        expect(t_asym.traverse<DeleteOperation>(0,0)             == 1_i);        // Must return no error (0), as leafnode does exist
        expect(t_asym.traverse<ReadOperation>(0,0)               == 1_i);        // Must return an error (1), as path is not activated entirely (parent node is not active)
        CreateOperation::ID = 0;
        expect(t_asym.traverse<CreateOperation>()                == 1_i);        // Must return an error (1), as path is not activated entirely (parent node is not active)
        DeleteOperation::ID = 0;
        expect(t_asym.traverse<DeleteOperation>()                == 1_i);        // Must return an error (1), as path is not activated entirely (parent node is not active)
        CreateOperation::ID = 1;
        expect(t_asym.traverse<CreateOperation>()                == 1_i);        // Must return an error (1), as path is not activated entirely (parent node is not active)
        t_asym.header.active = true;
        DeleteOperation::ID = 0;
        expect(t_asym.traverse<DeleteOperation>()                == 0_i);        // Must return no error (0), even if child node has't been activated yet
        expect(t_asym.header.activeChildren                      == 0_i);        // Still, the root node has got 0 active children
        CreateOperation::ID = 0;
        expect(t_asym.traverse<CreateOperation>()                == 0_i);        // Must return no error (0), as child node does exist
        expect(t_asym.header.activeChildren                      == 1_i);        // Now, the root node has got 1 active child
        DeleteOperation::ID = 0;
        expect(t_asym.traverse<DeleteOperation>(0)               == 0_i);        // Must return no error (0), even if children haven't been activated
        expect(t_asym.header.activeChildren                      == 1_i);        // Still, the root node has got 1 active children
        DeleteOperation::ID = 0;
        expect(t_asym.traverse<DeleteOperation>(1)               == 1_i);        // Must return an error (1), as parent node is not active anymore  
        expect(t_asym.traverse<ReadOperation>(0)                 == 1_i);        // Must return an error (1), as path is not activated entirely
        expect(t_asym.traverse<ReadOperation>(1)                 == 1_i);        // Must return an error (1), as path is not activated entirely
        CreateOperation::ID = 1;
        expect(t_asym.traverse<CreateOperation>()                == 0_i);        // Must return no error (0)
        expect(t_asym.header.activeChildren                      == 2_i);        // Now, the root node has got 2 active child again
        expect(t_asym.traverse<ReadOperation>(0)                 == 1_i);        // Must return an error (1), inner node does not support read operation
        expect(t_asym.traverse<ReadOperation>(1)                 == 0_i);        // Must return no error (0), leafnode is active and does exist
        WriteOperation::setValue<double>(3.1);
        expect(t_asym.traverse<WriteOperation>(ID1)              == 0_i);        // Must return no error (0), as leafnode does exist and is active
        expect(t_asym.traverse<ReadOperation>(ID1)               == 0_i);        // Must return no error (0), as leafnode does exist and is active   
        expect(ReadOperation::getValue<double>()                 == 3.1_d);      // Variable must equal 3.1, as 3.1 was written to leafnode
        CreateOperation::ID = 0;
        expect(t_asym.traverse<CreateOperation>(0)               == 0_i);        // Must return no error (0), as leafnode does exist
        DeleteOperation::ID = 0;
        expect(t_simple->header.activeChildren                   == 1_i);        // Inner node has got one active child
        expect(t_asym.traverse<DeleteOperation>()                == 1_i);        // Must return an error (1), as node still got some active children
        CreateOperation::ID = 1;
        expect(t_asym.traverse<CreateOperation>(0)               == 0_i);        // Must return no error (0), as leafnode does exist
        expect(t_asym.traverse<ReadOperation>(0,2)               == 1_i);        // Must return an error (0), as leafnode is not active
        WriteOperation::setValue<double>(4.1);
        expect(t_asym.traverse<WriteOperation>(0,1)              == 0_i);        // Must return no error (0), as leafnode does exist and is active
        expect(t_asym.traverse<ReadOperation>(0,1)               == 0_i);        // Must return no error (0), as leafnode does exist and is active
        expect(ReadOperation::getValue<double>()                 == 4.1_d);      // Variable must equal 4.1, as 4.1 was written to leafnode
        CreateOperation::ID = 0;
        expect(t_asym.traverse<CreateOperation>(0)               == 0_i);        // Must return no error (0), even if node has already been activated
        DeleteOperation::ID = 0;
        expect(t_asym.traverse<DeleteOperation>(0)               == 0_i);        // Must return no error (0)
        expect(t_asym.traverse<ReadOperation>(0,0)               == 1_i);        // Must return an error (1), as leafnode is not active anymore
        DeleteOperation::ID = 1;
        expect(t_asym.traverse<DeleteOperation>(0)               == 0_i);        // Must return no error (0)
        expect(t_asym.traverse<ReadOperation>(0,1)               == 1_i);        // Must return an error (1), as leafnode is not active anymore
        DeleteOperation::ID = 0;
        expect(t_asym.traverse<DeleteOperation>()                == 0_i);        // Must return no error, as node hasn't got any active children anymore 
        expect(t_asym.header.activeChildren                      == 1_i);        // Root node has got 1 active child left
    };

}