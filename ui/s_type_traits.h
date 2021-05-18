#ifndef STYPETRAITS_H
#define STYPETRAITS_H
#if __cplusplus >= 201103L
#include <type_traits>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>
using namespace std;
#else
#endif
namespace Ui {
    template<typename U>
    struct Value_Type;

    template<typename U>
    struct Value_Type<U*>{
        using type = U;
    };
    template<typename U>
    struct Value_Type<const U*>{
        using type = U;
    };

    template<typename T>
    struct function_ref;
    template<typename Return , typename... Args>
    struct function_ref<Return(Args...)>
    {
        void* _ptr;
        using ERASED_FN = Return (*)(void*,Args...);
        ERASED_FN erased_fn;
    public:
        template <typename F>
        function_ref(F&& x):_ptr(&x){
            erased_fn=[](void* fptr,Args&&... as)->Return{
                return (*reinterpret_cast<F*>(fptr))(std::forward<Args>(as)...);
            };
        }
        Return operator()(Args... as){
           return erased_fn(_ptr,std::forward<Args>(as)...);
        }

    };
}
namespace Ui {
    template <class...> using void_t = void;
    struct details {
      template<template<class...>class Z, class=void, class...Ts>
      struct can_apply:
        std::false_type
      {};
      template<template<class...>class Z, class...Ts>
      struct can_apply<Z, Ui::void_t<Z<Ts...>>, Ts...>:
        std::true_type
      {};
    };
    template<template<class...>class Z, class...Ts>
    using can_apply = details::can_apply<Z, void, Ts...>;

    template<typename Type,typename Ret, typename... FunArgs>
    struct check_callable{
        template <class T,  class Retv,class... Args>
        using function_type = decltype(declval<T&>().operator()(declval<Args>()...));

        template <typename T,class =void>
        struct has_fun_callable:false_type{};

        template<typename T>
        struct has_fun_callable<T,void_t<function_type<T,Ret,FunArgs...>>>:is_same<function_type<T,Ret,FunArgs...>,Ret>{};
        using fun=has_fun_callable<Type>;
    };
    template <typename T>
    struct s_callable:false_type{};
    template<typename T, typename Ret, typename... Args>
    struct s_callable<Ret(T::*)(Args...)>
    {
       static constexpr bool value = check_callable<T,Ret,Args...>::fun::value;
       static constexpr bool type = check_callable<T,Ret,Args...>::fun::type;
    };
    template <typename T>
    using s_callable_t=typename s_callable<T>::type;
    template <typename T>
    using s_callable_v=typename s_callable<T>::value;
}

#define DEFINE_DETECTOR(Name) \
template<typename Type, typename Ret,typename... FunArgs>\
struct check_##Name{\
    template <typename Inner,class =void>\
    struct has_##Name:false_type{};\
\
    template<typename Inner>\
    struct has_##Name<Inner,Ui::void_t<decltype(Inner::Name)> >:true_type{};\
\
    template <class Inner,  class Retv,class... Args>\
    using function_type = decltype(declval<Inner&>().Name(declval<Args>()...));\
\
    template <typename Inner,class =void>\
    struct has_fun_##Name:false_type{};\
\
    template<typename Inner>\
    struct has_fun_##Name<Inner,Ui::void_t<function_type<Inner,Ret,FunArgs...>>>:is_same<function_type<Inner,Ret,FunArgs...>,Ret>{};\
    using member=has_##Name<Type>;\
    using fun=has_fun_##Name<Type>;\
\
};

template<typename T>
struct type_of_pointer
{
    using value_type=T;
};
template<typename T>
struct type_of_pointer<T*>
{
    using value_type=T;
};
template<typename T>
using type_of_pointer_t  =typename type_of_pointer<T>::value_type;

template<typename... T>
struct STypeCheckHelper;
template <typename T>
inline void typecheckerFun(){
    std::cout<<__PRETTY_FUNCTION__;
}
#define DISPLAY_TYPE(X) typecheckerFun<X>()

#define S_DECLARE_FUNCTION_EXISTS_TESTER(FUNCNAME) \
    template<typename, typename T>\
    struct FUNC_EXISTS_##FUNCNAME{\
        static_assert(\
            std::integral_constant<T, false>::value,\
            "Second template parameter needs to be of function type.");\
    };\
template<typename C, typename Ret, typename... Args>\
struct FUNC_EXISTS_##FUNCNAME<C, Ret(*)(Args...)> {\
private: \
    template<typename T>\
    static constexpr auto check(T*) ->\
            typename std::is_same<\
            decltype( std::declval<T*>()->FUNCNAME( std::declval<Args>()... ) ),Ret\
            >::type;\
    template<typename>\
    static constexpr std::false_type check(...);\
    typedef decltype(check<C>(0)) type;\
public: \
    static constexpr bool value =  type::value;\
};

#define CHECK_FUNCTION(FUNCNAME) FUNC_EXISTS_##FUNCNAME

#define S_DECLARE_MEMBER_EXISTS_TESTER(MEMVAR) \
template <typename T, typename = int> \
struct MEM_EXISTS_##MEMVAR : std::false_type { };\
template <typename T>\
struct MEM_EXISTS_##MEMVAR <T, decltype((void) std::declval<T*>()->MEMVAR, 0)> : std::true_type { };

//#define CHECK_MEM_VARIABLE(MEMVAR) MEM_EXISTS_##MEMVAR
//#define CHECK_MEM_VARIABLE(MEMVAR) has_member_variable(MEMVAR)


template<class F, class... Args>
struct s_is_callable
{

