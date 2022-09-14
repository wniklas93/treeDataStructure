#ifndef NODES_TPP
#define NODES_TPP

//#ifndef NODES_HPP
//#error __FILE__ should only be included from nodes.hpp
//#endif

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

    bool error;
        
    //Generic compile-time implementation of switch-case
    if constexpr (sizeof... (residualIDs) > 0)
    {
        initializer_list<bool> {
        N::Header::guard(ID) ? (error = get<id2idx<N::Header::ID>::getIndex()>(childs).template
         read<T>(result,residualIDs...),
        false) : false... 
        };
    }

        
    if constexpr(sizeof... (residualIDs) == 0)
    {
        constexpr typename filter<T,N...>::type matchingLeafs;

        result = apply([&](auto... Match) {
            T result;
            initializer_list<bool> {
            decltype(Match)::Header::guard(ID) ? (error=false, result = get<id2idx<decltype(Match)::Header::ID>::getIndex()>(childs).data,
            false) : false... 
            };

            return result;                
        ;}, matchingLeafs);
    }
        
        //return move(ret); does not work because Werror=pessimizing-move
        return error;
}

#endif