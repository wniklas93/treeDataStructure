#ifndef NODES_TPP
#define NODES_TPP

//#ifndef NODES_HPP
//#error __FILE__ should only be included from nodes.hpp
//#endif

template<NodeHeader H>
template<uint8_t queriedID, uint8_t ID, NodeLike... N>
constexpr uint8_t Node<H>::id2idx<queriedID,NodeHeaderImpl<ID,N...>>::getIndex(){
            uint8_t idx = 0;

            auto mask = std::array<bool,sizeof...(N)> {
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




// template<NodeHeader H>
// bool Node<H>::getIDs(std::span<const uint8_t>& result) const{
//     result = std::span(getChildrenIDs<H>::value.begin(), getChildrenIDs<H>::value.end());
//     return false;
// }

// template<NodeHeader H>
// template<std::convertible_to<uint8_t>... R>
// bool Node<H>::getIDs(std::span<const uint8_t>& result, const uint8_t& ID, const R&... residualIDs) const{
//     bool error = true;

//     // Node switch:
//     //  |
//     //  |-Leafnodes: 1) Do nothing as leafnodes haven't got any children
//     //  |
//     //  |-Node:      1) Filter out nodes with ID not matching queried ID
//     //               2) Get children IDs

//     auto nodeSwitch = overloaded {
//         [&]<LeafnodeConcept L>(L l) {},
//         [&]<NodeConcept K>(K k){
//                 K::Header::guard(ID) ? (error = get<id2idx<K::Header::ID, Header>::getIndex()>(children).template
//                 getIDs(result,residualIDs...),
//                 false) : false;   
//         },
//     };

//     std::apply([&](const auto&... child){ (nodeSwitch(child), ...);}, children);

//     return error;
// }

#endif