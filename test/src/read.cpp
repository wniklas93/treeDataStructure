
#include <boost/ut.hpp>

#include "tree/nodes.hpp"
#include "tree/nodeFactory.hpp"

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
                        LeafNode<LeafHeaderImpl<1,5.5,double>>
                        >;


int main() {
  
  using namespace boost::ut;
  
  int             result_int;
  float           result_float;
  double          result_double;
  array<char,255> result_string;

  // Test simple tree (Tree with one layer)
  SimpleTree t_simple;


  "read_exist_simple_tree"_test = [&] {
    expect(t_simple.read<int>(result_int,0)                == 0_i);        // Must return no error (0), as leafnode does exist
    expect(t_simple.read<double>(result_double,1)          == 0_i);        // ""
    expect(t_simple.read<float>(result_float,2)            == 0_i);        // ""
    expect(t_simple.read<array<char,255>>(result_string,3) == 0_i);        // ""
    expect(t_simple.read<float>(result_float,4)            == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.read<float>(result_float,-4)           == 1_i);        // Must return an error (1), as leafnode does not exist
  };

  // Test asymetric tree (Tree with leafnodes in different layers)
  AsymetricTree t_asym;
  "read_exist_asym_tree"_test = [&] {
    //expect(t_asym.read<double>(result_double,1) == 0_i);
  };

}