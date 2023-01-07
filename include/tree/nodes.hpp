#pragma once

#include <utility>
#include <cstdint>
#include <tuple>
#include <algorithm>
#include <span>
#include <any>
#include <iostream>

namespace Tree{

// Data structure interface
// Todo: Code revision of concepts/interface
namespace archetypes{

    struct Visitor{
        Visitor() = delete;
        Visitor(Visitor const&) = delete;
        Visitor(Visitor&&) = delete;
        ~Visitor() = delete;

        template<class N>
        static int visitNode(N* n){return true;}

        template<class N>
        static int previsit(N* n){return true;}
    };

    struct NodeHeader{
        NodeHeader() = delete;
        NodeHeader(NodeHeader const&) = delete;
        NodeHeader(NodeHeader&&) = delete;
        ~NodeHeader() = delete;

        static constexpr uint16_t ID{0};

        static int guard(){return 0;}; 

        using childrenTypes = int;
    };

    struct NodeLike {
        NodeLike() = delete;
        NodeLike(NodeLike const&) = delete;
        NodeLike(NodeLike&&) = delete;
        ~NodeLike() = delete;

        template<class V, std::convertible_to<uint16_t>... Is>
        int traverse(const uint16_t& ID, const Is&... residualIDs){return true;}

        template<class V>
        int accept(){return true;};

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
concept LeafnodeHeaderConcept = requires (T t){
    {T::ID} -> std::convertible_to<uint16_t>;
    {t.template guard()} -> std::same_as<int>;
    typename T::type;
    T::defaultValue;
};

template<class T>
concept NodeHeaderConcept = requires (T t){
    {T::ID} -> std::convertible_to<uint16_t>;
    {t.template guard()} -> std::same_as<int>;
    typename T::childrenTypes;
};


template<class T>
concept LeafnodeConcept = requires (T t) {
                            {t.data}; //-> std::convertible_to<std::semiregular>;
                            {t.template accept<archetypes::Visitor>()} -> std::same_as<int>;
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
                            {t.template accept<archetypes::Visitor>()} -> std::same_as<int>;
                            {t.template traverse<archetypes::Visitor>(ID)} -> std::same_as<int>;
};

template<class T>
concept NodeLike = LeafnodeConcept<T> || NodeConcept<T>;

template<class T>
concept LeafnodeVisitor = requires(T t, archetypes::NodeLike* n){
    {t.visitLeafnode(n)} -> std::same_as<int>;
};

template<class T>
concept NodeVisitor = requires(T t, archetypes::NodeLike* n){
    {t.visitNode(n)} -> std::same_as<int>;
    {t.previsit(n)} -> std::same_as<int>;
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

// Error codes
constexpr int NO_ERROR = 0;
constexpr int ID_NOT_FOUND = 1;
constexpr int VISITOR_NOT_ACCEPTED = 2;

// Data structure

template<LeafnodeHeaderConcept H>
struct Leafnode{    
    public:
        // Member variables
        H::type data = H::defaultValue;
        H header;

        template<Visitor V>
        int accept() {
            if constexpr (LeafnodeVisitor<V>){
                using L = std::remove_pointer_t<decltype(this)>;
                return V::template visitLeafnode<L>(this);
            }
            return VISITOR_NOT_ACCEPTED;
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
        int traverse(const uint16_t& ID, const Is&... residualIDs){
            int error = ID_NOT_FOUND;
            
            auto nodeSwitch = overloaded {
                [&]<LeafnodeConcept L>(L& l) {

                    if(l.header.ID != ID) return;
                    error = l.header.template guard();
                    if(error != NO_ERROR) return;
                    
                    if constexpr (sizeof... (residualIDs) == 0)
                    {
                        V::template previsit<L>(&l);
                        error = l.template accept<V>();
                    } else {
                        error = ID_NOT_FOUND;
                    }
                },
                [&]<NodeConcept K>(K& k){

                    if(k.header.ID != ID) return;
                    error = k.header.template guard();
                    if(error != NO_ERROR) return;
                    
                    if constexpr (sizeof... (residualIDs) > 0)
                    {
                        V::template previsit<K>(&k);
                        error = k.template traverse<V>(residualIDs...);
                    } else {
                        error = k.template accept<V>();
                    }
                },
            };

            // Apply switch
            std::apply([&](auto&... child){ (nodeSwitch(child), ...);}, children);

            return std::move(error);
            
        }

        template<Visitor V>
        int traverse(){
            int error = header.template guard();
            if(error != NO_ERROR) return std::move(error);
            
            return accept<V>();
        }

        template<Visitor V>
        int accept(){
            
            if constexpr(NodeVisitor<V>){
                using N = std::remove_pointer_t<decltype(this)>;
                return V::template visitNode<N>(this);
            }
            
            return VISITOR_NOT_ACCEPTED;
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

} // end namespace "Tree"

