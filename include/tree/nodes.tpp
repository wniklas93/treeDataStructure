#ifndef NODES_TPP
#define NODES_TPP

//#ifndef NODES_HPP
//#error __FILE__ should only be included from nodes.hpp
//#endif

#include <iostream>

template<NodeHeader H, NodeLike... N>
template<uint8_t ID>
constexpr uint8_t Node<H,N...>::id2idx<ID>::getIndex(){
            uint8_t idx = 0;

            auto mask = array<bool,sizeof...(N)> {
                ID==N::Header::ID ? true : false...
            };

            for(bool element : mask){
                if(element == true) {
                    break;
                }else{
                    idx++;
                }
            }

            return idx;
        }

template<NodeHeader H, NodeLike... N>
template<class T, class... Args>
bool Node<H,N...>::read(auto& result, const uint8_t& ID, const Args&... residualIDs){

    bool error = true;
        
    if constexpr (sizeof... (residualIDs) > 0)
    {
        // Handle inner node

        initializer_list<bool> {
        N::Header::guard(ID) ? (error = get<id2idx<N::Header::ID>::getIndex()>(childs).template
         read<T>(result,residualIDs...),
        false) : false... 
        };
    }

        
    if constexpr(sizeof... (residualIDs) == 0)
    {

        // Sequential filter:
        // 1) Filter out inner nodes (Non-leafnodes)
        // 2) Filter out leafnodes storing the wrong type
        // 3) Filter out leafnodes with the wrong ID
        auto filter = overloaded {
            [&]<LeafNodeConcept L>(L l)
                {
                    if constexpr (is_same_v<typename L::Header::type,T>)
                    {
                       L::Header::guard(ID) ? (error=false, result = get<id2idx<L::Header::ID>::getIndex()>(childs).data,
                       false) : false;
                    }
                },
            [&]<NodeConcept K>(K k){},
        };

        // Apply filter
        std::apply([&](const auto&... child){ (filter(child), ...);}, childs);

    }
        
        //return move(ret); does not work because Werror=pessimizing-move
        return error;
}

#endif