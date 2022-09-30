#ifndef NODES_H
#define NODES_H

#include <utility>
#include <cstdint>
#include <tuple>
#include <algorithm>
#include <span>
#include <any>

// Data structure interface

template<class T>
concept NodeHeader = requires (){
    {T::ID} -> std::convertible_to<uint8_t>;
    {T::guard} -> std::convertible_to<bool>;
};

template<class T>
concept LeafnodeConcept = requires (T t) {
                            t.data;
                            typename T::Header;
                            typename T::datatype;
};

template<class T>
concept NodeConcept = requires (T t){
                            typename T::Header;
                            t.getChildrenIDs();
};

template<class T>
concept NodeLike = LeafnodeConcept<T> || NodeConcept<T>;

template<class T>
concept Visitor = requires (T t){
    std::is_object_v<T>;
};

// Operations
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
        inline static std::any value;
};

struct WriteOperation{
    public:

        template<LeafnodeConcept L>
        static bool visit(L* l){
            l->data = std::any_cast<typename L::datatype>(value);
        return false;
        }

        template<class T>
        static void setValue(const T& v){
            value = v;
            
        }

    private:
        inline static std::any value;
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
            static constexpr T value = T(V);
        };

        // Supported operations
        using validOperations = TypeList<ReadOperation,
                                         WriteOperation>;
    
    public:
        // Member variables
        using datatype = getDefaultValue<H>::type;
        static constexpr datatype defaultValue = getDefaultValue<H>::value;
        datatype data = getDefaultValue<H>::value;
        using Header = H;

        template<Visitor V>
        bool accept() {
            if constexpr (inTypeList<V,validOperations>::value){
                using L = std::remove_pointer_t<decltype(this)>;
                return V::template visit<L>(this);
            }
            return true;
        }
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

        template <typename T> struct ChildrenIDs{};

        template<uint8_t ID, NodeLike... N>
        struct ChildrenIDs<NodeHeaderImpl<ID,N...>>{
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

        // Supported operations
        using validOperations = TypeList<GetIDsOperation>;

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
                        if(L::Header::guard(ID)) error = l.template accept<V>();
                    }
                },
                [&]<NodeConcept K>(K& k){
                    
                    if constexpr (sizeof... (residualIDs) > 0)
                    {
                        K::Header::guard(ID) ? (error = k.template traverse<V>(residualIDs...),
                        false) : false;
                    } else {
                        K::Header::guard(ID) ? error = k.template accept<V>() : false;
                    }
                },
            };

            // Apply switch
            std::apply([&](auto&... child){ (nodeSwitch(child), ...);}, children);

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

        std::span<const uint8_t> getChildrenIDs() const {
            return std::span(ChildrenIDs<H>::value.begin(), 
                             ChildrenIDs<H>::value.end());
        }
    
        using Header = H;
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
