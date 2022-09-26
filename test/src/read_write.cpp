
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
  
  int             result_int;
  float           result_float;
  double          result_double;
  std::array<char,255> result_string;

  // Test simple tree (Tree with one layer)
  SimpleTree t_simple;


  "read_simple_tree"_test = [&] {
    expect(t_simple.read<float>(result_float,4)                 == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<float>(result_float,-4)                == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<float>(result_float,5)                 == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<int>(result_int,0)                     == 0_i);        // Must return no error (0), as leafnode does exist
    expect(result_int                                           == 5_i);        // Variable must equal 5, as 5 was written to leafnode
    expect(t_simple.read<double>(result_double,1)               == 0_i);        // Must return no error (0), as leafnode does exist
    expect(result_double                                        == 5.5_d);      // Variable must equal 5.5, as 5.5 was written to leafnode
    expect(t_simple.read<float>(result_float,2)                 == 0_i);        // Must return no error (0), as leafnode does exist
    expect(result_float                                         == -4.5_f);     // Variable must equal -4.5, as -4.5 was written to leafnode
    expect(t_simple.read<std::array<char,255>>(result_string,3) == 0_i);        // Must return no error (0), as leafnode does exist
    expect(result_string[0] == 'h' and result_string[1] == 'e');
  };

  "write_simple_tree"_test = [&] {
    expect(t_simple.write<int>(1,0)                                                 == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_simple.read<int>(result_int,0)                                         == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_int                                                               == 1_i);      // Variable must equal 1, as 1 was written to leafnode
    expect(t_simple.write<double>(1.3,1)                                            == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_simple.read<double>(result_double,1)                                   == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_double                                                            == 1.3_d);    // Variable must equal 1.3, as 1.3 was written to leafnode
    expect(t_simple.write<float>(1.3,2)                                             == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_simple.read<float>(result_float,2)                                     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_float                                                             == 1.3_f);    // Variable must equal 1.3, as 1.3 was written to leafnode
    expect(t_simple.write<std::array<char,255>>(std::array<char,255>{"test"},3)     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_simple.read<std::array<char,255>>(result_string,3)                     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_string[0] == 't' and result_string[1] == 'e' and result_string[2] == 's');      // Variable must equal 'test', as 'test' was written to leafnode
  };

  // Test asymetric tree (Tree with leafnodes in different layers)
  AsymetricTree t_asym;
  "read_exist_asym_tree"_test = [&] {
    expect(t_asym.read<double>(result_double,0)                    == 1_i);       // Must return error, as tree was not completely traversed
    expect(t_asym.read<int>(result_int,0)                          == 1_i);       // Must return error, as tree was not completely traversed
    expect(t_asym.read<std::array<char,255>>(result_string,0)      == 1_i);       // Must return error, as tree was not completely traversed
    expect(t_asym.read<double>(result_double,1)                    == 0_i);       // Must return no error (0), as leafnode does exist
    expect(result_double                                           == 2.5_d);     // Variable must equal 2.5, as 2.5 was written to leafnode
    expect(t_asym.read<int>(result_int,0,0)                        == 0_i);       // Must return no error (0), as leafnode does exist
    expect(result_int                                              == 5_i);       // Variable must equal 5, as 5 was written to leafnode
    expect(t_asym.read<double>(result_double,0,1)                  == 0_i);       // Must return no error (0), as leafnode does exist
    expect(result_double                                           == 5.5_d);     // Variable must equal 5.5, as 5.5 was written to leafnode
    expect(t_asym.read<std::array<char,255>>(result_string,0,3)    == 0_i);       // Must return no error (0), as leafnode does exist
    expect(result_string[0] ==  'h' and result_string[1] ==  'e');                // Variable must equal 'test', as 'test' was written to leafnode
  };

  "write_asym_tree"_test = [&] {
    expect(t_asym.write<int>(1,0,0)                                                 == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_asym.read<int>(result_int,0,0)                                         == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_int                                                               == 1_i);      // Variable must equal 1, as 1 was written to leafnode
    expect(t_asym.write<double>(1.3,0,1)                                            == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_asym.read<double>(result_double,0,1)                                   == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_double                                                            == 1.3_d);    // Variable must equal 1.3, as 1.3 was written to leafnode
    expect(t_asym.write<float>(1.3,0,2)                                             == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_asym.read<float>(result_float,0,2)                                     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_float                                                             == 1.3_f);    // Variable must equal 1.3, as 1.3 was written to leafnode
    expect(t_asym.write<std::array<char,255>>(std::array<char,255>{"test"},0,3)     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_asym.read<std::array<char,255>>(result_string,0,3)                     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_string[0] == 't' and result_string[1] == 'e' and result_string[2] == 's');      // Variable must equal 'test', as 'test was written to leafnode
    expect(t_asym.write<double>(-5,1)                                               == 0_i);      // Must return no error (0), as leafnode does exist
    expect(t_asym.read<double>(result_double,1)                                     == 0_i);      // Must return no error (0), as leafnode does exist
    expect(result_double                                                            == -5_i);     // Variable must equal -5, as -5 was written to leafnode
  };



}