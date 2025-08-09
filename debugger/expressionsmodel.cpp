#include "expressionsmodel.h"

#include "../qspstr.h"

#include <qsp_default.h>

#include <QApplication>
#include <QPalette>

#include <array>
#include <utility>

Debugger::ExpressionsModel::ExpressionsModel(QObject* parent)
	: Base{parent}
{
}

QModelIndex Debugger::ExpressionsModel::addExpression(const QString& expression)
{
	beginInsertRows({}, entries_.size(), entries_.size());
	entries_.emplace_back(expression);
	evaluate(entries_.size() - 1);
	endInsertRows();
	return index(static_cast<int>(entries_.size() - 1), 0);
}

bool Debugger::ExpressionsModel::evaluate(qsizetype index)
{
	if (index < 0) {
		return evaluateAll();
	}

	if (index >= entries_.size()) {
		return false;
	}

	Expression& expression{entries_[index]};
	if (expression.expression.isEmpty()) {
		return false;
	}

	static thread_local std::array<QSP_CHAR, 1024> buffer;
	if (QSPCalculateStrExpression(QSPStr(expression.expression), buffer.data(), buffer.size(), QSP_FALSE) != QSP_TRUE) {
		return false;
	}
	QString newValue{fromQSPChars(buffer.data(), buffer.size())};
	expression.changed = expression.value != newValue;
	QModelIndex valueIndex{this->index(index, 1)};
	if (expression.changed) {
		expression.value = std::move(newValue);
		Q_EMIT dataChanged(valueIndex, valueIndex);
	} else {
		Q_EMIT dataChanged(valueIndex, valueIndex, {Qt::ForegroundRole});
	}
	return true;
}

bool Debugger::ExpressionsModel::evaluateAll()
{
	bool result = true;
	for (qsizetype i = 0; i < entries_.size(); ++i) {
		if (!evaluate(i)) {
			result = false;
		}
	}
	return result;
}

const QString& Debugger::ExpressionsModel::expression(int row) const
{
	return entries_.at(row).expression;
}

const QString& Debugger::ExpressionsModel::value(int row) const
{
	return entries_.at(row).value;
}

void Debugger::ExpressionsModel::clear()
{
	beginResetModel();
	entries_.clear();
	endResetModel();
}

int Debugger::ExpressionsModel::rowCount(const QModelIndex& parent) const
{
	return entries_.count();
}

int Debugger::ExpressionsModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

Qt::ItemFlags Debugger::ExpressionsModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags result = QAbstractTableModel::flags(index);
	if (index.column() == 0) {
		result |= Qt::ItemFlag::ItemIsEditable;
	}
	return result;
}

QVariant Debugger::ExpressionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	switch (orientation) {
		case Qt::Orientation::Horizontal:
			switch (role) {
				case Qt::DisplayRole:
					switch (section) {
						case 0: return tr("Expression");
						case 1: return tr("Value");
					}
			}
		default: break;
	}
	return Base::headerData(section, orientation, role);
}

QVariant Debugger::ExpressionsModel::data(const QModelIndex& index, int role) const
{
	const Expression& expression = entries_.at(index.row());
	switch (role) {
		case Qt::DisplayRole: return index.column() == 0 ? expression.expression : expression.value;
		case Qt::EditRole: return index.column() == 0 ? expression.expression : QVariant{};
		case Qt::ToolTipRole: return index.column() == 1 ? expression.value : QVariant{};
		case Qt::ForegroundRole: return index.column() == 1 && expression.changed ? QApplication::palette().accent() : QVariant{};
	}
	return {};
}

bool Debugger::ExpressionsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.column() > 0 || index.row() >= entries_.size()) {
		return false;
	}

	switch (role) {
		case Qt::EditRole:
			entries_[index.row()].expression = value.toString();
			evaluate(index.row());
			return true;
	}
	return false;
}

bool Debugger::ExpressionsModel::insertRows(int row, int count, const QModelIndex& parent)
{
	if (row != entries_.size()) {
		return false;
	}

	beginInsertRows({}, row, row + count - 1);
	for (int i = 0; i < count; ++i) {
		entries_.emplace_back(QString{});
	}
	endInsertRows();
	return true;
}

bool Debugger::ExpressionsModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows({}, row, row + count - 1);
	entries_.remove(row, count);
	endRemoveRows();
	return true;
}

Debugger::ExpressionsModel::Expression::Expression(QString exp)
	: expression{std::move(exp)}
	, value{}
	, changed{false}
{
}
