#ifndef NODES_TPP
#define NODES_TPP

//#ifndef NODES_HPP
//#error __FILE__ should only be included from nodes.hpp
//#endif

template<NodeHeader H>
template<uint8_t queriedID, uint8_t ID, NodeLike... N>
constexpr uint8_t Node<H>::id2idx<queriedID,NodeHeaderImpl<ID,N...>>::getIndex(){
            uint8_t idx = 0;

            auto mask = array<bool,sizeof...(N)> {
                queriedID==N::Header::ID ? true : false...
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

template<NodeHeader H>
template<class T, class... Args>
bool Node<H>::read(T& result, const uint8_t& ID, const Args&... residualIDs){

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
                   L::Header::guard(ID) ? (error=false, result = get<id2idx<L::Header::ID,Header>::getIndex()>(children).data,
                   false) : false;
                }
            },
        [&]<NodeConcept K>(K k){
            if constexpr (sizeof... (residualIDs) > 0)
            {
                K::Header::guard(ID) ? (error = get<id2idx<K::Header::ID,Header>::getIndex()>(children).template
                read<T>(result,residualIDs...),
                false) : false;
            }
        },
    };

    // Apply switch
    std::apply([&](const auto&... child){ (switcher(child), ...);}, children);

    return error;
}


template<NodeHeader H>
bool Node<H>::getIDs(span<const uint8_t>& result){
    result = span(getChildrenIDs<H>::value.begin(), getChildrenIDs<H>::value.end());
    return false;
}

template<NodeHeader H>
template<convertible_to<uint8_t>... R>
bool Node<H>::getIDs(span<const uint8_t>& result, const uint8_t& ID, const R&... residualIDs){
    bool error = true;

    auto switcher = overloaded {
        [&]<LeafNodeConcept L>(L l) {},
        [&]<NodeConcept K>(K k){
                K::Header::guard(ID) ? (error = get<id2idx<K::Header::ID, Header>::getIndex()>(children).template
                getIDs(result,residualIDs...),
                false) : false;   
        },
    };

    std::apply([&](const auto&... child){ (switcher(child), ...);}, children);

    return error;
}

#endif