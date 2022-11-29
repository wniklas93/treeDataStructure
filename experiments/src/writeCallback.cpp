#include <string>
#include <chrono>
#include <functional>
#include <iostream>

#include "tree/nodes.hpp"



template<uint16_t I, bool B, NodeLike... N>
struct NodeHeaderImpl_{
    static constexpr uint8_t ID = I;

    using childrenTypes = std::tuple<N...>;

    bool guard(const uint16_t& queryID){
      return queryID == ID ? true : false;
    }

};

template<uint16_t I, auto V, class T>
struct LeafnodeHeader{
    static constexpr uint8_t ID = I;

    using type = T;
    inline const static T defaultValue = T(V);

    bool guard(const uint8_t& queryID){
      return queryID == ID ? true : false;
    }
};

template<uint16_t I, auto V, class T, auto F, class C>
struct LeafnodeHeaderWithCallback : LeafnodeHeader<I,V,T>{
    inline static C callback = C(F);
};

template<class T>
concept LeafnodeHeaderWithCallbackConcept = LeafnodeHeaderConcept<T> && 
                                            requires(T t){
                                                t.callback;
                                            };

struct WriteOperation{
    public:

        template<LeafnodeConcept L>
        static bool visitLeafnode(L* l){
            l->data = std::any_cast<decltype(l->data)>(value);
            headerSwitch(l->header);
            return false;
        }

        template<NodeLike N>
        static bool previsit(N* n){
          return true;
        }

        template<class T>
        static void setValue(const T& v){
            value = v;
            
        }

    private:
        inline static std::any value = nullptr;

        static constexpr auto headerSwitch = overloaded {
            []<LeafnodeHeaderWithCallbackConcept LHC>(LHC& header) {
                    header.callback("Invoke callback");
            },
            []<LeafnodeHeaderConcept LH> (LH& header){}
            };
};

void print(std::string input) {
    std::cout << input << std::endl;
}


using SimpleTree = Node<
                    NodeHeaderImpl_<
                        0,
                        true,
                        Leafnode<LeafnodeHeader<0,5,int>>,
                        Leafnode<LeafnodeHeaderWithCallback<1,5.5,double,print,void(*)(std::string)>>,
                        Leafnode<LeafnodeHeaderWithCallback<2,-4.5,float,print,void(*)(std::string)>>,
                        Leafnode<LeafnodeHeader<3,std::array<char,255>{"hello"},std::array<char,255>>>,
                        Leafnode<LeafnodeHeader<4,5,std::chrono::seconds>>,
                        Leafnode<LeafnodeHeader<9,nullptr,std::function<int()>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeaderImpl_<
                            0,
                            true,
                            SimpleTree,
                            Leafnode<LeafnodeHeader<1,2.5,double>>,
                            Leafnode<LeafnodeHeader<2,nullptr,int(*)(int,int)>>,
                            Leafnode<LeafnodeHeader<3,nullptr,std::function<int()>>>
                          >
                        >;

int main(){

    // 1. Simple tree
    std::cout << "Output for simple tree data structure:" << std::endl;
    SimpleTree t_simple;
    
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

    return 0;
}
