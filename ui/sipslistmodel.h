#ifndef SIPSLISTMDODEL_H
#define SIPSLISTMDODEL_H

#include <QAbstractListModel>

#include "datasource.h"
class SipsListMdodel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ModelItems *listData READ listData WRITE setListData NOTIFY listDataChanged)
public:
    explicit SipsListMdodel(QObject *parent = nullptr);
    enum {
        Done=Qt::UserRole,
        Data
    };
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    ModelItems* listData()const;
    void setListData(ModelItems* data);
    Q_INVOKABLE void handleOnclick(QModelIndex index);
signals:
    void listDataChanged();
private:
    ModelItems* mData{nullptr};
};

#endif // SIPSLISTMDODEL_H
