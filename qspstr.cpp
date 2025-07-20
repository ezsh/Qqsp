#include "./qspstr.h"

QSPStr::QSPStr(QString s)
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
