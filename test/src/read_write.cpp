
#include <string>
#include <chrono>
#include <functional>
#include <iostream>

#include <boost/ut.hpp>
#include "tree/nodes.hpp"



template<uint16_t I, bool B, NodeLike... N>
struct NodeHeaderImpl_{
    static constexpr uint8_t ID = I;

    using childrenTypes = std::tuple<N...>;

    bool guard(const uint16_t& queryID){
      return queryID == ID ? true : false;
    }

};

template<uint16_t I, auto V, class T>
struct LeafnodeHeader{
    static constexpr uint8_t ID = I;

    using type = T;
    inline const static T defaultValue = T(V);

    bool guard(const uint8_t& queryID){
      return queryID == ID ? true : false;
    }
};

struct ReadOperation{
    public:

        template<LeafnodeConcept L>
        static bool visitLeafnode(L* l){

            value = l->data;
        return false;
        }

        template<class T>
        static const T getValue(){
            return std::any_cast<T>(value);
        }

        template<NodeLike N>
        static bool previsit(N* n){
          return true;
        }

    private:
        inline static std::any value = nullptr;
};

struct WriteOperation{
    public:

        template<LeafnodeConcept L>
        static bool visitLeafnode(L* l){
            l->data = std::any_cast<decltype(l->data)>(value);
        return false;
        }

        template<NodeLike N>
        static bool previsit(N* n){
          return true;
        }

        template<class T>
        static void setValue(const T& v){
            value = v;
            
        }

    private:
        inline static std::any value = nullptr;
};

int sumMock(int a, int b){
  return a + b;
}

struct Mock {
  public:
    constexpr Mock(int k, float l,int(*)(int,int)){};
    constexpr ~Mock(){};
    constexpr Mock operator=(Mock m){
      return Mock(4,3,&sumMock);
    };

    float f = 2.3;

    int sum(int a, int b){
      return a + b;
    }

    int i = 0;
};

using SimpleTree = Node<
                    NodeHeaderImpl_<
                        0,
                        true,
                        Leafnode<LeafnodeHeader<0,5,int>>,
                        Leafnode<LeafnodeHeader<1,5.5,double>>,
                        Leafnode<LeafnodeHeader<2,-4.5,float>>,
                        Leafnode<LeafnodeHeader<3,std::array<char,255>{"hello"},std::array<char,255>>>,
                        Leafnode<LeafnodeHeader<4,5,std::chrono::seconds>>,
                        Leafnode<LeafnodeHeader<5,Mock{2,3,&sumMock},Mock>>,
                        Leafnode<LeafnodeHeader<6,nullptr,int(*)(int,int)>>,
                        Leafnode<LeafnodeHeader<7,sumMock,int(*)(int,int)>>,
                        Leafnode<LeafnodeHeader<8,&Mock::sum,int(Mock::*)(int,int)>>,
                        Leafnode<LeafnodeHeader<9,nullptr,std::function<int()>>>
                      >
                  >;

using AsymetricTree = Node<
                        NodeHeaderImpl_<
                            0,
                            true,
                            SimpleTree,
                            Leafnode<LeafnodeHeader<1,2.5,double>>,
                            Leafnode<LeafnodeHeader<2,nullptr,int(*)(int,int)>>,
                            Leafnode<LeafnodeHeader<3,nullptr,std::function<int()>>>
                          >
                        >;