    template<class U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), std::true_type());
    template<class U> static auto test(...) -> decltype(std::false_type());

    static constexpr bool value = decltype(test<F>(0))::value;
    using type = typename decltype(test<F>(0))::type;
};

template<class F, class... Args>
using s_is_callable_t=typename s_is_callable<F,Args...>::type;
template<class F, class... Args>
constexpr bool s_is_callable_v= s_is_callable<F,Args...>::value;

template<typename>
constexpr bool is_string(){return false;}

template<typename>
constexpr bool is_c_style_string(){return false;}

template<>
constexpr bool is_c_style_string<const char*>(){return true;}
template<>
constexpr bool is_c_style_string<char*>(){return true;}
//template<>
//constexpr bool is_c_style_string<uchar*>(){return true;}
//template<>
//constexpr bool is_c_style_string<const uchar*>(){return true;}

//template<>
//constexpr bool is_string<QString>(){return true;}
template<class T>
constexpr bool is_string_type(){ return (is_c_style_string<T>()|| is_string<T>());}

template<bool>
struct check_truth:public std::false_type{};
template<>
struct check_truth<true>:public std::true_type{};


template <typename... T>
struct check_string_types
{
    enum {value = std::__and_<check_truth<is_string_type<T>()>... >::value};
};

template <class T>
inline int s_sizeof(T b);
template<bool B>
using Requires = std::enable_if_t<B, bool>;
namespace Ui {
    template<typename T>
    struct vectoridentity
    {
        template <typename U>
        using vectortype = std::vector<U>;
        using valuetype = typename T::value_type;
        enum {
            value = std::is_same<vectortype<valuetype>,T>::value
        };
    };
    template<typename B>
    inline bool checkall(B b)
    {
           static_assert(std::is_same<bool,B>::value,"expression should return bool");
           return b;
    }
    template<typename B1, typename... B2>
    inline bool checkall(B1 b1 , B2... b2)
    {
        static_assert(std::is_same<bool,B1>::value,"expression should return bool");
        return b1 && checkall(b2...);
    }

    template<typename FunctionBase, template<class... > class Function>
        struct SGenericFunctor
        {
            DEFINE_DETECTOR(clone)
            static_assert (check_clone<FunctionBase,FunctionBase*>::fun::value,"Missing the clone function" );
            using handler_type = typename FunctionBase::handler_type;
            using base_type = FunctionBase;
            base_type* base{nullptr};
            SGenericFunctor(){}
            ~SGenericFunctor(){if(base)delete base;}
             SGenericFunctor(SGenericFunctor&& other):base(other.base){other.base =nullptr;}
            SGenericFunctor(const SGenericFunctor& other){
                if(other.base)
                    base = other.base->clone();
            }
            SGenericFunctor& operator = (const SGenericFunctor& other) {
                SGenericFunctor temp(other);
                std::swap(base,temp.base);
                return *this;
            }
            SGenericFunctor& operator = (SGenericFunctor&& other){
                SGenericFunctor temp(std::move(other));
                std::swap(base,temp.base);
                return *this;
            }
            template<typename T>
            SGenericFunctor(T func):base(new Function<T>(std::move(func))){}
            // SVariant operator()()const{ return (*base)();}
            operator bool()const{return base != nullptr;}
            operator base_type*(){return base;}
            base_type& get()const {return *base;}
        };


        template<typename Head>
        unsigned predicate_to_value_impl(unsigned sofar ,const Head& head){
           return sofar<<1 | head();
        }
        template<typename Head,typename... Tail>
        unsigned predicate_to_value_impl(unsigned sofar,const Head& head, const Tail&... tail){
           sofar = predicate_to_value_impl(sofar,tail...);
           return sofar <<1 | head();
        }

        template<typename... Args>
        unsigned predicate_to_value(const Args&... args){
            unsigned sofar=0;
            return predicate_to_value_impl(sofar,args...);

        }
        constexpr unsigned is_true_at(unsigned index){
            return std::pow(2,index);
        }
        template <typename HeadFunc>
        auto chain(HeadFunc head)
        {
            return [=](auto&&... v){
                return head(std::forward<decltype (v)>(v)...);

            };
        }

        template <typename HeadFunc,typename... TailFunc>
        auto chain(HeadFunc&& head, TailFunc&&... tail)
        {
            return [=](auto&&... v){
                auto r = head(std::forward<decltype (v)>(v)...);
                if(r) return r;
                return chain(tail...)(std::forward<decltype (v)>(v)...);
            };
        }

        template <typename Op,typename HeadFunc>
        auto combine(Op&& ,HeadFunc head)
        {
            return [=](auto&&... v)->auto{
                return head(std::forward<decltype (v)>(v)...);

            };
        }

        template <typename Op,typename HeadFunc,typename... TailFunc>
        auto combine(Op&& op,HeadFunc&& head, TailFunc&&... tail)
        {
            return [=](auto&&... v)->auto{
                auto a = head(std::forward<decltype (v)>(v)...);
                auto b = combine(op,tail...)(std::forward<decltype (v)>(v)...);
                return op(a,b);
            };
        }
    //    template <typename HeadFun,typename TailFunc>
    //    auto operator | (HeadFun head,TailFunc tail){
    //        return chain(head,tail);
    //    }

struct SCopeHandler
{
    ~SCopeHandler(){
        handler();
    }
    void operator=(std::function<void()> h){
        handler=std::move(h);
    }
    std::function<void()> handler;
};
}

#define ON_SCOPE_EXIT(X) Ui::SCopeHandler scope##X;\
                         scope##X = [&]()


#endif // STYPETRAITS_H

