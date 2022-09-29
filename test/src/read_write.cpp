
#include <boost/ut.hpp>

#include "tree/nodes.hpp"

template<uint8_t I,NodeLike... N>
struct NodeHeaderImpl{
    static constexpr uint8_t ID = I;

    static constexpr auto guard = [](const uint8_t& queryID){
        return queryID == ID ? true : false; 
    };
};

template<uint8_t I, auto V, class T>
struct LeafnodeHeaderImpl{
    static constexpr uint8_t ID = I;
    static constexpr auto guard = [](const uint8_t& queryID){
        return queryID == ID ? true : false; 
    };
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
  
  // Test simple tree (Tree with one layer)
  SimpleTree t_simple;

  "read_simple_tree"_test = [&] {
    expect(t_simple.traverse<ReadOperation>(4)                 == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(-4)                == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(5)                 == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(0)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                      == 5_i);        // Variable must equal 5, as 5 was written to leafnode
    expect(t_simple.traverse<ReadOperation>(1)                 == 0_i);     // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                   == 5.5_d);   // Variable must equal 5.5, as 5.5 was written to leafnode
    expect(t_simple.traverse<ReadOperation>(2)                 == 0_i);     // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                    == -4.5_f);  // Variable must equal -4.5, as -4.5 was written to leafnode
    expect(t_simple.traverse<ReadOperation>(3)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]  == 'h');        // Variable must equal "hello", as "hello" was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]  == 'e');
    expect(ReadOperation::getValue<std::array<char,255>>()[2]  == 'l');
   };

  "write_simple_tree"_test = [&] {
    WriteOperation::setValue<int>(1);
    expect(t_simple.traverse<WriteOperation>(0)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(0)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                                         == 1_i);            // Variable must equal 1, as 1 was written to leafnode
    WriteOperation::setValue<double>(1.1);
    expect(t_simple.traverse<WriteOperation>(1)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(1)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                                      == 1.1_d);          // Variable must equal 1.1, as 1.1 was written to leafnode
    WriteOperation::setValue<float>(5.1);
    expect(t_simple.traverse<WriteOperation>(2)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(2)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                                       == 5.1_f);          // Variable must equal 5.1, as 5.1 was written to leafnode
    WriteOperation::setValue<std::array<char,255>>(std::array<char,255>{"test"});
    expect(t_simple.traverse<WriteOperation>(3)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(3)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]                     == 't');            // Variable must equal 'test', as 'test was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]                     == 'e');
 };

    // Test asymetric tree (Tree with leafnodes in different layers)
    AsymetricTree t_asym;
  "read_exist_asym_tree"_test = [&] {
    expect(t_asym.traverse<ReadOperation>(0,6)                 == 1_i);        // Must return an error (1), as leafnode does not exist
    //expect(t_asym.traverse(read,1,-4)                  == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_asym.traverse<ReadOperation>(0,5)                 == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_asym.traverse<ReadOperation>(0,0)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                      == 5_i);        // Variable must equal 5, as 5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,1)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                   == 5.5_d);      // Variable must equal 5.5, as 5.5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,2)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                    == -4.5_f);     // Variable must equal -4.5, as -4.5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,3)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]  == 'h');        // Variable must equal "hello", as "hello" was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]  == 'e');
    expect(ReadOperation::getValue<std::array<char,255>>()[2]  == 'l');
    expect(t_asym.traverse<ReadOperation>(1)                   == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                   == 2.5_d);      // Variable must equal 2.5, as 2.5 was written to leafnode
  };

  "write_asym_tree"_test = [&] {
    WriteOperation::setValue<int>(3);
    expect(t_asym.traverse<WriteOperation>(0,0)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,0)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                                          == 3_i);            // Variable must equal 1, as 1 was written to leafnode
    WriteOperation::setValue<double>(3.3);
    expect(t_asym.traverse<WriteOperation>(0,1)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,1)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                                       == 3.3_d);          // Variable must equal 3.3, as 3.3 was written to leafnode
    WriteOperation::setValue<float>(-4.3);
    expect(t_asym.traverse<WriteOperation>(0,2)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,2)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                                        == -4.3_f);         // Variable must equal -4.3, as -4.3 was written to leafnode
    WriteOperation::setValue<std::array<char,255>>(std::array<char,255>{"test"});
    expect(t_asym.traverse<WriteOperation>(0,3)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,3)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]                      == 't');            // Variable must equal 'test', as 'test' was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]                      == 'e');
    WriteOperation::setValue<double>(-10.3);
    expect(t_asym.traverse<WriteOperation>(1)                                      == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(1)                                       == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                                       == -10.3_d);        // Variable must equal -10.3, as -10.3 was written to leafnode

  };



}