#pragma once
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/join.hpp>
#include <iostream>
#include "s_type_traits.h"
using namespace boost::adaptors;

template<typename T>
using SValuePair=std::pair<const char*,T>;
template<typename... T>
using SValuePairs =std::tuple<SValuePair<T>...>;


template<class Tuple, typename OP, std::size_t N>
struct StdTupleVisitor {
    static void visit(const Tuple& t,const OP& op)
    {
        StdTupleVisitor<Tuple,OP, N-1>::visit(t,op);
        op(std::get<N-1>(t));
    }
};

template<class Tuple,typename OP>
struct StdTupleVisitor<Tuple,OP, 1> {
    static void visit(const Tuple& t,const OP& op)
    {
       op(std::get<0>(t));
    }
};

template<class Tuple, typename OP, std::size_t N>
struct StdTupleWriterVisitor {
    static void visit(Tuple& t,const OP& op)
    {
        StdTupleWriterVisitor<Tuple,OP, N-1>::visit(t,op);
        op(std::get<N-1>(t));
    }
};

template<class Tuple,typename OP>
struct StdTupleWriterVisitor<Tuple,OP, 1> {
    static void visit(Tuple& t,const OP& op)
    {
       op(std::get<0>(t));
    }
};

template<class Tuple, typename OP, std::size_t N>
struct BoostTupleVisitor {
    static void visit(const Tuple& t,const OP& op)
    {
        BoostTupleVisitor<Tuple,OP, N-1>::visit(t,op);
        op(boost::get<N-1>(t));
    }
};

template<class Tuple,typename OP>
struct BoostTupleVisitor<Tuple,OP, 1> {
    static void visit(const Tuple& t,const OP& op)
    {

       op(boost::get<0>(t));
    }
};

template<class Tuple, typename OP, std::size_t N>
struct BoostTupleWriterVisitor {
    static void visit(Tuple& t,const OP& op)
    {
        BoostTupleWriterVisitor<Tuple,OP, N-1>::visit(t,op);

        op(boost::get<N-1>(t));
    }
};

template<class Tuple,typename OP>
struct BoostTupleWriterVisitor<Tuple,OP, 1> {
    static void visit(Tuple& t,const OP& op)
    {

       op(boost::get<0>(t));
    }
};
namespace Ui {

    template<typename RetunType>
    struct ValueGetterVisitfun
    {

        template<typename T>
        void fromValuepairType(const T& val)const{
           STypeCheckHelper<T> v;//do nothing for these types
        }
        template<typename T>
        void fromValuepairType(const SValuePair<T>& val)const{
            if(name == val.first) {
                DISPLAY_TYPE(T);
                DISPLAY_TYPE(RetunType);
                std::cout<<"type mismatch in return type";
                assert(0);//"type mismatch in return type"
            }

        }
        std::string name ;
        boost::optional<RetunType>& refReturn;
        ValueGetterVisitfun(const char* nm, boost::optional<RetunType>& returnval):name(nm),refReturn(returnval) {}

        template<typename U ,typename std::enable_if<std::is_convertible<U,RetunType>::value,int>::type v=0>
        void operator()(const SValuePair<U>& val)const
        {
            if(name == val.first) {
                RetunType ret=val.second;
                refReturn = boost::optional<RetunType>(ret);
            }

        }

        template<typename... V>  //nested tuples
        void operator()(const std::tuple<V...>& val)const
        {
           constexpr size_t tupsize = std::tuple_size<std::tuple<V...>>::value;
           StdTupleVisitor<std::tuple<V...>,
                   ValueGetterVisitfun<RetunType>,
                   tupsize>::visit(val,ValueGetterVisitfun<RetunType>(name,refReturn));
        }
        template<typename V>
        void operator()(const V& val)const
        {
            fromValuepairType(val);
        }

    };

    template<typename ValueType>
    struct ValueSetterVisitfun
    {
        std::string name ;
        ValueType val;
        ValueSetterVisitfun(const char* nm, const ValueType& v):name(nm),val(v) {}

        template<typename U ,typename std::enable_if<std::is_same<ValueType,U>::value,int>::type v=0>
        void operator()(SValuePair<U>& val)const
        {
            if(name == val.first) {
                val.second=val;
            }

        }

        template<typename... V>  //nested tuples
        void operator()(std::tuple<V...>& val)const
        {
           constexpr size_t tupsize = std::tuple_size<std::tuple<V...>>::value;
           StdTupleVisitor<std::tuple<V...>,
                   ValueGetterVisitfun<ValueType>,
                   tupsize>::visit(val,ValueSetterVisitfun<ValueType>(name,val));
        }
        template<typename V>
        void operator()(V val)const
        {
           STypeCheckHelper<V> v;//do nothing for these types
        }
    };

