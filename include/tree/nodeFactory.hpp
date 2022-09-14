#ifndef NODEFACTORY_HPP
#define NODEFACTORY_HPP

#include "nodes.hpp"

template<class H, template<uint8_t> class T, typename Seq>
struct expander;

template<class H, template<uint8_t> class T, std::size_t... Is>
struct expander<H, T, std::index_sequence<Is...>>{

    using type = Node<
                        H,
                        T<Is>...
                    >;
};

template <class H,template<uint8_t> class T, std::size_t N>
struct nodeFactory {
    using type = typename expander<H,T,std::make_index_sequence<N>>::type;
};

#endif