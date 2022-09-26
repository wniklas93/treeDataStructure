#ifndef NODES_H
#define NODES_H

#include <cstdint>
#include <tuple>
#include <initializer_list>
#include <concepts>
#include <type_traits>
#include <array>
#include <utility>
#include <span>
#include <algorithm>


template<class T>
concept NodeHeader = requires (){
    {T::ID} -> std::convertible_to<uint8_t>;
    {T::guard} -> std::convertible_to<bool>;
};

// ToDo: delete (redundant)
template<class T>
concept LeafnodeHeader = requires () {
    {T::ID} -> std::convertible_to<uint8_t>;
    {T::guard} -> std::convertible_to<bool>;
};


template<class T>
concept LeafnodeConcept = requires (T t) {
                            t.data;
                            typename T::Header;
};

template<class T>
concept NodeConcept = requires (T t){
                            t.children;
                            typename T::Header;
};

template<class T>
concept NodeLike = LeafnodeConcept<T> || NodeConcept<T>;

template<uint8_t I, auto V, class T>
struct LeafnodeHeaderImpl;

template<LeafnodeHeader H>
struct Leafnode{
    private:
        // Helpers for internal usage
        template <class T> struct getType{};

        template<uint8_t I, auto V, class T>
        struct getType<LeafnodeHeaderImpl<I,V,T>>{
            using type = T;
        };

        template <class T> struct getDefaultValue{};

        template<uint8_t I, auto V, class T>
        struct getDefaultValue<LeafnodeHeaderImpl<I,V,T>>{
            static constexpr T value = V;
        };
    
    public:
        // Member variables
        using datatype = getType<H>::type;
        static constexpr datatype defaultValue = getDefaultValue<H>::value;
        datatype data = getDefaultValue<H>::value;
        using Header = H;
};

template<uint8_t ID, NodeLike... N>
struct NodeHeaderImpl;

template<NodeHeader H>
struct Node{

    private:
        // Helpers for internal usage
        template<uint8_t queriedID, class T> struct id2idx;

        template<uint8_t queriedID, uint8_t ID, NodeLike... N>
        struct id2idx<queriedID, NodeHeaderImpl<ID,N...>> {
            static constexpr uint8_t getIndex();
        };

        template <typename T> struct getChildrenTypes{};

        template<uint8_t ID, NodeLike... N>
        struct getChildrenTypes<NodeHeaderImpl<ID,N...>>{
            using types = std::tuple<N...>;
        };

        template <typename T> struct getChildrenIDs{};

        template<uint8_t ID, NodeLike... N>
        struct getChildrenIDs<NodeHeaderImpl<ID,N...>>{
            static constexpr std::array<uint8_t, sizeof...(N)> value = {(N::Header::ID)...};

            // Check for duplicates in ID-array
            static consteval bool uniqueIDs(){
                return [] () {
                    std::array<uint8_t, sizeof...(N)> childIDs = {(N::Header::ID)...};
                    std::sort(childIDs.begin(), childIDs.end());
                    return (std::unique(childIDs.begin(),childIDs.end()) == childIDs.end());
                } ();
            }
        };
    
        template<class... Ts>
        struct overloaded : Ts... {
            using Ts::operator()...;
        };

        // Error when using gcc-11:
        // template<class... Ts>
        // overloaded(Ts...) -> overloaded<Ts...>;

    public:
    
        // Api functions
        template<class T, std::convertible_to<uint8_t>... I>
        bool read(T& result, const uint8_t& ID, const I&... residualIDs) const;

        template<class T, std::convertible_to<uint8_t>... I>
        bool write(const T&& value, const uint8_t&ID, const I&... residualIDs);

        bool getIDs(std::span<const uint8_t>& result) const;

        template<std::convertible_to<uint8_t>...R>
        bool getIDs(std::span<const uint8_t>& result, const uint8_t& ID, const R&... residualIDs) const;
    
        // Member variables
        getChildrenTypes<H>::types children;
        using Header = H;

        // Check for unique children IDs
        static_assert(getChildrenIDs<H>::uniqueIDs() == true, 
                    "Children IDs must be unique!");

};

template<uint8_t ID, template<uint8_t> class T, typename Seq>
struct expander;

template<uint8_t ID, template<uint8_t> class T, std::size_t... Is>
struct expander<ID, T, std::index_sequence<Is...>>{

    using type = Node<
                        NodeHeaderImpl<ID,T<Is>...>
                    >;
};

template<uint8_t ID, template<uint8_t> class T, std::size_t N>
struct nodeFactory {
    using type = typename expander<ID, T,std::make_index_sequence<N>>::type;
};

// Include template implementation
#include "nodes.tpp"

#endif