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
        static bool visitNode(N* n){return true;};
    };

    struct NodeHeader{
        NodeHeader() = delete;
        NodeHeader(NodeHeader const&) = delete;
        NodeHeader(NodeHeader&&) = delete;
        ~NodeHeader() = delete;

        static constexpr uint8_t ID{0};

        static bool guard(uint8_t ID){return true;}; 
    };

    struct NodeLike {
        NodeLike() = delete;
        NodeLike(NodeLike const&) = delete;
        NodeLike(NodeLike&&) = delete;
        ~NodeLike() = delete;

        template<class V, std::convertible_to<uint8_t>... Is>
        bool traverse(const uint8_t& ID, const Is&... residualIDs){return true;}

        template<class V>
        bool accept(){return true;};

        std::span<const uint8_t> getChildrenIDs() const {return std::span<uint8_t>{};};

        struct R {};

        R* getChildren(){R* r;return r;}

        template<class N>
        N* getChild(const uint8_t& ID){N* n; return n;}

        NodeHeader header;
        R data;
    };
}

template<class T>
concept NodeHeader = requires (uint8_t ID, T t){
    {T::ID} -> std::convertible_to<uint8_t>;
    {t.template guard(ID)} -> std::same_as<bool>;
};


template<class T>
concept LeafnodeConcept = requires (T t) {
                            {t.data}; //-> std::convertible_to<std::semiregular>;
                            {t.template accept<archetypes::Visitor>()} -> std::same_as<bool>;
                            t.header;
};

template<class T>
concept NodeConcept = requires (T t, uint8_t ID){
                            t.header;
                            {t.template getChild<archetypes::NodeLike>(ID)};
                            {t.getChildren()};
                            {t.getChildrenIDs()} -> std::same_as<std::span<const uint8_t>>;
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
};

template<class T>
concept Visitor = LeafnodeVisitor<T> || NodeVisitor<T>;

static_assert(NodeHeader<archetypes::NodeHeader>);
static_assert(Visitor<archetypes::Visitor>);
static_assert(NodeLike<archetypes::NodeLike>);


// Operations
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};


// Data structure
template<uint8_t I, auto V, class T>
struct LeafnodeHeaderImpl;

template<NodeHeader H>
struct Leafnode{
    private:
        // Helpers for internal usage
        template <class T> struct getDefaultValue{};

        template<uint8_t I, auto V, class T>
        struct getDefaultValue<LeafnodeHeaderImpl<I,V,T>>{
            using type = T;

            constexpr static T value(){
                return T(V);
            }
        };
    
    public:
        // Member variables
        getDefaultValue<H>::type data = getDefaultValue<H>::value();
        H header;

        template<Visitor V>
        bool accept() {
            if constexpr (LeafnodeVisitor<V>){
                using L = std::remove_pointer_t<decltype(this)>;
                return V::template visitLeafnode<L>(this);
            }
            return true;
        }

       static constexpr uint8_t getID() {
            return H::ID;
        }
};

template<uint8_t ID, NodeLike... N>
struct NodeHeaderImpl;

template<NodeHeader H>
struct Node{

    private:
        // Helpers for internal usage
        template <typename T> struct Children{};

        template<uint8_t I, NodeLike... N>
        struct Children<NodeHeaderImpl<I,N...>>{
            using types = std::tuple<N...>;
            static constexpr std::array<uint8_t, sizeof...(N)> value = {(N::getID())...};

            // Check for duplicates in ID-array
            static consteval bool uniqueIDs(){
                return [] () {
                    std::array<uint8_t, sizeof...(N)> childIDs = {(N::getID())...};
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

            static constexpr uint8_t id2idx(const uint8_t& queriedID){
                uint8_t idx = 0;
                for(const uint8_t& ID : value){
                    if(ID==queriedID) return idx;
                    idx++;
                }
                return idx;
            }
        };

        // Member variables
        Children<H>::types children;

        // Check for unique children IDs
        static_assert(Children<H>::uniqueIDs() == true, 
                    "Children IDs must be unique!");
        
    public:

        static constexpr uint16_t getNumLeafnodes(){
            return Children<H>::getNumLeafnodes();
        }

        template<Visitor V, std::convertible_to<uint8_t>... Is>
        bool traverse(const uint8_t& ID, const Is&... residualIDs){
            bool error = true;
            
            auto nodeSwitch = overloaded {
                [&]<LeafnodeConcept L>(L& l) {
                    
                    if constexpr (sizeof... (residualIDs) == 0)
                    {
                        if(l.header.template guard(ID)) error = l.template accept<V>();
                    }
                },
                [&]<NodeConcept K>(K& k){
                    
                    if constexpr (sizeof... (residualIDs) > 0)
                    {
                        k.header.template guard(ID) ? (error = k.template traverse<V>(residualIDs...),
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

        Children<H>::types* getChildren(){
            return &children;
        }

        template<NodeLike N>
        N* getChild(const uint8_t& queriedID){
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

        std::span<const uint8_t> getChildrenIDs() const {
            return std::span(Children<H>::value.begin(), 
                             Children<H>::value.end());
        }

       static constexpr uint8_t getID() {
            return H::ID;
        }
    
        H header;
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

#endif
