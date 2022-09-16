
#include <iostream>
#include <stdlib.h>
#include <map>
#include <string>
#include <variant>

#include "tree/nodes.hpp"
#include "tree/nodeFactory.hpp"
#include "matplotlibcpp.hpp"

namespace plt = matplotlibcpp;


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
    
    // Benchmark homogenous tree data structure
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

    // Plot results
    // Set the size of output image to 1200x780 pixels
    plt::figure_size(1200, 780);

    plt::bar(std::vector<std::size_t>{0},
    std::vector<std::size_t>{allocatedTree_h},
    "lightyellow",
    "-",
    0.2,
    std::map<std::string, std::string>{{"color", "steelblue"}});

    plt::bar(std::vector<std::size_t>{1},
    std::vector<std::size_t>{allocatedArray_h},
    "black",
    "-",
    0.2,
    std::map<std::string, std::string>{{"color", "mediumseagreen"}});

    plt::bar(std::vector<std::size_t>{2},
    std::vector<std::size_t>{allocatedTree_v},
    "black",
    "-",
    0.2,
    std::map<std::string, std::string>{{"color", "royalblue"}});

    plt::bar(std::vector<std::size_t>{3},
    std::vector<std::size_t>{allocatedArray_v},
    "black",
    "-",
    0.2,
    std::map<std::string, std::string>{{"color", "mediumpurple"}});
    plt::show();
    plt::legend();
    return 0;
}