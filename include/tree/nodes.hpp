#ifndef NODES_H
#define NODES_H

#include <utility>
#include <cstdint>
#include <tuple>
#include <algorithm>
#include <span>
#include <any>
#include <iostream>

// Data structure interface
// Todo: Code revision of concepts/interface
namespace archetypes{

    struct Visitor{
        Visitor() = delete;
        Visitor(Visitor const&) = delete;
        Visitor(Visitor&&) = delete;
        ~Visitor() = delete;

        template<class N>
        static bool visitNode(N* n){return true;}

        template<class N>
        static bool previsit(N* n){return true;}
    };

    struct NodeHeader{
        NodeHeader() = delete;
        NodeHeader(NodeHeader const&) = delete;
        NodeHeader(NodeHeader&&) = delete;
        ~NodeHeader() = delete;

        static constexpr uint16_t ID{0};

        static bool guard(uint16_t ID){return true;}; 

        using childrenTypes = int;
    };

    struct NodeLike {
        NodeLike() = delete;
        NodeLike(NodeLike const&) = delete;
        NodeLike(NodeLike&&) = delete;
        ~NodeLike() = delete;

        template<class V, std::convertible_to<uint16_t>... Is>
        bool traverse(const uint16_t& ID, const Is&... residualIDs){return true;}

        template<class V>
        bool accept(){return true;};

        std::span<const uint16_t> getChildrenIDs() const {return std::span<uint16_t>{};};

        struct R {};

        R* getChildren(){R* r;return r;}

        template<class N>
        N* getChild(const uint16_t& ID){N* n; return n;}

        static constexpr uint16_t getID(){ return 0;}

        static constexpr uint16_t getNumLeafnodes() {return 0;}

        NodeHeader header;
        R data;
        using childrenTypes = std::tuple<int,float>;
        using type = int;
        static constexpr int defaultValue = 1;
    };
}

template<class T>
concept LeafnodeHeaderConcept = requires (uint16_t ID, T t){
    {T::ID} -> std::convertible_to<uint16_t>;
    {t.template guard(ID)} -> std::same_as<bool>;
    typename T::type;
    T::defaultValue;
};

template<class T>
concept NodeHeaderConcept = requires (uint16_t ID, T t){
    {T::ID} -> std::convertible_to<uint16_t>;
    {t.template guard(ID)} -> std::same_as<bool>;
    typename T::childrenTypes;
};


template<class T>
concept LeafnodeConcept = requires (T t) {
                            {t.data}; //-> std::convertible_to<std::semiregular>;
                            {t.template accept<archetypes::Visitor>()} -> std::same_as<bool>;
                            t.header;
                            {T::getID()} -> std::same_as<uint16_t>;
};

template<class T>
concept NodeConcept = requires (T t, uint16_t ID){
                            t.header;
                            {T::getID()} -> std::same_as<uint16_t>;
                            {T::getNumLeafnodes()} -> std::same_as<uint16_t>;
                            {t.template getChild<archetypes::NodeLike>(ID)};
                            {t.getChildren()};
                            {t.getChildrenIDs()} -> std::same_as<std::span<const uint16_t>>;
                            {t.template accept<archetypes::Visitor>()} -> std::same_as<bool>;
                            {t.template traverse<archetypes::Visitor>(ID)} -> std::same_as<bool>;
};

template<class T>
concept NodeLike = LeafnodeConcept<T> || NodeConcept<T>;

template<class T>
concept LeafnodeVisitor = requires(T t, archetypes::NodeLike* n){
    {t.visitLeafnode(n)} -> std::same_as<bool>;
};

template<class T>
concept NodeVisitor = requires(T t, archetypes::NodeLike* n){
    {t.visitNode(n)} -> std::same_as<bool>;
    {t.previsit(n)} -> std::same_as<bool>;
};

template<class T>
concept Visitor = LeafnodeVisitor<T> || NodeVisitor<T>;

static_assert(NodeHeaderConcept<archetypes::NodeHeader>);
static_assert(Visitor<archetypes::Visitor>);
static_assert(NodeLike<archetypes::NodeLike>);


// Operations
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};


// Data structure

template<LeafnodeHeaderConcept H>
struct Leafnode{    
    public:
        // Member variables
        H::type data = H::defaultValue;
        H header;

        template<Visitor V>
        bool accept() {
            if constexpr (LeafnodeVisitor<V>){
                using L = std::remove_pointer_t<decltype(this)>;
                return V::template visitLeafnode<L>(this);
            }
            return true;
        }

