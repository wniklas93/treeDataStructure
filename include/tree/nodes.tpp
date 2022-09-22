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
bool Node<H,N...>::read(T& result, const uint8_t& ID, const Args&... residualIDs){

    bool error = true;
        
    // Switch with read operation:
    //  |
    //  |-Leafnodes: 1) Filter out leafnodes with data types not matching to query data type   
    //  |            2) Filter out leafnodes with ID not matching queried ID
    //  |            3) Get data if matching leafnode was found
    //  |
    //  |-Node:      1) Filter out nodes with ID not matching queried ID
    //               2) Immerse one layer deeper
    
    auto switcher = overloaded {
        [&]<LeafNodeConcept L>(L l)
            {
                if constexpr ((is_same_v<typename L::Header::type,T>) && (sizeof... (residualIDs) == 0))
                {
                   L::Header::guard(ID) ? (error=false, result = get<id2idx<L::Header::ID>::getIndex()>(childs).data,
                   false) : false;
                }
            },
        [&]<NodeConcept K>(K k){
            if constexpr (sizeof... (residualIDs) > 0)
            {
                K::Header::guard(ID) ? (error = get<id2idx<K::Header::ID>::getIndex()>(childs).template
                read<T>(result,residualIDs...),
                false) : false;
            }
        },
    };

    // Apply switch
    std::apply([&](const auto&... child){ (switcher(child), ...);}, childs);

        
    //return move(ret); does not work because Werror=pessimizing-move
    return error;
}


template<NodeHeader H, NodeLike... N>
bool Node<H,N...>::getChildNum(size_t& result){
    result = sizeof...(N);
    return false;
}

template<NodeHeader H, NodeLike... N>
template<convertible_to<uint8_t>... R>
bool Node<H,N...>::getChildNum(size_t& result, const uint8_t& ID, const R&... residualIDs){
    bool error = true;

    auto switcher = overloaded {
        [&]<LeafNodeConcept L>(L l) {},
        [&]<NodeConcept K>(K k){
                K::Header::guard(ID) ? (error = get<id2idx<K::Header::ID>::getIndex()>(childs).template
                getChildNum(result,residualIDs...),
                false) : false;   
        },
    };

    std::apply([&](const auto&... child){ (switcher(child), ...);}, childs);

    return error;
}

template<NodeHeader H, NodeLike... N>
template<class T>
bool Node<H,N...>::getIDs(T& result){
    // Todo: check on array type
    result = T{(N::Header::ID)...};
    return false;
}

template<NodeHeader H, NodeLike... N>
template<class T, convertible_to<uint8_t>... R>
bool Node<H,N...>::getIDs(T& result, const uint8_t& ID, const R&... residualIDs){
    bool error = true;

    auto switcher = overloaded {
        [&]<LeafNodeConcept L>(L l) {},
        [&]<NodeConcept K>(K k){
                K::Header::guard(ID) ? (error = get<id2idx<K::Header::ID>::getIndex()>(childs).template
                getIDs(result,residualIDs...),
                false) : false;   
        },
    };

    std::apply([&](const auto&... child){ (switcher(child), ...);}, childs);

    return error;
}

#endif