int main() {
  
  using namespace boost::ut;
  
  // Test simple tree (Tree with one layer)
  SimpleTree t_simple;
  
  "read_simple_tree"_test = [&] {
    expect(t_simple.traverse<ReadOperation>(10)                         == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(11)                         == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(0,1,2)                      == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(-4)                         == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_simple.traverse<ReadOperation>(0)                          == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                               == 5_i);        // Variable must equal 5, as 5 was written to leafnode
    expect(t_simple.traverse<ReadOperation>(1)                          == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                            == 5.5_d);      // Variable must equal 5.5, as 5.5 was written to leafnode
    expect(t_simple.traverse<ReadOperation>(2)                          == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                             == -4.5_f);     // Variable must equal -4.5, as -4.5 was written to leafnode
    expect(t_simple.traverse<ReadOperation>(3)                          == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]           == 'h');        // Variable must equal "hello", as "hello" was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]           == 'e');
    expect(ReadOperation::getValue<std::array<char,255>>()[2]           == 'l');
    expect(t_simple.traverse<ReadOperation>(4)                          == 0_i);
    expect(ReadOperation::getValue<std::chrono::seconds>().count()      == 5_i);
    expect(t_simple.traverse<ReadOperation>(5)                          == 0_i);
    expect(ReadOperation::getValue<Mock>().f                            == 2.3_f);
    expect(t_simple.traverse<ReadOperation>(6)                          == 0_i);
    expect(ReadOperation::getValue<int(*)(int,int)>()                   == nullptr);
    expect(t_simple.traverse<ReadOperation>(7)                          == 0_i);
    expect(ReadOperation::getValue<int(*)(int,int)>()                   == &sumMock);
    expect(ReadOperation::getValue<int(*)(int,int)>()(1,2)              == 3_i);
    expect(t_simple.traverse<ReadOperation>(8)                          == 0_i);
    Mock m(4,2,&sumMock);
    expect(ReadOperation::getValue<int(Mock::*)(int,int)>()             == &Mock::sum);
    expect((m.*ReadOperation::getValue<int(Mock::*)(int,int)>())(3,2)   == 5_i);
    expect(t_simple.traverse<ReadOperation>(9)                          == 0_i);
    expect(ReadOperation::getValue<std::function<int()>>()              == nullptr);
   };

  "write_simple_tree"_test = [&] {
    WriteOperation::setValue<int>(1);
    expect(t_simple.traverse<WriteOperation>(0)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(0)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                                         == 1_i);            // Variable must equal 1, as 1 was written to leafnode
    WriteOperation::setValue<double>(1.1);
    expect(t_simple.traverse<WriteOperation>(1)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(1)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                                      == 1.1_d);          // Variable must equal 1.1, as 1.1 was written to leafnode
    WriteOperation::setValue<float>(5.1);
    expect(t_simple.traverse<WriteOperation>(2)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(2)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                                       == 5.1_f);          // Variable must equal 5.1, as 5.1 was written to leafnode
    WriteOperation::setValue<std::array<char,255>>(std::array<char,255>{"test"});
    expect(t_simple.traverse<WriteOperation>(3)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(3)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]                     == 't');            // Variable must equal 'test', as 'test was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]                     == 'e');
    WriteOperation::setValue<std::chrono::seconds>(std::chrono::seconds(8));
    expect(t_simple.traverse<WriteOperation>(4)                                   == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_simple.traverse<ReadOperation>(4)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::chrono::seconds>().count()                == 8_i);
    WriteOperation::setValue<int(*)(int,int)>(&sumMock);
    expect(t_simple.traverse<WriteOperation>(6)                                   == 0_i);
    expect(t_simple.traverse<ReadOperation>(6)                                    == 0_i);
    expect(ReadOperation::getValue<int(*)(int,int)>()(3,4)                        == 7_i);
    WriteOperation::setValue<int(*)(int,int)>(nullptr);
    expect(t_simple.traverse<WriteOperation>(7)                                   == 0_i);
    expect(t_simple.traverse<ReadOperation>(7)                                    == 0_i);
    expect(ReadOperation::getValue<int(*)(int,int)>()                             == nullptr);
    Mock m(4,2,&sumMock);
    WriteOperation::setValue<std::function<int()>>(std::bind(
        &Mock::sum,&m,1,2));
    expect(t_simple.traverse<WriteOperation>(9)                                   == 0_i);
    expect(t_simple.traverse<ReadOperation>(9)                                    == 0_i);
    expect(ReadOperation::getValue<std::function<int()>>()()                      == 3_i);
 };

  // Test asymetric tree (Tree with leafnodes in different layers)
  AsymetricTree t_asym;
  "read_exist_asym_tree"_test = [&] {
    expect(t_asym.traverse<ReadOperation>(0,10)                     == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_asym.traverse<ReadOperation>(0,11)                     == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_asym.traverse<ReadOperation>(1,14)                     == 1_i);        // Must return an error (1), as leafnode does not exist
    expect(t_asym.traverse<ReadOperation>(0,0)                      == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                           == 5_i);        // Variable must equal 5, as 5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,1)                      == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                        == 5.5_d);      // Variable must equal 5.5, as 5.5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,2)                      == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                         == -4.5_f);     // Variable must equal -4.5, as -4.5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,3)                      == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]       == 'h');        // Variable must equal "hello", as "hello" was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]       == 'e');
    expect(ReadOperation::getValue<std::array<char,255>>()[2]       == 'l');
    expect(t_asym.traverse<ReadOperation>(1)                        == 0_i);        // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                        == 2.5_d);      // Variable must equal 2.5, as 2.5 was written to leafnode
    expect(t_asym.traverse<ReadOperation>(0,4)                      == 0_i);
    expect(ReadOperation::getValue<std::chrono::seconds>().count()  == 5_i);
    expect(t_asym.traverse<ReadOperation>(0,5)                      == 0_i);
    expect(ReadOperation::getValue<Mock>().f                        == 2.3_f);
    expect(t_asym.traverse<ReadOperation>(2)                        == 0_i);
    expect(ReadOperation::getValue<int(*)(int,int)>()               == nullptr);
    expect(t_asym.traverse<ReadOperation>(3)                        == 0_i);
    expect(ReadOperation::getValue<std::function<int()>>()          == nullptr);

  };

  "write_asym_tree"_test = [&] {
    WriteOperation::setValue<int>(3);
    expect(t_asym.traverse<WriteOperation>(0,0)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,0)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<int>()                                          == 3_i);            // Variable must equal 1, as 1 was written to leafnode
    WriteOperation::setValue<double>(3.3);
    expect(t_asym.traverse<WriteOperation>(0,1)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,1)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                                       == 3.3_d);          // Variable must equal 3.3, as 3.3 was written to leafnode
    WriteOperation::setValue<float>(-4.3);
    expect(t_asym.traverse<WriteOperation>(0,2)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,2)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<float>()                                        == -4.3_f);         // Variable must equal -4.3, as -4.3 was written to leafnode
    WriteOperation::setValue<std::array<char,255>>(std::array<char,255>{"test"});
    expect(t_asym.traverse<WriteOperation>(0,3)                                    == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(0,3)                                     == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<std::array<char,255>>()[0]                      == 't');            // Variable must equal 'test', as 'test' was written to leafnode
    expect(ReadOperation::getValue<std::array<char,255>>()[1]                      == 'e');
    WriteOperation::setValue<double>(-10.3);
    expect(t_asym.traverse<WriteOperation>(1)                                      == 0_i);            // Must return no error (0), as leafnode does exist
    expect(t_asym.traverse<ReadOperation>(1)                                       == 0_i);            // Must return no error (0), as leafnode does exist
    expect(ReadOperation::getValue<double>()                                       == -10.3_d);        // Variable must equal -10.3, as -10.3 was written to leafnode
    WriteOperation::setValue<int(*)(int,int)>(&sumMock);
    expect(t_asym.traverse<WriteOperation>(2)                                      == 0_i);
    expect(t_asym.traverse<ReadOperation>(2)                                       == 0_i);
    expect(ReadOperation::getValue<int(*)(int,int)>()(4,4)                         == 8_i);
    Mock m(4,2,&sumMock);
    WriteOperation::setValue<std::function<int()>>(std::bind(
        &Mock::sum,&m,8,2));
    expect(t_asym.traverse<WriteOperation>(3)                                      == 0_i);
    expect(t_asym.traverse<ReadOperation>(3)                                       == 0_i);
    expect(ReadOperation::getValue<std::function<int()>>()()                       == 10_i);
  };



}