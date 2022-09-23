
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
struct LeafHeaderImpl{
    static constexpr uint8_t ID = I;
    using type = T;
    static constexpr type defaultValue = V;
    static constexpr auto guard = [](const uint8_t& queryID){
        return queryID == ID ? true : false; 
    };
};

using SimpleTree = Node<
                    NodeHeaderImpl<
                        0,
                        LeafNode<LeafHeaderImpl<0,5,int>>,
                        LeafNode<LeafHeaderImpl<1,5.5,double>>,
                        LeafNode<LeafHeaderImpl<2,-4.5,float>>,
                        LeafNode<LeafHeaderImpl<3,array<char,255>{"hello"},array<char,255>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeaderImpl<
                            0,
                            SimpleTree,
                            LeafNode<LeafHeaderImpl<1,2.5,double>>
                          >
                        >;

int main() {
  
  using namespace boost::ut;
  
  int             result_int;
  float           result_float;
  double          result_double0;
  double          result_double1;
  array<char,255> result_string;

  // Test simple tree (Tree with one layer)
  SimpleTree t_simple;


  "read_exist_simple_tree"_test = [&] {
    expect(t_simple.read<float>(result_float,4)            == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<float>(result_float,-4)           == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<float>(result_float,5)            == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<int>(result_int,0)                == 0_i);        // Must return no error (0), as leafnode does exist
    expect(t_simple.read<double>(result_double0,1)          == 0_i);       // Must return no error (0), as leafnode does exist
    expect(t_simple.read<float>(result_float,2)            == 0_i);        // Must return no error (0), as leafnode does exist
    expect(t_simple.read<array<char,255>>(result_string,3) == 0_i);        // Must return no error (0), as leafnode does exist
  };

  "read_result_simple_tree"_test = [&]{
    expect(result_int       == 5_i);
    expect(result_double0   == 5.5_d);
    expect(result_float     == -4.5_f);
    expect(result_string[0] == 'h');
    expect(result_string[1] == 'e');
    expect(result_string[2] == 'l');    
  };
  
  // Test asymetric tree (Tree with leafnodes in different layers)
  AsymetricTree t_asym;
  "read_exist_asym_tree"_test = [&] {
    expect(t_asym.read<double>(result_double0,0)             == 1_i);       // Must return error, as tree was not completely traversed
    expect(t_asym.read<int>(result_int,0)                    == 1_i);       // Must return error, as tree was not completely traversed
    expect(t_asym.read<array<char,255>>(result_string,0)     == 1_i);       // Must return error, as tree was not completely traversed
    expect(t_asym.read<double>(result_double0,1)             == 0_i);       // Must return no error (0), as leafnode does exist
    expect(t_asym.read<int>(result_int,0,0)                  == 0_i);       // Must return no error (0), as leafnode does exist
    expect(t_asym.read<double>(result_double1,0,1)           == 0_i);       // Must return no error (0), as leafnode does exist
    expect(t_asym.read<array<char,255>>(result_string,0,3)   == 0_i);       // Must return no error (0), as leafnode does exist
  };

    "read_result_asym_tree"_test = [&]{
    expect(result_double0   ==  2.5_d);
    expect(result_int       ==  5_i);
    expect(result_float     == -4.5_f);
    expect(result_double1   ==  5.5_d);
    expect(result_string[0] ==  'h');
    expect(result_string[1] ==  'e');
    expect(result_string[2] ==  'l');    
  };



}