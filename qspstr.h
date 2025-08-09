#ifndef QQSP_QSPSTR_H
#define QQSP_QSPSTR_H

#include <qsp_default.h>

#include <QString>

#include <string>

class QSPStr {
public:
	QSPStr(const QString& s);
	operator QSPString() const;

private:
#ifdef _UNICODE
	using str = std::wstring;
#else
	using str = std::string;
#endif
	str _str;
};

QString fromQSPChars(const QSP_CHAR* str, std::size_t maxSize);

#endif