    template< typename R,typename T>
    struct SValuePairGetter
    {
        using RetunType = R;
        const T & data;
        static constexpr size_t  tuplesize = boost::tuples::length<T>::value;

        SValuePairGetter(const T & t ):data(t){}

        RetunType get(const char* name)const
        {
            boost::optional<RetunType> returnval;
            BoostTupleVisitor<T,ValueGetterVisitfun<R>,tuplesize>::visit(data,ValueGetterVisitfun<R>(name,returnval));
            return returnval ? *returnval:RetunType();
        }
    };

    template< typename R,typename... T>
    struct SValuePairGetter<R,SValuePairs<T...>>
    {
        using RetunType = R;
        const SValuePairs<T...> & data;
        static constexpr size_t  tuplesize = std::tuple_size<SValuePairs<T...>>::value;

        SValuePairGetter(const SValuePairs<T...> & t ):data(t){}

        RetunType get(const char* name)const
        {
            boost::optional<RetunType> returnval;
            StdTupleVisitor<SValuePairs<T...>,ValueGetterVisitfun<RetunType>,tuplesize>::visit(data,ValueGetterVisitfun<R>(name,returnval));
            return returnval ? *returnval:RetunType();
        }

    };
    template<typename Ret,typename T>
    Ret getValue(const char* name ,const T& source){
        auto getter= SValuePairGetter<Ret,T>(source);
        return getter.get(name);
    }
    template<typename Ret,typename Range>
    std::vector<Ret> getValues(const char* name,const Range& range){
        std::vector<Ret> result;
        boost::transform(range,std::back_inserter(result),[&](auto data){
            return Ui::getValue<Ret>(name,data);
        });
        return result;
    }

    template<typename Ret,typename T>
    SValuePair<Ret> getValuePair(const char* name ,const T& source){
        auto getter= SValuePairGetter<Ret,T>(source);
        return SValuePair<Ret>{name,getter.get(name)};
    }

    template<typename Ret,typename Range>
    std::vector<SValuePair<Ret>> getValuePairs(const char* name ,const Range& range){
        std::vector<SValuePair<Ret>> result;
        boost::transform(range,std::back_inserter(result),[&](auto data){
            return Ui::getValuePair<Ret>(name,data);
        });
        return result;
    }


    template< typename V,typename T>
    struct SValuePairSetter
    {
        using Value_Type = V;
        T & data;
        static constexpr size_t  tuplesize = boost::tuples::length<T>::value;

        SValuePairSetter(T & t ):data(t){}

        void set(const char* name,const Value_Type& v)
        {
            BoostTupleWriterVisitor<T,ValueSetterVisitfun<V>,tuplesize>::visit(data,ValueSetterVisitfun<V>(name,v));

        }
    };

    template< typename V,typename... T>
    struct SValuePairSetter<V,SValuePairs<T...>>
    {
        using Value_Type = V;
        SValuePairs<T...> & data;
        static constexpr size_t  tuplesize = std::tuple_size<SValuePairs<T...>>::value;

        SValuePairSetter(SValuePairs<T...> & t ):data(t){}

        void set(const char* name,const Value_Type& v)
        {

            StdTupleWriterVisitor<SValuePairs<T...>,ValueSetterVisitfun<Value_Type>,tuplesize>::visit(data,ValueSetterVisitfun<V>(name,v));

        }

    };
    template<typename V,typename T>
    void setValue(const char* name ,const V& v,T& source){
        auto setter= SValuePairSetter<V,T>(source);
        setter.set(name,v);
    }


}
template <typename ItemType>
struct ModelVisitfun
{
    ItemType* mitem ;
    ModelVisitfun(ItemType* item):mitem(item) {}
    template<typename U>
    void operator()(const SValuePair<U>& val)const
    {
//        mitem->setProperty(val.first,val.second);
        mitem->setValue(val.first,val.second);
    }

    template<typename... V>  //nested tuples
    void operator()(const std::tuple<V...>& val)const
    {
       StdTupleVisitor<decltype(val),ModelVisitfun,sizeof...(V)>::visit(val,ModelVisitfun(mitem));
    }
    template<typename V>
    void operator()(const V& val)const
    {
       STypeCheckHelper<V> v;
    }
};
#define S_BEGIN_DATA(X) struct X {\
              auto operator()()const

#define S_END_DATA };
