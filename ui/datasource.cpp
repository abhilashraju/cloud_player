#include "datasource.h"

ModelItems::ModelItems(QObject *parent) : QObject(parent)
{

}

void ModelItems::append(DataSource* data)
{
    emit beginAppendItem();
    mdata.push_back(std::unique_ptr<DataSource>{data});
    emit endAppendItem();

}

void ModelItems::remove(int index)
{
    emit beginRemoveItem(index);
    mdata.erase(std::begin(mdata)+index);
    emit endRemoveItem();
}
 QVariant ModelItems::data(const QModelIndex &index, int role)const
 {
     if(index.row()<mdata.size() && index.row()>=0){
         return mdata.at(index.row())->data(role);
     }
     return QVariant();
 }
 bool ModelItems::setData(const QModelIndex &index, const QVariant &value,
             int role )
 {
     if(index.row()<mdata.size() && index.row()>=0){
         return mdata.at(index.row())->setData(value,role);
     }
     return false;
 }
 int ModelItems::rowCount(const QModelIndex & ) const
 {
     return mdata.size();
 }
 QHash<int, QByteArray> ModelItems::roleNames() const
 {
     if(mdata.size()){
         return mdata.at(0)->roleNames();
     }
     return QHash<int, QByteArray>();
 }
void ModelItems::handleOnclick(QModelIndex index)
{
    if(mdata.size()){
        return mdata.at(index.row())->handleOnClick(index);
    }
}
