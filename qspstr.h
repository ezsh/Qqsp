#ifndef QQSP_QSPSTR_H
#define QQSP_QSPSTR_H

#include <qsp_default.h>
#include <QString>
#include <string>

class QSPStr {
public:
	QSPStr(QString s);
	operator QSPString() const;

private:
#ifdef _UNICODE
	using str = std::wstring;
#else
	using str = std::string;
#endif
	str _str;
};

#endif
