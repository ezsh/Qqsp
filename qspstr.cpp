#include "./qspstr.h"

#include <string_view>

namespace {
	QString toQString(const QSP_CHAR* str, qsizetype size)
	{
#ifdef _UNICODE
		return QString::fromWCharArray(str, size);
#else
		return QString::fromLocal8Bit(str, size);
#endif
	}
} // namespace

QSPStr::QSPStr(const QString& s)
#ifdef _UNICODE
	: _str{s.toStdWString()}
#else
	: _str{s.toStdString()}
#endif
{
}

QSPStr::operator QSPString() const
{
	return {const_cast<str::value_type*>(_str.c_str()), const_cast<str::value_type*>(_str.c_str()) + _str.size()};
}

QString fromQSPChars(const QSP_CHAR* str, std::size_t maxSize)
{
	using QSP_string_view = std::basic_string_view<QSP_CHAR>;
	QSP_string_view input{str, maxSize};
	auto nullPos = input.find(QSP_CHAR{0});
	return toQString(input.data(), nullPos == QSP_string_view::npos ? input.size() : nullPos);
}
