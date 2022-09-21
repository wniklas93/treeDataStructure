
#include <iostream>
#include <stdlib.h>
#include <map>
#include <string>
#include <variant>

#include <iostream>
#include <fstream>

#include "tree/nodes.hpp"
#include "tree/nodeFactory.hpp"

// Function to track memory consumption of objects
void* operator new(std::size_t sz, std::size_t& allocated){
    allocated += sz;
    return std::malloc(sz);
}


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

// Definition of homogenous tree data structure
template<uint8_t ID>
using Zh = LeafNode<LeafHeaderImpl<ID,10,int>>;


template<uint8_t ID>
using Yh = nodeFactory<
                     NodeHeaderImpl<ID>,
                     Zh,
                     3
                     >::type;

template<uint8_t ID>
using Xh = nodeFactory<
                    NodeHeaderImpl<ID>,
                    Yh,
                    3
                    >::type;

using XYZTree_h = nodeFactory<
                    RootHeader,
                    Xh,
                    3
                    >::type;

// Definition of tree data structure with varying data types
template<uint8_t ID>
using Yv = Node<
                NodeHeaderImpl<ID>,
                LeafNode<LeafHeaderImpl<0,10,int>>,
                LeafNode<LeafHeaderImpl<1,10,double>>,
                LeafNode<LeafHeaderImpl<2,10,float>>
               >;

template<uint8_t ID>
using Xv = nodeFactory<
                    NodeHeaderImpl<ID>,
                    Yv,
                    3
                    >::type;

using XYZTree_v = nodeFactory<
                    RootHeader,
                    Xv,
                    3
                    >::type;

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

    std::cout << allocatedTree_v << std::endl;

    // Benchmark simple array data structure
    size_t allocatedArray_h = 0;
    XYZArray_h* ah  = new(allocatedArray_h) XYZArray_h;

    // Benchmark simple array data structure
    size_t allocatedArray_v = 0;
    XYZArray_v* av  = new(allocatedArray_v) XYZArray_v;

    std::cout << allocatedArray_v << std::endl;

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