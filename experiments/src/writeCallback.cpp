#include <string>
#include <chrono>
#include <functional>
#include <iostream>

#include "tree/nodes.hpp"

using namespace Tree;

template<uint16_t I, bool B, NodeLike... N>
struct NodeHeader{
    static constexpr uint8_t ID = I;

    using childrenTypes = std::tuple<N...>;

    int guard(){
      return NO_ERROR;
    }

};

template<uint16_t I, auto V, class T>
struct LeafnodeHeader{
    static constexpr uint8_t ID = I;

    using type = T;
    inline const static T defaultValue = T(V);

    int guard(){
      return NO_ERROR;
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
        static int visitLeafnode(L* l){
            l->data = std::any_cast<decltype(l->data)>(value);
            headerSwitch(l->header);
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

        static constexpr auto headerSwitch = overloaded {
            []<LeafnodeHeaderWithCallbackConcept LHC>(LHC& header) {
                    header.callback("Invoke callback");
            },
            []<LeafnodeHeaderConcept LH> (LH& header){}
            };
};

template<class T>
struct SetCallbackOperation
{
    template <LeafnodeConcept L> 
    static int visitLeafnode(L *l)
    {
        headerSwitch(l->header);
        return NO_ERROR;
    }

    static void SetCallback(void(*cb)(T))
    {
        Callback = [cb](T value) { return (*cb)(value); };
    }

    template <NodeLike N> 
    static int previsit(N* n)
    {
        return NO_ERROR;
    }
private:
    inline static std::function<void(T)> Callback;

    static constexpr auto headerSwitch = overloaded {
        []<LeafnodeHeaderWithCallbackConcept LHW>(LHW header){
            // Todo: Make sure function signature match -> if not verbose error message
            header.callback = Callback;
            return false;
        },
        []<LeafnodeHeaderConcept LH>(LH header){return true;},
    };
};

void print(std::string input) {
    std::cout << input << std::endl;
}

void print_verbose(std::string input){
    std::cout << "New message: " << input << std::endl;
}


using SimpleTree = Node<
                    NodeHeader<
                        0,
                        true,
                        Leafnode<LeafnodeHeader<0,5,int>>,
                        Leafnode<LeafnodeHeaderWithCallback<1,5.5,double,print,std::function<void(std::string)>>>,
                        Leafnode<LeafnodeHeaderWithCallback<2,-4.5,float,print,std::function<void(std::string)>>>,
                        Leafnode<LeafnodeHeader<3,std::array<char,255>{"hello"},std::array<char,255>>>,
                        Leafnode<LeafnodeHeader<4,5,std::chrono::seconds>>,
                        Leafnode<LeafnodeHeader<9,nullptr,std::function<int()>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeader<
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

    // Set new callback function
    SetCallbackOperation<std::string>::SetCallback(&print_verbose);
    t_simple.traverse<SetCallbackOperation<std::string>>(1);

    WriteOperation::setValue<double>(1.1);
    t_simple.traverse<WriteOperation>(1);



    std::cout << std::endl;

    return 0;
}
