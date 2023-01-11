
#include <iostream>
#include <stdint.h>
#include <variant>

#include <iostream>
#include <fstream>

#include "tree/nodes.hpp"

using namespace Tree;

// Function to track memory consumption of objects
void* operator new(std::size_t sz, std::size_t& allocated){
    allocated += sz;
    return std::malloc(sz);
}


// Definition of headers according to tree api
template<uint16_t I, NodeLike... N>
struct NodeHeader{
    static constexpr uint16_t ID = I;

    using childrenTypes = std::tuple<N...>;

    int guard(){
      return NO_ERROR;
    }
};

template<uint16_t I, auto V, class T>
struct LeafnodeHeaderImpl{
    static constexpr uint16_t ID = I;
    using type = T;
    inline constexpr static T defaultValue = T(V);
    
    int guard(){
      return NO_ERROR;
    }
};

// Definition of homogenous tree data structure
template<uint8_t ID>
using Zh = Leafnode<LeafnodeHeaderImpl<ID,10,int>>;

template<uint8_t ID, class... N>
using h0 = NodeHeader<ID,N...>;
 
template<uint8_t ID>
using Yh = nodeFactory<ID,h0,Zh,3>::type;
 
template<uint8_t ID>
using Xh = nodeFactory<ID,h0,Yh,3>::type;

using XYZTree_h = nodeFactory<0,h0,Xh,3>::type;

// Definition of tree data structure with varying data types
template<uint8_t ID>
using Yv = Node<
            NodeHeader<
                ID,
                Leafnode<LeafnodeHeaderImpl<0,10,int>>,
                Leafnode<LeafnodeHeaderImpl<1,10,double>>,
                Leafnode<LeafnodeHeaderImpl<2,10,float>>
             >
            >;

template<uint8_t ID>
using Xv = nodeFactory<ID, h0, Yv,3>::type;

using XYZTree_v = nodeFactory<0, h0, Xv,3>::type;

// Definition of homogenous array data structure
using XYZArray_h = std::array<std::array<std::array<int,3>,3>,3>;

// Definition of array data structure with varying data types
using XYZArray_v = std::array<std::array<std::array<std::variant<int,double, float>,3>,3>,3>;

int main (){
    
    std::ofstream gnuscript;
    gnuscript.open("experiments/memory_footprint.gnu");

    // Benchmark homogeneous tree data structure
    size_t allocatedTree_h = 0;
    XYZTree_h* th = new(allocatedTree_h) XYZTree_h;

    // Benchmark heterogenous tree data structure
    size_t allocatedTree_v = 0;
    XYZTree_v* tv = new(allocatedTree_v) XYZTree_v;

    // Benchmark simple array data structure
    size_t allocatedArray_h = 0;
    XYZArray_h* ah  = new(allocatedArray_h) XYZArray_h;

    // Benchmark simple array data structure
    size_t allocatedArray_v = 0;
    XYZArray_v* av  = new(allocatedArray_v) XYZArray_v;

    // Visualize object memory footprint via gnuplot script
    gnuscript << "$data << EOD \n";
    gnuscript << "2.75 " <<  allocatedArray_h << "\n";
    gnuscript << "3.25 " <<  allocatedTree_h  << "\n";
    gnuscript << "\n";
    gnuscript << "6.75 " << allocatedArray_v  << "\n";
    gnuscript << "7.25 " << allocatedTree_v   << "\n";
    gnuscript << "EOD \n";

    gnuscript << "set title \"Memory Footprint of Objects\" \n";
    gnuscript << "set xrange [0:10] \n";
    gnuscript << "set yrange [0:500] \n";
    gnuscript << "set ylabel \"Bytes\" \n";
    gnuscript << "unset xtics \n";
    gnuscript << "set xtics format \" \" \n";
    gnuscript << "set xtics (\"Homogeneous \\n (27 integers)\" 3, \"Heterogeneous \\n (18 integers, 9 floats)\" 7) \n";


    gnuscript << "set boxwidth 0.5 \n";
    gnuscript << "set style fill solid \n";
    gnuscript << "set terminal png \n";
    gnuscript << "set output \"experiments/memory_footprint.png\" \n";
    gnuscript << "plot $data every 2::0:0 using  1:2 with boxes ls 1 t \"Array Data Structure\",\\" << "\n";
    gnuscript << "$data every 2::1:0 using 1:2 with boxes ls 2 t \"Tree Data Structure\" \n";
    gnuscript.close();

    return 0;
}