#ifndef SPROPERTYACCESSOR_H
#define SPROPERTYACCESSOR_H
#include <qglobal.h>
#include "s_type_traits.h"
#include <QVariant>

S_DECLARE_FUNCTION_EXISTS_TESTER(source)
S_DECLARE_FUNCTION_EXISTS_TESTER(property)
S_DECLARE_MEMBER_EXISTS_TESTER(isOrphan)

DEFINE_DETECTOR(isOrphan)
DEFINE_DETECTOR(property)
DEFINE_DETECTOR(source)
template<typename T>
using ophancheck = typename check_isOrphan<T,bool>::member;

template<typename T>
using propertyfunccheck = typename check_property<T,QByteArray>::fun;
template<typename T>
using sourcefunccheck = typename check_source<T,QObject*>::fun;
template<typename T>
struct sourcebinderproperties{
    constexpr static bool value = CHECK_FUNCTION(source)<T,QObject*(*)(void)>::value &&
                                            CHECK_FUNCTION(property)<T,QByteArray(*)(void)>::value &&
                                                ophancheck<T>::value ;

};

template<typename T>
struct QVariantbinderproperties{
    constexpr static bool value = std::is_convertible<T,QVariant>::value;
};

class QObject;
//property converter. A converter which can map property values from one to another.
using PropertyConverter = std::function<QVariant(QVariant)>;
inline QVariant IdentityConverter(QVariant v) {return v;}
//composition of property converter. The propertry from f1 is piped to f2 in order to get it converted by f2.
//this way you can make  complex property conversions by passing it through  series of simpler convertion functions.
inline auto operator |(const PropertyConverter& f1, const PropertyConverter& f2) -> PropertyConverter{
    auto composite = [=](QVariant val)->QVariant{
        return f2(f1(val));
    };
    return composite;
}

//sourceBinder is a concept .This concept is responsible for wrapping source and its property.
//the concept will be used in bindings. Expression binding , formatter binding ,AttributeWrapper and propertybinder
//implements this concept to make themselves bindable.
//isOrphan trait of binding helps the target object to decide on the ownership of sourceBinder.
// mandatory apis and types the sourceBinder should supports
// 1)source - the actual source object
// 2)property- the actual source property
// 3)isOrphan - the traits ,which helps the target to decide on the ownership.
// 4)setOwnerShip- function to be implemented as callback ,which the target uses to take the ownership.
template<typename T>
using sourcebinderconcept_t= typename std::enable_if<
                                sourcebinderproperties<T>::value
                                ,int>::type;
template<typename T>
using QVariantbinderconcept_t = typename std::enable_if<QVariantbinderproperties<T>::value,int>::type;

template<typename T,typename U, typename OP>
struct CompositeBinder;//forward declaration;
template<typename T>
class SAttributeWrapper;//forward declaration

class SAny; //forward delaration
class SourceBinderBase;//forward delaration
class SupposeEvaluator;//forward delaration
template<typename... Exprs>
class ListenerEvaluator;
class SPropertyAccessor
{
    QObject* mTarget;
public:
    struct SPropertyBinder;



    template<typename T>
    struct BaseHolder{
        static_assert(sourcefunccheck<T>::value,
                      "type should implement sourcebinder concept");
        static_assert(propertyfunccheck<T>::value,
                      "type should implement sourcebinder concept");
        T sourceBinder;
        constexpr static bool shouldtakeOwnership() { return T::isOrphan;}

        void handleOwnership(SPropertyBinder& /*owner*/ ,std::false_type) const{ /* do nothing */ }
        void handleOwnership(SPropertyBinder& owner,std::true_type)const { sourceBinder.setOwnerShip(owner);}
        BaseHolder(const T& source):sourceBinder(source){}

        QObject* source()const {return sourceBinder.source();}
        QByteArray property()const {return sourceBinder.property();}
        void setOwnerShip(SPropertyBinder& owner) const{
              using selectedtype =typename std::conditional<shouldtakeOwnership(),int,float>::type;
              handleOwnership(owner,std::is_integral<selectedtype>());
        }
    };

    template<typename T>
    struct ConverterHolder:BaseHolder<T>{
        using RESULTTYPE = typename T::RESULTTYPE;
        static const bool isOrphan=T::isOrphan;
        PropertyConverter convertFunc;
        ConverterHolder(const T& source,PropertyConverter conv):BaseHolder<T>(source),convertFunc(conv){}
        ConverterHolder operator |(PropertyConverter conv){
            return ConverterHolder(std::move(BaseHolder<T>::sourceBinder),convertFunc|conv);
        }
        RESULTTYPE operator()()const {
            QVariant v= convertFunc(BaseHolder<T>::sourceBinder());
            return qvariant_cast<RESULTTYPE>(v);
        }
    };

