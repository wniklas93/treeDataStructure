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
        static bool visit(N* n){return true;};
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
concept Visitor = requires (T t, archetypes::NodeLike* n){
    {t.visit(n)} -> std::same_as<bool>;
    
};


static_assert(NodeHeader<archetypes::NodeHeader>);
static_assert(Visitor<archetypes::Visitor>);
static_assert(NodeLike<archetypes::NodeLike>);


// Operations
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};


struct DecodeOperation{
    public:
        template<LeafnodeConcept L>
        static bool visit(L* l){
            std::cout << "Hello" << std::endl;
            typeSwitch(l->data);
        return false;
        }
    private:
        static constexpr auto typeSwitch = overloaded {
            [](float& f) {std::cout << "float " << f << std::endl;},
            [](int& i) {std::cout << "int " << i << std::endl;},
            [](double& d) {std::cout << "double " << d << std::endl;},
            [](char& c) {std::cout << "char " << c << std::endl;},
            [](auto& c) {},
            };

};

struct ReadOperation{
    public:

        template<LeafnodeConcept L>
        static bool visit(L* l){
            value = l->data;
        return false;
        }

        template<class T>
        static const T getValue(){
            return std::any_cast<T>(value);
        }

    private:
        inline static std::any value = nullptr;
};

struct WriteOperation{
    public:

        template<LeafnodeConcept L>
        static bool visit(L* l){
            l->data = std::any_cast<decltype(l->data)>(value);
        return false;
        }

        template<class T>
        static void setValue(const T& v){
            value = v;
            
        }

    private:
        inline static std::any value = nullptr;
};

struct GetIDsOperation{
    public:
        
        template<NodeConcept N>
        static bool visit(N* n){
            value = n->getChildrenIDs();
            
            return false;
        }
        
        inline static std::span<const uint8_t> value;
};

struct CreateOperation{
    public:

        template<NodeConcept N>
        static bool visit(N* n){
            bool error = true;
            
            auto nodeSwitch = overloaded {
                [&]<NodeLike L>(L& l) {
                  if(l.header.ID == ID){
                    error = false;
                    n->header.activeChildren += l.header.active != true;
                    l.header.active = true;
                  }  
                },
            };

            // Apply switch
            std::apply([&](auto&... child){ (nodeSwitch(child), ...);}, *n->getChildren());



            return error;
        }

        inline static uint8_t ID = 0;
};

struct DeleteOperation{
    public:
        
        template<NodeConcept N>
        static bool visit(N* n){
            bool error = true;

            auto nodeSwitch = overloaded {
                [&]<NodeConcept K>(K& k) {
                  if((k.header.ID == ID) && (k.header.activeChildren == 0)){
                    error = false;
                    n->header.activeChildren -= k.header.active == true;
                    k.header.active = false;
                  }  
                },
                [&]<LeafnodeConcept L>(L& l){
                    if(l.header.ID == ID){
                        error = false;
                        n->header.activeChildren -= l.header.active == true;
                        l.header.active = false;
                    }
                }
            };

            // Apply switch
            std::apply([&](auto&... child){ (nodeSwitch(child), ...);}, *n->getChildren());


            return error;
        }

        inline static uint8_t ID = 0;
};

// Data structure
template<class... Ts>
struct TypeList{};

template<class T, class... Ts>
struct inTypeList;

template<class T, class... Ts>
struct inTypeList<T,TypeList<Ts...>>{
    static constexpr bool value = (std::is_same_v<T,Ts> || ...);
};

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

        // Supported operations
        using validOperations = TypeList<ReadOperation,
                                         WriteOperation,
                                         DecodeOperation>;
    
    public:
        // Member variables
        getDefaultValue<H>::type data = getDefaultValue<H>::value();
        H header;

        template<Visitor V>
        bool accept() {
            if constexpr (inTypeList<V,validOperations>::value){
                using L = std::remove_pointer_t<decltype(this)>;
                return V::template visit<L>(this);
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
        template <typename T> struct getChildrenTypes{};

        template<uint8_t ID, NodeLike... N>
        struct getChildrenTypes<NodeHeaderImpl<ID,N...>>{
            using types = std::tuple<N...>;
        };

        template <typename T> struct ChildrenIDs{};

        template<uint8_t ID, NodeLike... N>
        struct ChildrenIDs<NodeHeaderImpl<ID,N...>>{
            static constexpr std::array<uint8_t, sizeof...(N)> value = {(N::getID())...};

            // Check for duplicates in ID-array
            static consteval bool uniqueIDs(){
                return [] () {
                    std::array<uint8_t, sizeof...(N)> childIDs = {(N::getID())...};
                    std::sort(childIDs.begin(), childIDs.end());
                    return (std::unique(childIDs.begin(),childIDs.end()) == childIDs.end());
                } ();
            }
        };

        // Supported operations
        using validOperations = TypeList<GetIDsOperation,
                                         CreateOperation,
                                         DeleteOperation>;

        // Member variables
        getChildrenTypes<H>::types children;

        // Check for unique children IDs
        static_assert(ChildrenIDs<H>::uniqueIDs() == true, 
                    "Children IDs must be unique!");
        
    public:

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
            
            if constexpr(inTypeList<V,validOperations>::value){
                using N = std::remove_pointer_t<decltype(this)>;
                return V::template visit<N>(this);
            }
            
            return true;
        }

        getChildrenTypes<H>::types* getChildren(){
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
            return std::span(ChildrenIDs<H>::value.begin(), 
                             ChildrenIDs<H>::value.end());
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
