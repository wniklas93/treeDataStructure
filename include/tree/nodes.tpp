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

#endif