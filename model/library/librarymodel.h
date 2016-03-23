#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QAbstractTableModel>


class LibraryModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit LibraryModel(QObject *parent = 0);
	virtual ~LibraryModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

#endif // LIBRARYMODEL_H
