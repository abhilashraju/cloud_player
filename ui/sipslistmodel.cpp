#include "sipslistmodel.h"
#include "datasource.h"
SipsListMdodel::SipsListMdodel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SipsListMdodel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || mData==nullptr)
        return 0;

    return mData->size();
}

QVariant SipsListMdodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    return mData->data(index,role);

}

bool SipsListMdodel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        mData->setData(index,value,role);
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags SipsListMdodel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> SipsListMdodel::roleNames() const
{
    return  mData->roleNames();
}

ModelItems *SipsListMdodel::listData() const
{
    return mData;
}

void SipsListMdodel::setListData(ModelItems *data)
{
    beginResetModel();
    if(mData){
        mData->disconnect(this);
    }
    mData=data;
    if(mData){
        connect(mData,&ModelItems::beginAppendItem,[=](){
            auto index= rowCount();
            beginInsertRows(QModelIndex(),index,index);
        });
        connect(mData,&ModelItems::endAppendItem,[=](){
           endInsertRows();
        });
        connect(mData,&ModelItems::beginRemoveItem,[=](int index){
            beginRemoveRows(QModelIndex(),index,index);
        });
        connect(mData,&ModelItems::endRemoveItem,[=](){
            endRemoveRows();
        });
    }
    endResetModel();

}
void SipsListMdodel::handleOnclick(QModelIndex index)
{
    mData->handleOnclick(index);
}