    template<typename T>
    struct RelationalOperatorHolder:BaseHolder<T>{
        using RESULTTYPE = bool;
        static const bool isOrphan=T::isOrphan;
        PropertyConverter convertFunc;
        RelationalOperatorHolder(const T& source,PropertyConverter conv):BaseHolder<T>(std::move(source)),convertFunc(conv){}
        RelationalOperatorHolder operator |(PropertyConverter conv){
            return RelationalOperatorHolder(std::move(BaseHolder<T>::sourceBinder),convertFunc|conv);
        }
        RESULTTYPE operator()()const {
            QVariant v= convertFunc(BaseHolder<T>::sourceBinder());
            return qvariant_cast<RESULTTYPE>(v);
        }
        operator RESULTTYPE() const{
            return operator ()();
        }
        using BaseHolder<T>::source;

    };
    //to catch binding to orphan temporary SOject types
    struct SPropertyBinderOrphan
    {
        using RESULTTYPE = QVariant;
        static const bool isOrphan=true;
        SPropertyBinderOrphan(QObject* t , const QByteArray& nm):src(t),prop(nm){}
        QObject* source()const {return src;}
        QByteArray property()const {return prop;}
        inline void setOwnerShip(SPropertyBinder& owner) const;//defined in spropertyaccessorimpl.h
        inline RESULTTYPE operator()()const;//defined in spropertyaccessorimpl.h
        QObject* src{nullptr};
        QByteArray prop;
        PropertyConverter convertFunc{IdentityConverter};
    };
    struct SPropertyBinder
    {
       static const bool isOrphan=false;
       template<typename U>
       SPropertyBinder& operator=(U value);
       using RESULTTYPE = QVariant;

       template<typename U>
       SPropertyBinder& operator=( const ConverterHolder<U>& val)
       {
           applyBindingAssignment(val);
           return *this;
       }
       template<typename U>
       SPropertyBinder& operator=( const RelationalOperatorHolder<U>& val)
       {
           applyBindingAssignment(val);
           return *this;
       }
        SPropertyBinder& operator=(const SPropertyBinderOrphan& val)
        {
            applyBindingAssignment(val);
            return *this;
        }
       template<typename T,typename U,typename OP>
       SPropertyBinder& operator=( const CompositeBinder<T,U,OP>& val);//defined in scompositebinder.h
       template<typename T>
       SPropertyBinder& operator=(const SAttributeWrapper<T>& attribAccessor);//defined in sattributeutilites.h
       SPropertyBinder& operator=(const SAny& any);//defined in spropertyaccessorimpl.h
       SPropertyBinder& operator=(const SupposeEvaluator& any);//defined in spropertyaccessorimpl.h

       template<typename... T>
       SPropertyBinder& operator=(const ListenerEvaluator<T...>& listener);//defined in sattibuteutilities.h

       template<typename T>
       void applyBindingAssignment(const ConverterHolder<T>& sourceholder){
           addPropertyBinding(sourceholder.source(),sourceholder.property(),
                               sourceholder.convertFunc
                              );
           sourceholder.setOwnerShip(*this);
       }
       template<typename T>
       void applyBindingAssignment( const RelationalOperatorHolder<T>& sourceholder){
           addPropertyBinding(sourceholder.source(),sourceholder.property(),
                               sourceholder.convertFunc
                              );
           sourceholder.setOwnerShip(*this);
       }

       void applyBindingAssignment( const SPropertyBinderOrphan& sourceholder){
           addPropertyBinding(sourceholder.source(),sourceholder.property(),
                               sourceholder.convertFunc
                              );
           sourceholder.setOwnerShip(*this);
       }
       SPropertyBinder& operator = (SPropertyBinder other)
       {
           observe(other);
           return *this;
       }
       void observe(const SPropertyBinder& accesor);

       void addPropertyBinding(QObject* source,const QByteArray& propname, PropertyConverter convertFunc);
       QObject* target()const;
       QObject* source()const{return target();}
       QByteArray property()const ;
       operator QVariant();
       QVariant operator ()()const ;
       static inline SPropertyBinder makeBinder(QObject* t , const QByteArray& nm){
           return SPropertyBinder( t ,  nm);
       }
    private:
       SPropertyBinder(QObject* t , const QByteArray& nm):name(nm),mTarget(t){}
       QByteArray name;
       QObject* mTarget;
       friend class SPropertyAccessor;
       friend class SourceBinderBase;
    };


    SPropertyAccessor(QObject* t):mTarget(t){}
    SPropertyBinder operator[](const QByteArray& str){
        return SPropertyBinder(mTarget,str);
    }
    SPropertyBinderOrphan make_orphan_binder(const QByteArray& str){
        return SPropertyBinderOrphan(mTarget,str);
    }

};

//inline QString s_normalizeArg(const SPropertyAccessor::SPropertyBinder& arg){
//   return fromQVariantToString(arg());
//}
using SPropertyBinder = SPropertyAccessor::SPropertyBinder;
using SPropertyBinderOrphan = SPropertyAccessor::SPropertyBinderOrphan;

template<typename T>
using BaseHolder = SPropertyAccessor::BaseHolder<T>;
template<typename T>
using ConverterHolder = SPropertyAccessor::ConverterHolder<T>;
template<typename T>
using RelOpHolder = SPropertyAccessor::RelationalOperatorHolder<T>;

#endif // SPROPERTYACCESSOR_H
