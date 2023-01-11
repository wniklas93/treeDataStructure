#include "tree/nodes.hpp"

#include <iostream>
#include <chrono>
#include <functional>

using namespace Tree;

constexpr int MISSING_PERMISSION = 3;

struct Permission {
    unsigned int r:1;
    unsigned int w:1;
    unsigned int x:1;
}__attribute__((packed));

template<uint16_t I, bool B, NodeLike... N>
struct NodeHeader{
    static constexpr uint8_t ID = I;

    using childrenTypes = std::tuple<N...>;

    int guard(){
      return NO_ERROR;
    }

};

template<uint16_t I, auto V, auto P, class T>
struct LeafnodeHeader{
    static constexpr uint8_t ID = I;

    using type = T;
    inline const static T defaultValue = T(V);
    
    int guard(){
      return NO_ERROR;
    }

    Permission permission = P;

};

struct ReadOperation{
    public:

        template<LeafnodeConcept L>
        static int visitLeafnode(L* l){
            if (!l->header.permission.r){
                std::cout << "Read permission required!" << std::endl;
                return MISSING_PERMISSION;
            }
            std::cout << "Read access granted" << std::endl;
            value = l->data;
            return NO_ERROR;
        }

        template<class T>
        static const T getValue(){
            return std::any_cast<T>(value);
        }

        template<NodeLike N>
        static int previsit(N* n){
            return NO_ERROR;
        }

    private:
        inline static std::any value = nullptr;
};

struct WriteOperation{
    public:

        template<LeafnodeConcept L>
        static int visitLeafnode(L* l){
            if(!l->header.permission.w){
                std::cout << "Write permission required!" << std::endl;
                return MISSING_PERMISSION;
            }
            std::cout << "Write access granted" << std::endl;
            l->data = std::any_cast<decltype(l->data)>(value);
            return NO_ERROR;
        }

        template<NodeLike N>
        static int previsit(N* n){
            return NO_ERROR;
        }

        template<class T>
        static void setValue(const T& v){
            value = v;
            
        }

    private:
        inline static std::any value = nullptr;
};

void print(std::string input){
    std::cout << input << std::endl;
}

using SimpleTree = Node<
                    NodeHeader<
                        0,
                        true,
                        Leafnode<LeafnodeHeader<0,5,Permission{1,1,0},int>>,
                        Leafnode<LeafnodeHeader<1,5.5,Permission{1,1,0},double>>,
                        Leafnode<LeafnodeHeader<2,-4.5,Permission{1,1,0},float>>,
                        Leafnode<LeafnodeHeader<3,std::array<char,255>{"hello"},Permission{1,1,0},std::array<char,255>>>,
                        Leafnode<LeafnodeHeader<4,5,Permission{1,1,0},std::chrono::seconds>>,
                        Leafnode<LeafnodeHeader<6,nullptr,Permission{0,1,1},int(*)(int,int)>>,
                        Leafnode<LeafnodeHeader<9,nullptr,Permission{0,1,1},std::function<int()>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeader<
                            0,
                            true,
                            SimpleTree,
                            Leafnode<LeafnodeHeader<1,2.5,Permission{1,1,0},double>>,
                            Leafnode<LeafnodeHeader<2,nullptr,Permission{0,1,1},int(*)(int,int)>>,
                            Leafnode<LeafnodeHeader<3,nullptr,Permission{0,1,1},std::function<int()>>>
                          >
                        >;

int main (){

    // 1. Simple tree
    std::cout << "Output for simple tree data structure:" << std::endl;
    SimpleTree t_simple;

    // Read operation
    t_simple.traverse<ReadOperation>(0);
    t_simple.traverse<ReadOperation>(1);
    t_simple.traverse<ReadOperation>(2);
    t_simple.traverse<ReadOperation>(3);
    t_simple.traverse<ReadOperation>(4);
    t_simple.traverse<ReadOperation>(6);
    t_simple.traverse<ReadOperation>(9);
    
    // Write operation
    WriteOperation::setValue<int>(1);
    t_simple.traverse<WriteOperation>(0);
    WriteOperation::setValue<double>(1.1);
    t_simple.traverse<WriteOperation>(1);
    WriteOperation::setValue<float>(5.1);
    t_simple.traverse<WriteOperation>(2);
    WriteOperation::setValue<std::array<char,255>>(std::array<char,255>{"test"});
    t_simple.traverse<WriteOperation>(3);
    WriteOperation::setValue<std::chrono::seconds>(std::chrono::seconds(8));

    std::cout << std::endl;

    // 2. Asymetric tree
    std::cout << "Output for asymetric tree data structure:" << std::endl;
    AsymetricTree t_asym;

    // Read operation
    t_asym.traverse<ReadOperation>(0,0);
    t_asym.traverse<ReadOperation>(0,1);
    t_asym.traverse<ReadOperation>(0,2);
    t_asym.traverse<ReadOperation>(0,3);
    t_asym.traverse<ReadOperation>(0,4);
    t_asym.traverse<ReadOperation>(0,5);
    t_asym.traverse<ReadOperation>(0,6);
    t_asym.traverse<ReadOperation>(0,9);
    t_asym.traverse<ReadOperation>(1);
    t_asym.traverse<ReadOperation>(2);
    t_asym.traverse<ReadOperation>(3);

    // Write operation


    return 0;
}