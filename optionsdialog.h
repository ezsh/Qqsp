#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QColor>
#include <QDialog>
#include <QFont>
#include <QString>

namespace Ui {
	class OptionsDialog;
}

class OptionsDialog: public QDialog {
	Q_OBJECT

public:
	explicit OptionsDialog(QWidget* parent = 0);
	~OptionsDialog();

	// QDialog interface
	void accept() override;

private slots:
	void on_pushButton_font_clicked();
	void on_pushButton_backColor_clicked();
	void on_pushButton_fontColor_clicked();
	void on_pushButton_linkColor_clicked();

private:
	Ui::OptionsDialog* ui;
	QFont m_font;
	QColor m_backColor;
	QColor m_linkColor;
	QColor m_fontColor;
	QString langid;
};

#endif // OPTIONSDIALOG_H
