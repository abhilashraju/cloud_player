#pragma once
#include <QAbstractItemModel>
#include "rangealgorithm.h"

template<typename DataSource,typename T>
struct generatemodel
{

    static constexpr size_t  tuplesize = boost::tuples::length<T>::value;
    static_assert(tuplesize >0 ,"data parameters has no fields");


    DataSource* operator()( const T& data) const
    {

        DataSource* modelItem= new DataSource;
        BoostTupleVisitor<T,ModelVisitfun<DataSource>,tuplesize>::visit(data,ModelVisitfun<DataSource>(modelItem));
        return modelItem;
    }

};
template<typename DataSource,typename... types >
struct generatemodel<DataSource,SValuePairs<types...>>
{
    using T = SValuePairs<types...>;
    static constexpr size_t  tuplesize = std::tuple_size<T>::value;
    static_assert(tuplesize >0 ,"data parameters has no fields");


    DataSource* operator()( const T& data) const
    {

        DataSource* modelItem= new DataSource;
        StdTupleVisitor<T,ModelVisitfun<DataSource>,tuplesize>::visit(data,ModelVisitfun<DataSource>(modelItem));
        return modelItem;
    }
    template<typename... U>
    DataSource* operator()(std::tuple<U...>& valdata) const
    {

        DataSource* modelItem= new DataSource;
        StdTupleVisitor<decltype(valdata),ModelVisitfun<DataSource>,tuplesize>::visit(valdata,ModelVisitfun<DataSource>(modelItem));
        return modelItem;
    }

};


namespace Ui {
    template<typename DataSource,typename ModelSource>
    struct populateModel{
        using ModelDataSource=DataSource;
        using Model=ModelSource;
        template<typename T>
        inline void operator()(const T& r,QAbstractItemModel* model){

            boost::transform(r,std::back_inserter(*model),
                             generatemodel<DataSource, typename T::value_type>());
        }
    };

}


template<typename T,typename Populate>
class SValuePairModel:public Populate::Model
{

//    static_assert(Ui::vectoridentity<decltype(T()())>::value,"model accept only std::vector types datasource");
  public:
    SValuePairModel(QObject* p =0)
    {
        Populate populator;
        populator(T()(),this);
    }
protected:
//    bool handleSetProperty(const QString& name, const QVariant& value){
//        if(name == "refresh"){
//            reset();
//            Populate populator;
//            populator(T()(),this);
//            return true;
//        }
//        return SItemModel::handleSetProperty(name,value);

//    }

};

template<typename ModelItems,typename Range>
inline ModelItems* make_modelItems(const Range& r){
    ModelItems* items = new ModelItems();
    boost::transform(r,std::back_inserter(*items),generatemodel<typename ModelItems::data_source_type, typename Range::value_type>());
}
