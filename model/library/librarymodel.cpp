#include "librarymodel.h"


LibraryModel::LibraryModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

LibraryModel::~LibraryModel()
{
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
	// TODO
	return 0;
}

int LibraryModel::columnCount(const QModelIndex &parent) const
{
	// TODO
	return 0;
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
	// TODO
	return QVariant();
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// TODO
	return QVariant();
}