       static constexpr uint16_t getID() {
            return H::ID;
        }
};

template<NodeHeaderConcept H>
struct Node{

    private:
        // Helpers for internal usage
        template <typename T> struct Children{};

        template<NodeLike... N>
        struct Children<std::tuple<N...>>{
            
            static constexpr std::array<uint16_t, sizeof...(N)> value = {(N::getID())...};

            // Check for duplicates in ID-array
            static consteval bool uniqueIDs(){
                return [] () {
                    std::array<uint16_t, sizeof...(N)> childIDs = {(N::getID())...};
                    std::sort(childIDs.begin(), childIDs.end());
                    return (std::unique(childIDs.begin(),childIDs.end()) == childIDs.end());
                } ();
            }


            static constexpr auto getNumLeafnodes = [](){
                return (... + [](){
                    if constexpr(NodeConcept<N>){
                        return N::getNumLeafnodes();
                    }else{
                        return 1;
                    }
                }());
            };
        };

        // Member variables
        H::childrenTypes children;

        // Check for unique children IDs
        static_assert(Children<typename H::childrenTypes>::uniqueIDs() == true, 
                    "Children IDs must be unique!");
        
    public:

        static constexpr uint16_t getNumLeafnodes(){
            return Children<typename H::childrenTypes>::getNumLeafnodes();
        }

        template<Visitor V, std::convertible_to<uint16_t>... Is>
        bool traverse(const uint16_t& ID, const Is&... residualIDs){
            bool error = true;
            
            auto nodeSwitch = overloaded {
                [&]<LeafnodeConcept L>(L& l) {
                    
                    if constexpr (sizeof... (residualIDs) == 0)
                    {
                        if(l.header.template guard(ID)){
                            V::template previsit<L>(&l);
                            error = l.template accept<V>();
                        }
                    }
                },
                [&]<NodeConcept K>(K& k){
                    
                    if constexpr (sizeof... (residualIDs) > 0)
                    {
                        k.header.template guard(ID) ? (
                            V::template previsit<K>(&k),
                            error = k.template traverse<V>(residualIDs...),
                        false) : false;
                    } else {
                        k.header.template guard(ID) ? error = k.template accept<V>() : false;
                    }
                },
            };

            // Apply switch
            std::apply([&](auto&... child){ (nodeSwitch(child), ...);}, children);

            return error;
            
        }

        template<Visitor V>
        bool traverse(){
            bool error = true;
            
            header.template guard(header.ID) ? error = accept<V>() : false;

            return error;
        }

        template<Visitor V>
        bool accept(){
            
            if constexpr(NodeVisitor<V>){
                using N = std::remove_pointer_t<decltype(this)>;
                return V::template visitNode<N>(this);
            }
            
            return true;
        }

        H::childrenTypes* getChildren(){
            return &children;
        }

        template<NodeLike N>
        N* getChild(const uint16_t& queriedID){
            N* n;
            auto nodeSwitch = overloaded {
                [&]<NodeLike M>(M& m) {
                    if constexpr (std::is_same_v<N,M>){
                        n = &m;
                    }
                },
            };

            // Apply switch
            std::apply([&](auto&... child){ (nodeSwitch(child), ...);}, children);
            return n;
        }

        std::span<const uint16_t> getChildrenIDs() const {
            return std::span(Children<typename H::childrenTypes>::value.begin(), 
                             Children<typename H::childrenTypes>::value.end());
        }

       static constexpr uint16_t getID() {
            return H::ID;
        }
    
        H header;
};

template<NodeLike... N>
struct NodeList {};

template<template<uint16_t> class T, typename Seq>
struct expander;

template<template<uint16_t> class T, std::size_t... Is>
struct expander<T, std::index_sequence<Is...>>{

    using type = NodeList<T<Is>...>;
};

// Todo: Figure out why gcc 11 complains about variadic NodeLike template in nodeFactory,
// Todo: while gcc 12 does not complain (template<NodeLike...> class H)
template<uint16_t ID, template<uint16_t, class...> class H, template<uint16_t> class T, std::size_t N>
struct nodeFactory {

    // 1) Create node list
    using nl = typename expander<T,std::make_index_sequence<N>>::type;

    // 2) Create header
    template<uint16_t I, class NL>
    struct Build{};

    template<uint16_t I,NodeLike... L>
    struct Build<I,NodeList<L...>>{
        using type = H<I,L...>;
    };

    using header = Build<ID, nl>::type;

    // 3) Create node
    using type = Node<header>;
};

#endif
