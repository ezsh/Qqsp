#include "mainwindow.h"

#include "callbacks_gui.h"
#include "comtools.h"
#include "debuglogwindow.h"
#include "optionsdialog.h"
#include "qspstr.h"
#include "webinspector.h"
#include "webinspectorwindow.h"

#include <QApplication>
#include <QAudioOutput>
#include <QCursor>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QIcon>
#include <QInputDialog>
#include <QLocale>
#include <QMessageBox>
#include <QMimeData>
#include <QPalette>
#include <QScreen>
#include <QSettings>
#include <QThread>

#ifdef Q_OS_ANDROID
#	include "androidfiledialog.h"

#	include <QStandardPaths>
#endif

#include "ui_mainwindow.h"

#include <qqsp-config.h>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, _ui(new Ui::MainWindow())
{
	_ui->setupUi(this);

	resize(600, 450);
	setWindowTitle(QSP_LOGO);
	setObjectName(QStringLiteral("MainWindow"));

#ifdef QT_WEBENGINEWIDGETS_LIB
	//    qwuri = new QspWebEngineUrlRequestInterceptor();
	//    QWebEngineProfile::defaultProfile()->setRequestInterceptor(qwuri);
	//    QspWebEngineUrlSchemeHandler *qweush = new QspWebEngineUrlSchemeHandler();
	//    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(QByteArray("qsp"),qweush);
#else
	_ui->actionWebInspector->setVisible(false);
#endif

	m_palette = palette();

	// mainStatusBar = new QStatusBar(this);
	// mainStatusBar->setObjectName(QStringLiteral("mainStatusBar"));
	// setStatusBar(mainStatusBar);

	// Set QMainWindow in the center of desktop
	// QRect rect = geometry();
	// rect.moveCenter(QApplication::desktop()->availableGeometry().center());
	// setGeometry(rect);

	// mainStatusBar->setVisible(false);
	_ui->mainToolBar->setVisible(false);

	m_timer = new QTimer(this);
	m_timer->setObjectName(QStringLiteral("m_timer"));
	connect(m_timer, &QTimer::timeout, this, &MainWindow::OnTimer);
	m_savedGamePath.clear();
	m_isQuit = false;
	m_keyPressedWhileDisabled = false;
	m_isGameOpened = false;
	showPlainText = false;

	// setCentralWidget(_mainDescWidget);

	m_linkColor = palette().color(QPalette::Link);
	m_fontColor = palette().color(QPalette::Text);
	m_backColor = QColor(224, 224, 224);
	m_isUseBackColor = false;
	m_isUseLinkColor = false;
	m_isUseFontColor = false;
	m_defaultBackColor = m_backColor;
	m_defaultLinkColor = m_linkColor;
	m_defaultFontColor = m_fontColor;

	m_font = QFont("Sans", 12, QFont::Normal);
	m_font.setStyle(QFont::StyleNormal);
	m_font.setStyleHint(QFont::SansSerif);
	m_defaultFont = m_font;
	m_isUseFontSize = false;
	m_isUseFont = false;
	m_fontSize = 12;

	showCaptions = true;

	m_isShowHotkeys = false;

	m_volume = 1.0f;

	disableVideo = false;
	m_videoFix = false;

	perGameConfig = true;
	autostartLastGame = false;

	m_isAllowHTML5Extras = true;

	langid = QObject::tr("__LANGID__");
	if (langid == QStringLiteral("__LANGID__")) {
		langid = QLocale::system().name();
	}

	setupDockWindows();

	_debugLogWindow = new DebugLogWindow(_ui->actionDebug_log, this);

	ApplyBackColor(m_backColor);
	ApplyFontColor(m_fontColor);
	ApplyLinkColor(m_linkColor);
	ApplyFont(m_font, 0, 0);

	QFileInfo settingsFile(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
	if (settingsFile.exists() && settingsFile.isFile()) {
		LoadSettings(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
	} else {
		LoadSettings();
	}
	CreateMenuBar();

	m_menu = new QMenu(this);
	m_menu->setObjectName(QStringLiteral("m_menu"));
	connect(m_menu, &QMenu::triggered, this, &MainWindow::OnMenu);

	QSPInit();
	QSPCallBacks::Init(this, _debugLogWindow);
	QSPCallBacks::SetAllowHTML5Extras(m_isAllowHTML5Extras);
	SetOverallVolume(m_volume);

	QFileInfo gameFile(QApplication::applicationDirPath() + "/standalone_content/" + QSP_GAME);
	if (gameFile.exists() && gameFile.isFile()) {
		OpenGameFile(QApplication::applicationDirPath() + "/standalone_content/" + QSP_GAME);
	} else {
		if (autostartLastGame) {
			OpenGameFile(lastGame);
		}
	}
}

MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::EnableControls(bool status, bool isExtended)
{
	if (isExtended) {
		_ui->_fileMenu->setEnabled(status); // TODO: ???
	}
	_ui->_fileMenu->setEnabled(status); // TODO: ???
	_ui->_gameMenu->setEnabled(status);
	_ui->_settingsMenu->setEnabled(status);
	_objectsListBox->setEnabled(status);
	_actionsListBox->setEnabled(status);
	_inputTextBox->setEnabled(status);
	m_isProcessEvents = status;
	m_keyPressedWhileDisabled = false;
}

void MainWindow::ApplyParams()
{
	QSPVariant val;
	QColor setBackColor, setFontColor, setLinkColor;
	setPalette(m_palette);
	// --------------
	if (!m_isUseBackColor) {
		if (QSPGetVarValue(L"BCOLOR"_qsp, 0, &val)) {
			assert(QSP_ISNUM(val.Type));
			if (QSP_NUM(val) == 0) {
				setBackColor = m_defaultBackColor;
			} else {
				setBackColor = QSPTools::wxtoQColor(QSP_NUM(val));
			}
		} else {
			setBackColor = m_defaultBackColor;
		}
	} else {
		setBackColor = m_settingsBackColor;
	}
	ApplyBackColor(setBackColor);
	// --------------
	if (!m_isUseFontColor) {
		if (QSPGetVarValue(L"FCOLOR"_qsp, 0, &val)) {
			assert(QSP_ISNUM(val.Type));
			if (QSP_NUM(val) == 0) {
				setFontColor = m_defaultFontColor;
			} else {
				setFontColor = QSPTools::wxtoQColor(QSP_NUM(val));
			}
		} else {
			setFontColor = m_defaultFontColor;
		}
	} else {
		setFontColor = m_settingsFontColor;
	}
	ApplyFontColor(setFontColor);
	// --------------
	if (!m_isUseLinkColor) {
		if (QSPGetVarValue(L"LCOLOR"_qsp, 0, &val)) {
			assert(QSP_ISNUM(val.Type));
			if (QSP_NUM(val) == 0) {
				setLinkColor = m_defaultLinkColor;
			} else {
				setLinkColor = QSPTools::wxtoQColor(QSP_NUM(val));
			}
		} else {
			setLinkColor = m_defaultLinkColor;
		}
	} else {
		setLinkColor = m_settingsLinkColor;
	}
	ApplyLinkColor(setLinkColor);
	// --------------
	QFont new_font = m_defaultFont;
	int fontType = 0;
	int sizeType = 0;
	if (!m_isUseFont) {
		if (QSPGetVarValue(L"FNAME"_qsp, 0, &val)) {
			assert(QSP_ISSTR(val.Type));
			if (QSP_STR(val).Str) {
				if (!QSPTools::qspStrToQt(QSP_STR(val)).isEmpty()) {
					new_font.setFamily(QSPTools::qspStrToQt(QSP_STR(val)));
					fontType = 1;
				}
			}
		}
		if (!m_isUseFontSize) {
			if (QSPGetVarValue(L"FSIZE"_qsp, 0, &val)) {
				assert(QSP_ISNUM(val.Type));
				if (QSP_NUM(val) != 0) {
					new_font.setPointSize(QSP_NUM(val));
					sizeType = 1;
				}
			}
		} else {
			new_font.setPointSize(m_fontSize);
			sizeType = 2;
		}
	} else {
		new_font = m_font;
		fontType = 2;
		if (m_isUseFontSize) {
			new_font.setPointSize(m_fontSize);
			sizeType = 2;
		}
	}
	ApplyFont(new_font, fontType, sizeType);
}

void MainWindow::DeleteMenu()
{
	m_menu->clear();
	m_menuItemId = 0;
}

void MainWindow::AddMenuItem(const QString& name, const QString& imgPath)
{
	if (name == QString("-")) {
		m_menu->addSeparator();
	} else {
		bool pixmap_ok = false;
		QPixmap itemPixmap;
		QFileInfo file(m_path + imgPath);
		QString itemPath(file.absoluteFilePath());
		if (file.exists() && file.isFile()) {
			if (itemPixmap.load(itemPath)) {
				pixmap_ok = true;
			}
		}
		QAction* action;
		if (pixmap_ok) {
			action = m_menu->addAction(QIcon(itemPixmap), name);
			// m_menu->addAction(QIcon(itemPixmap), name, this, SLOT(OnMenu(bool)));
		} else {
			action = m_menu->addAction(name);
			// m_menu->addAction(name, this, SLOT(OnMenu(bool)));
		}
		action->setData(m_menuItemId);
	}
	m_menuItemId++;
}

int MainWindow::ShowMenu()
{
	m_menuIndex = -1;
	m_menu->exec(QCursor::pos());
	return m_menuIndex;
}

void MainWindow::UpdateGamePath(const QString& path)
{
	QString new_path = path;
	if (!new_path.endsWith("/")) {
		new_path += "/";
	}
	m_path = new_path;
	_mainDescTextBox->SetGamePath(new_path);
	_descTextBox->SetGamePath(new_path);
	_actionsListBox->SetGamePath(new_path);
	_objectsListBox->SetGamePath(new_path);
	m_imgView->SetGamePath(new_path);
}

void MainWindow::ShowError()
{
	bool oldIsProcessEvents;
	QString errorMessage;
	if (m_isQuit) {
		return;
	}
	QSPErrorInfo error = QSPGetLastErrorData();
	QString desc = QSPTools::qspStrToQt(error.ErrorDesc);
	if (error.LocName.Str) {
		errorMessage = QString("Location: %1\nArea: %2\nLine: %3\nCode: %4\nDesc: %5")
		                   .arg(QSPTools::qspStrToQt(error.LocName))
		                   .arg(error.ActIndex < 0 ? QString("on visit") : QString("on action"))
		                   .arg(error.IntLineNum)
		                   .arg(error.ErrorNum)
		                   .arg(desc);
	} else {
		errorMessage = QString("Code: %1\nDesc: %2").arg(error.ErrorNum).arg(desc);
	}
	QMessageBox dialog(QMessageBox::Critical, tr("Error"), errorMessage, QMessageBox::Ok, this);
	oldIsProcessEvents = m_isProcessEvents;
	m_isProcessEvents = false;
	dialog.exec();
	m_isProcessEvents = oldIsProcessEvents;
	if (m_isGameOpened) {
		QSPCallBacks::RefreshInt(QSP_FALSE, QSP_FALSE);
	}
}

QDockWidget* MainWindow::GetVarsDock() const
{
	return _ui->dockWidgetAdditional;
}

QDockWidget* MainWindow::GetInputDock() const
{
	return _ui->dockWidgetInput;
}

QDockWidget* MainWindow::GetActionsDock() const
{
	return _ui->dockWidgetActions;
}

QDockWidget* MainWindow::GetObjectsDock() const
{
	return _ui->dockWidgetObjects;
}

QDockWidget* MainWindow::GetImageDock() const
{
	return _ui->dockWidgetImage;
}

QMenu* MainWindow::GetGameMenu() const
{
	return _ui->_gameMenu;
}

void MainWindow::SetShowPlainText(bool isPlain)
{
	showPlainText = isPlain;
	_mainDescTextBox->SetShowPlainText(showPlainText);
	_descTextBox->SetShowPlainText(showPlainText);
	_actionsListBox->SetShowPlainText(showPlainText);
	_objectsListBox->SetShowPlainText(showPlainText);
}

void MainWindow::RefreshUI()
{
	_mainDescTextBox->RefreshUI();
	_objectsListBox->RefreshUI();
	_actionsListBox->RefreshUI();
	_descTextBox->RefreshUI();
	// m_input->Refresh();
	m_imgView->RefreshUI();
}

void MainWindow::ApplyFont(const QFont& new_font, int fontType, int sizeType)
{
	m_font = new_font;
	_mainDescTextBox->SetTextFont(new_font);
	_mainDescTextBox->SetFontType(fontType);
	_mainDescTextBox->SetSizeType(sizeType);
	_descTextBox->SetTextFont(new_font);
	_descTextBox->SetFontType(fontType);
	_descTextBox->SetSizeType(sizeType);
	_objectsListBox->SetTextFont(new_font);
	_actionsListBox->SetTextFont(new_font);
}

bool MainWindow::ApplyFontColor(const QColor& color)
{
	m_fontColor = color;
	_mainDescTextBox->SetForegroundColor(color);
	_descTextBox->SetForegroundColor(color);
	_objectsListBox->SetForegroundColor(color);
	_actionsListBox->SetForegroundColor(color);
	return false;
}

bool MainWindow::ApplyBackColor(const QColor& color)
{
	m_backColor = color;
	QPalette p = palette();
	p.setColor(QPalette::Base, color);
	setPalette(p);
	_mainDescTextBox->SetBackgroundColor(color);
	_descTextBox->SetBackgroundColor(color);
	_objectsListBox->SetBackgroundColor(color);
	_actionsListBox->SetBackgroundColor(color);
	m_imgView->SetBackgroundColor(color);
	return false;
}

bool MainWindow::ApplyLinkColor(const QColor& color)
{
	m_linkColor = color;
	_mainDescTextBox->SetLinkColor(color);
	_descTextBox->SetLinkColor(color);
	_objectsListBox->SetLinkColor(color);
	_actionsListBox->SetLinkColor(color);
	return false;
}

void MainWindow::SetOverallVolume(float new_volume)
{
	QSPCallBacks::SetOverallVolume(new_volume);
	m_volume = new_volume;
}

void MainWindow::SetDisableVideo(bool isDisableVideo)
{
	disableVideo = isDisableVideo;
#ifndef QT_WEBENGINEWIDGETS_LIB
	_mainDescTextBox->SetDisableVideo(disableVideo);
	_descTextBox->SetDisableVideo(disableVideo);
#endif
}

void MainWindow::SetVideoFix(bool isFix)
{
	m_videoFix = isFix;
#ifdef QT_WEBENGINEWIDGETS_LIB
	_mainDescTextBox->SetVideoFix(m_videoFix);
	_descTextBox->SetVideoFix(m_videoFix);
#endif
}

void MainWindow::SetAllowHTML5Extras(bool HTML5Extras)
{
	m_isAllowHTML5Extras = HTML5Extras;
	QSPCallBacks::SetAllowHTML5Extras(m_isAllowHTML5Extras);
}

void MainWindow::SetUseCaseInsensitiveFilePath(bool CaseInsensitiveFilePath)
{
	QSPTools::useCaseInsensitiveFilePath = CaseInsensitiveFilePath;
}

bool MainWindow::GetUseCaseInsensitiveFilePath()
{
	return QSPTools::useCaseInsensitiveFilePath;
}

const QString& MainWindow::gameFilePath() const
{
	return m_gameFile;
}

void MainWindow::LoadSettings(QString filePath)
{
	QSettings* settings;
	if (filePath.isEmpty()) {
		settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
	} else {
		settings = new QSettings(filePath, QSettings::IniFormat);
	}

	restoreGeometry(settings->value("mainWindow/geometry").toByteArray());
	if (isMaximized()) {
		setGeometry(QApplication::screenAt(geometry().center())->availableGeometry());
	}
	restoreState(settings->value("mainWindow/windowState").toByteArray());

	if (settings->value("mainWindow/isMaximized", isMaximized()).toBool()) {
		showMaximized();
	}
	if (settings->value("mainWindow/isFullScreen", isFullScreen()).toBool()) {
		showFullScreen();
	}

	OnToggleCaptions(settings->value("mainWindow/showCaptions", showCaptions).toBool());

	SetShowPlainText(settings->value("application/isShowPlainText", showPlainText).toBool());

	SetLastPath(settings->value("application/lastPath", GetLastPath()).toString());
	perGameConfig = settings->value("application/perGameConfig", perGameConfig).toBool();

	m_isUseFontSize = settings->value("application/isUseFontSize", m_isUseFontSize).toBool();
	m_fontSize = settings->value("application/fontSize", m_fontSize).toInt();
	m_isUseFont = settings->value("application/isUseFont", m_isUseFont).toBool();
	if (m_isUseFont) {
		ApplyFont(qvariant_cast<QFont>(settings->value("application/font", m_font)), 2, 2);
	}

	m_isUseBackColor = settings->value("application/isUseBackColor", m_isUseBackColor).toBool();
	m_isUseLinkColor = settings->value("application/isUseLinkColor", m_isUseLinkColor).toBool();
	m_isUseFontColor = settings->value("application/isUseFontColor", m_isUseFontColor).toBool();
	if (m_isUseBackColor) {
		ApplyBackColor(qvariant_cast<QColor>(settings->value("application/backColor", m_backColor)));
	}
	if (m_isUseLinkColor) {
		ApplyLinkColor(qvariant_cast<QColor>(settings->value("application/linkColor", m_linkColor)));
	}
	if (m_isUseFontColor) {
		ApplyFontColor(qvariant_cast<QColor>(settings->value("application/fontColor", m_fontColor)));
	}
	m_settingsBackColor = qvariant_cast<QColor>(settings->value("application/backColor", m_backColor));
	m_settingsLinkColor = qvariant_cast<QColor>(settings->value("application/linkColor", m_linkColor));
	m_settingsFontColor = qvariant_cast<QColor>(settings->value("application/fontColor", m_fontColor));

	disableVideo = settings->value("application/disableVideo", disableVideo).toBool();
	SetDisableVideo(disableVideo);
	m_videoFix = settings->value("application/videoFix", m_videoFix).toBool();
	SetVideoFix(m_videoFix);

	lastGame = settings->value("application/lastGame", lastGame).toString();
	autostartLastGame = settings->value("application/autostartLastGame", autostartLastGame).toBool();

	m_volume = settings->value("application/volume", m_volume).toFloat();
	SetOverallVolume(m_volume);

	m_isShowHotkeys = settings->value("application/isShowHotkeys", m_isShowHotkeys).toBool();

	m_isAllowHTML5Extras = settings->value("application/isAllowHTML5Extras", m_isAllowHTML5Extras).toBool();

	QSPTools::useCaseInsensitiveFilePath = settings->value("application/useCaseInsensitiveFilePath", QSPTools::useCaseInsensitiveFilePath).toBool();

	langid = settings->value("application/language", langid).toString();

	RefreshUI();
	delete settings;
}

void MainWindow::SaveSettings(QString filePath)
{
	QSettings* settings;
	if (filePath.isEmpty()) {
		settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName(), this);
	} else {
		settings = new QSettings(filePath, QSettings::IniFormat);
	}

	bool maximized = isMaximized();

	bool fullscreen = isFullScreen();

	settings->setValue("mainWindow/geometry", saveGeometry());
	settings->setValue("mainWindow/windowState", saveState());
	settings->setValue("mainWindow/isMaximized", maximized);
	settings->setValue("mainWindow/isFullScreen", fullscreen);
	settings->setValue("mainWindow/showCaptions", showCaptions);

	settings->setValue("application/isShowPlainText", showPlainText);

	settings->setValue("application/lastPath", lastPath);
	settings->setValue("application/perGameConfig", perGameConfig);

	settings->setValue("application/isUseFontSize", m_isUseFontSize);
	settings->setValue("application/fontSize", m_fontSize);
	settings->setValue("application/isUseFont", m_isUseFont);
	settings->setValue("application/font", m_font);

	settings->setValue("application/isUseBackColor", m_isUseBackColor);
	settings->setValue("application/isUseLinkColor", m_isUseLinkColor);
	settings->setValue("application/isUseFontColor", m_isUseFontColor);
	settings->setValue("application/backColor", m_settingsBackColor);
	settings->setValue("application/linkColor", m_settingsLinkColor);
	settings->setValue("application/fontColor", m_settingsFontColor);

	settings->setValue("application/disableVideo", disableVideo);
	settings->setValue("application/videoFix", m_videoFix);

	settings->setValue("application/lastGame", lastGame);
	settings->setValue("application/autostartLastGame", autostartLastGame);

	settings->setValue("application/volume", m_volume);

	settings->setValue("application/isShowHotkeys", m_isShowHotkeys);

	settings->setValue("application/isAllowHTML5Extras", m_isAllowHTML5Extras);

	settings->setValue("application/useCaseInsensitiveFilePath", QSPTools::useCaseInsensitiveFilePath);

	settings->setValue("application/language", langid);

	settings->sync();

	delete settings;
}

void MainWindow::CreateMenuBar()
{
	// Open item
	connect(_ui->actionOpen_game, &QAction::triggered, this, &ThisType::OnOpenGame);
	// New game item
	connect(_ui->actionRestart_game, &QAction::triggered, this, &ThisType::OnRestartGame);
	// Exit item
	connect(_ui->actionExit, &QAction::triggered, this, &ThisType::close);
	// Open saved game item
	connect(_ui->actionOpen_saved_game, &QAction::triggered, this, &ThisType::OnOpenSavedGame);
	// Save game item
	connect(_ui->actionSave_game, &QAction::triggered, this, &ThisType::OnSaveGame);
	// Open quicksave item
	connect(_ui->actionQuick_load, &QAction::triggered, this, &ThisType::OnOpenQuickSavedGame);
	// Quicksave item
	connect(_ui->actionQuick_save, &QAction::triggered, this, &ThisType::OnQuickSaveGame);
	// Objects item
	QAction* action;
	QAction* before = _ui->_showHideMenu->insertSeparator(_ui->actionCaptions);

	action = GetObjectsDock()->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_1));
	_ui->_showHideMenu->insertAction(before, action);

	// Actions item
	action = GetActionsDock()->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_2));
	_ui->_showHideMenu->insertAction(before, action);

	// Additional desc item
	action = GetVarsDock()->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_3));
	_ui->_showHideMenu->insertAction(before, action);

	// Input area item
	action = GetInputDock()->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_4));
	_ui->_showHideMenu->insertAction(before, action);

	// Main desc item
	action = _ui->dockWidgetMainDesc->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_5));
	_ui->_showHideMenu->insertAction(before, action);

	// Image item
	action = GetImageDock()->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_6));
	_ui->_showHideMenu->insertAction(before, action);

	// Captions item
	_ui->actionCaptions->setChecked(GetObjectsDock()->titleBarWidget() == nullptr);
	connect(_ui->actionCaptions, &QAction::triggered, this, &ThisType::OnToggleCaptions);

	// ToolBar
	action = _ui->mainToolBar->toggleViewAction();
	action->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
	_ui->_showHideMenu->addAction(action);

	// TODO: MenuBar
	//  MenuBar
	// action = _showHideMenu->addAction(tr("MenuBar"));
	// action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_7));
	// action->setCheckable(true);
	// if(mainMenuBar->isVisible())
	//     action->setChecked(true);
	// else
	//     action->setChecked(false);
	// connect(action, SIGNAL(toggled(bool)), this, SLOT(OnToggleMenuBar(bool)));

	// Hotkeys for actions item
	_ui->actionHotkeys_for_actions->setChecked(m_isShowHotkeys);
	connect(_ui->actionHotkeys_for_actions, &QAction::triggered, this, &ThisType::OnToggleHotkeys);

	// Window / Fullscreen mode item
	connect(_ui->actionWindow_Fullscreen_mode, &QAction::triggered, this, &ThisType::OnToggleWinMode);

	// Display HTML code as plain text
	connect(_ui->actionDisplay_HTML_code_as_plain_text, &QAction::triggered, this, &ThisType::OnToggleShowPlainText);
	_ui->actionDisplay_HTML_code_as_plain_text->setChecked(showPlainText);
	//    _settingsMenu->addAction(tr("Display HTML code as plain text"),
	//        this, SLOT(OnToggleShowPlainText()), QKeySequence(Qt::ALT + Qt::Key_D))->setCheckable(true);

	// Options item
	connect(_ui->actionOptions, &QAction::triggered, this, &ThisType::OnOptions);
	// mainToolBar->addAction(action);
	//------------------------------------------------------------------
	// mainToolBar->addSeparator();
	//  Help menu
	//  About item
	connect(_ui->actionAbout, &QAction::triggered, this, &ThisType::OnAbout);
#ifndef QT_WEBENGINEWIDGETS_LIB
	_ui->actionWebInspector->setVisible(false);
#else
	connect(_ui->actionWebInspector, &QAction::toggled, this, &ThisType::OnWebInspector);
#endif
}

void MainWindow::setupDockWindows()
{
	// "Main desc" widget
#ifndef QT_WEBENGINEWIDGETS_LIB
	_mainDescTextBox = new QspTextBox(this->centralWidget());
	connect(_mainDescTextBox, &QspTextBox::anchorClicked, this, &MainWindow::OnLinkClicked);
#else
	_mainDescTextBox = new QspWebBox(this->centralWidget());
	connect(_mainDescTextBox, &QspWebBox::qspLinkClicked, this, &MainWindow::OnLinkClicked);
#endif
	_mainDescTextBox->setObjectName(QStringLiteral("_mainDescTextBox"));
	_ui->dockWidgetMainDesc->setWidget(_mainDescTextBox);

	// "Objects" widget
	_objectsListBox = new QspListBox(this->centralWidget());
	_objectsListBox->setObjectName(QStringLiteral("_objectsListBox"));
	connect(_objectsListBox, &QspListBox::itemClicked, this, &MainWindow::OnObjectListBoxItemClicked);
	// connect(_objectsListBox, &QspListBox::itemPressed, this, &MainWindow::OnObjectListBoxItemClicked);
	connect(_objectsListBox, &QspListBox::itemDoubleClicked, this, &MainWindow::OnObjectListBoxItemClicked);
	// connect(_objectsListBox, &QspListBox::currentRowChanged, this, &MainWindow::OnObjectChange);
	GetObjectsDock()->setWidget(_objectsListBox);

	// "Actions" widget
	_actionsListBox = new QspListBox(this->centralWidget());
	_actionsListBox->setObjectName(QStringLiteral("_actionsListBox"));
	connect(_actionsListBox, &QspListBox::itemClicked, this, &MainWindow::OnActionsListBoxItemClicked);
	// connect(_actionsListBox, &QspListBox::itemPressed, this, &MainWindow::OnActionsListBoxItemClicked);
	connect(_actionsListBox, &QspListBox::itemDoubleClicked, this, &MainWindow::OnActionsListBoxItemClicked);
	connect(_actionsListBox, &QspListBox::SelectionChange, this, &MainWindow::OnActionChange);
	_actionsListBox->SetMouseTracking(true);
	GetActionsDock()->setWidget(_actionsListBox);

	// "Additional desc" widget
#ifndef QT_WEBENGINEWIDGETS_LIB
	_descTextBox = new QspTextBox(this->centralWidget());
	connect(_descTextBox, SIGNAL(anchorClicked(QUrl)), this, SLOT(OnLinkClicked(QUrl)));
#else
	_descTextBox = new QspWebBox(this->centralWidget());
	connect(_descTextBox, &QspWebBox::qspLinkClicked, this, &MainWindow::OnLinkClicked);
#endif
	_descTextBox->setObjectName(QStringLiteral("_descTextBox"));
	_ui->dockWidgetAdditional->setWidget(_descTextBox);

	// "Input area" widget
	_inputTextBox = new QspInputBox(this->centralWidget());
	_inputTextBox->setObjectName(QLatin1String("_inputTextBox"));
	_ui->dockWidgetInput->setWidget(_inputTextBox);
	connect(_inputTextBox, &QPlainTextEdit::textChanged, this, &MainWindow::OnInputTextChange);
	connect(_inputTextBox, &QspInputBox::InputTextEnter, this, &MainWindow::OnInputTextEnter);

	m_imgView = new QspImgCanvas(this->centralWidget());
	m_imgView->setObjectName(QLatin1String("m_imgView"));
	_ui->dockWidgetImage->setWidget(m_imgView);

	splitDockWidget(_ui->dockWidgetActions, _ui->dockWidgetInput, Qt::Vertical);
	splitDockWidget(_ui->dockWidgetMainDesc, _ui->dockWidgetObjects, Qt::Horizontal);

	setCentralWidget(nullptr);
	setDockNestingEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
#ifdef QT_WEBENGINEWIDGETS_LIB
	_mainDescTextBox->Quit();
	_descTextBox->Quit();
	// delete _mainDescTextBox;
	// delete _descTextBox;
	delete _inspector;
	_inspector = nullptr;
#endif
	if (!m_configPath.isEmpty()) {
		SaveSettings(m_configPath);
	}
	QFileInfo settingsFile(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
	if (settingsFile.exists() && settingsFile.isFile()) {
		SaveSettings(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
	} else {
		SaveSettings();
	}
	EnableControls(false, true);
	setVisible(false);
	m_isQuit = true;

	QSPTerminate();
	QSPCallBacks::DeInit();

	QCoreApplication::processEvents();
	QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	int action = -1;
	switch (event->key()) {
		case Qt::Key_0: action = 9; break;
		case Qt::Key_1:
		case Qt::Key_2:
		case Qt::Key_3:
		case Qt::Key_4:
		case Qt::Key_5:
		case Qt::Key_6:
		case Qt::Key_7:
		case Qt::Key_8:
		case Qt::Key_9: action = event->key() - Qt::Key_1; break;
	}

	if (action != -1 && _actionsListBox->count() > action) {
		if (!QSPSetSelActionIndex(action, QSP_TRUE)) {
			ShowError();
		}
		if (!QSPExecuteSelActionCode(QSP_TRUE)) {
			ShowError();
		}
		return;
	}

	if (event->key() == Qt::Key_Up) {
		if (_actionsListBox->count() != 0) {
			int newSel = _actionsListBox->GetSelection() - 1;
			if (newSel < 0) {
				_actionsListBox->SetSelection(_actionsListBox->count() - 1);
			} else {
				_actionsListBox->SetSelection(newSel);
			}
		}
		return;
	}
	if (event->key() == Qt::Key_Down) {
		if (_actionsListBox->count() != 0) {
			int newSel = _actionsListBox->GetSelection() + 1;
			if (newSel <= 0 || newSel >= _actionsListBox->count()) {
				_actionsListBox->SetSelection(0);
			} else {
				_actionsListBox->SetSelection(newSel);
			}
		}
		return;
	}
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
		if (_actionsListBox->GetSelection() != -1) {
			ActionsListBoxDoAction(_actionsListBox->GetSelection());
			return;
		}
	}


	if (event->key() == Qt::Key_Escape) {
		if (isFullScreen()) {
			showNormal();
		}
	}
#ifndef QT_WEBENGINEWIDGETS_LIB
	_descTextBox->keyPressEvent(event);
	_mainDescTextBox->keyPressEvent(event);
#endif
	QMainWindow::keyPressEvent(event);
}

void MainWindow::OpenGameFile(const QString& path)
{
	if (!path.isEmpty()) {
		QFileInfo fileName(path);
		QSPCallBacks::m_gamePath = fileName.canonicalPath();
		if (!QSPCallBacks::m_gamePath.endsWith("/")) {
			QSPCallBacks::m_gamePath += "/";
		}
		_mainDescTextBox->SetGamePath(QSPCallBacks::m_gamePath);
		_objectsListBox->SetGamePath(QSPCallBacks::m_gamePath);
		_actionsListBox->SetGamePath(QSPCallBacks::m_gamePath);
		_descTextBox->SetGamePath(QSPCallBacks::m_gamePath);
		if (QSPTools::loadGameFile((path))) {
			m_isGameOpened = true;
			lastGame = path;
			QFileInfo file(path);
			m_gameFile = file.canonicalFilePath();
			QString filePath(file.canonicalPath());
			if (!filePath.endsWith("/")) {
				filePath += "/";
			}
			QString configString(filePath + QSP_CONFIG);
			if (configString != m_configPath && perGameConfig) {
				if (m_configPath.isEmpty()) {
					QFileInfo settingsFile(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
					if (settingsFile.exists() && settingsFile.isFile()) {
						SaveSettings(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
					} else {
						SaveSettings();
					}
				} else {
					SaveSettings(m_configPath);
				}
				m_configPath = configString;
				QFileInfo configFile(configString);
				if (configFile.exists() && configFile.isFile()) {
					LoadSettings(configString);
				}
			}
			if (!m_isUseBackColor) {
				ApplyBackColor(m_defaultBackColor);
			}
			if (!m_isUseLinkColor) {
				ApplyLinkColor(m_defaultLinkColor);
			}
			if (!m_isUseFontColor) {
				ApplyFontColor(m_defaultFontColor);
			}
			if (!m_isUseFont) {
				ApplyFont(m_defaultFont, 0, 0);
			}
			QFileInfo cssFile(filePath + "custom.css");
			if (cssFile.exists() && cssFile.isFile()) {
				_mainDescTextBox->SetCustomCSS(true);
				_descTextBox->SetCustomCSS(true);
			} else {
				_mainDescTextBox->SetCustomCSS(false);
				_descTextBox->SetCustomCSS(false);
			}
			UpdateGamePath(filePath);
			OnNewGame();
			if (m_isQuit) {
				return;
			}
			// UpdateTitle();
			EnableControls(true);
			m_savedGamePath.clear();
			ApplyParams();
		} else {
			ShowError();
		}
	}
}

void MainWindow::ActionsListBoxDoAction(int action)
{
	if (m_isProcessEvents) {
		if (action != -1) {
			if (!QSPSetSelActionIndex(action, QSP_TRUE)) {
				ShowError();
			}
			if (!QSPExecuteSelActionCode(QSP_TRUE)) {
				ShowError();
			}
		}
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	if (!event->mimeData()->hasUrls() || event->mimeData()->urls().empty()) {
		return;
	}

	QString path = event->mimeData()->urls().at(0).toLocalFile();
	if (path.endsWith(".qsp")) {
		OpenGameFile(path);
		event->acceptProposedAction();
	} else if (path.endsWith(".sav")) {
		if (m_isGameOpened) {
			if (!QSPCallBacks::OpenGameStatusEx(QSPStr(path), true)) {
				ShowError();
			} else {
				ApplyParams();
			}
		}
		event->acceptProposedAction();
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept();
}

void MainWindow::OnOpenGame()
{
#ifndef Q_OS_ANDROID
#	ifndef NO_NATIVE_DIALOGS
	QString path = QFileDialog::getOpenFileName(this, tr("Select game file"), GetLastPath(), tr("QSP games (*.qsp *.gam)"));
#	else
	QString path =
		QFileDialog::getOpenFileName(this, tr("Select game file"), GetLastPath(), tr("QSP games (*.qsp *.gam)"), nullptr, QFileDialog::DontUseNativeDialog);
#	endif
	if (!path.isEmpty()) {
		SetLastPath(QFileInfo(path).canonicalPath());
		OpenGameFile(path);
	}
#else
	QString path = QFileDialog::getOpenFileName(
		this, tr("Select game file"), QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(0), tr("QSP games (*.qsp *.gam)"));
	if (!path.isEmpty()) {
		SetLastPath(QFileInfo(path).canonicalPath());
		OpenGameFile(path);
	}
	return;
	AndroidFileDialog fileDialog;
	connect(&fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(OpenGameFile(QString)));
	bool success = fileDialog.provideExistingFileName();
	if (!success) {
		qDebug() << "Problem with JNI or sth like that...";
		disconnect(fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(OpenGameFile(QString)));
		// or just delete fileDialog instead of disconnect
	}
#endif
}

void MainWindow::OnRestartGame()
{
	if (m_isGameOpened) {
		if (!QSPRestartGame(QSP_TRUE)) {
			ShowError();
		} else {
			ApplyParams();
		}
	}
}

void MainWindow::OnOpenSavedGame()
{
	if (!m_isGameOpened) {
		return;
	}
#ifndef NO_NATIVE_DIALOGS
	QString path = QFileDialog::getOpenFileName(this, tr("Select saved game file"), GetLastPath(), tr("Saved game files (*.sav)"));
#else
	QString path = QFileDialog::getOpenFileName(
		this, tr("Select saved game file"), GetLastPath(), tr("Saved game files (*.sav)"), nullptr, QFileDialog::DontUseNativeDialog);
#endif
	if (!path.isEmpty()) {
		SetLastPath(QFileInfo(path).canonicalPath());
		if (!QSPCallBacks::OpenGameStatusEx(QSPStr(path), true)) {
			ShowError();
		} else {
			ApplyParams();
		}
	}
}

void MainWindow::OnSaveGame()
{
	if (!m_isGameOpened) {
		return;
	}
#ifndef NO_NATIVE_DIALOGS
	QString path = QFileDialog::getSaveFileName(this, tr("Select file to save"), GetLastPath(), tr("Saved game files (*.sav)"));
#else
	QString path =
		QFileDialog::getSaveFileName(this, tr("Select file to save"), GetLastPath(), tr("Saved game files (*.sav)"), nullptr, QFileDialog::DontUseNativeDialog);
#endif
	if (!path.isEmpty()) {
		if (!path.endsWith(".sav")) {
			path.append(".sav");
		}
		QString p = GetLastPath();
		if (QSPCallBacks::SaveGameStatusEx(QSPStr(path), true)) {
			SetLastPath(QFileInfo(path).canonicalPath());
			m_savedGamePath = path;
		} else {
			ShowError();
		}
	}
}

void MainWindow::OnOpenQuickSavedGame()
{
	if (!m_isGameOpened) {
		return;
	}
	QString path = m_path + QSP_QUICKSAVE;
	QFileInfo fileInfo(path);
	if (fileInfo.exists() && fileInfo.isFile()) {
		if (!QSPCallBacks::OpenGameStatusEx(QSPStr(path), true)) {
			ShowError();
		} else {
			ApplyParams();
		}
	}
}

void MainWindow::OnQuickSaveGame()
{
	if (!m_isGameOpened) {
		return;
	}
	QString path = m_path + QSP_QUICKSAVE;
	if (QSPCallBacks::SaveGameStatusEx(QSPStr(path), true)) {
		m_savedGamePath = path;
	} else {
		ShowError();
	}
}

void MainWindow::OnOptions()
{
	OptionsDialog optdlg(this);
	optdlg.exec();
	if (!m_configPath.isEmpty()) {
		SaveSettings(m_configPath);
	}
	QFileInfo settingsFile(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
	if (settingsFile.exists() && settingsFile.isFile()) {
		SaveSettings(QApplication::applicationDirPath() + "/" + QSP_CUSTOM_CONFIG);
	} else {
		SaveSettings();
	}
}

void MainWindow::OnAbout()
{
	QPixmap icon = QPixmap(":/gfx/logo");
	icon = icon.scaledToHeight(64, Qt::SmoothTransformation);
	QString version(QSPTools::qspStrToQt(QSPGetVersion()));
	QString libCompiledDate(QSPTools::qspStrToQt(QSPGetCompiledDateTime()));
	QString guiCompiledDate(tr(__DATE__) + tr(", ") + tr(__TIME__));
	QString text =
		(tr("<h2>Qqsp</h2>"
	        "<p>Copyright &copy; 2017-2019, Sonnix<br/>Qt6 port by ezsh, 2025</p>"));
	text += tr("<p>Application version: %1<br>QSP library version: %2<br>Qt library version: %3<br>Application compilation date: %4<br>Library compilation "
	           "date: %5</p>")
	            .arg(QApplication::applicationVersion(), version, QT_VERSION_STR, guiCompiledDate, libCompiledDate);
	QMessageBox dlg(QMessageBox::NoIcon, tr("About"), text, QMessageBox::Ok);
	dlg.setIconPixmap(icon);
	dlg.exec();
}

void MainWindow::OnToggleCaptions(bool checked)
{
	showCaptions = checked;
	QWidget* mainTitleBarWidget = _ui->dockWidgetMainDesc->titleBarWidget();
	QWidget* objectsTitleBarWidget = _ui->dockWidgetObjects->titleBarWidget();
	QWidget* actionsTitleBarWidget = _ui->dockWidgetActions->titleBarWidget();
	QWidget* descTitleBarWidget = _ui->dockWidgetAdditional->titleBarWidget();
	QWidget* inputTitleBarWidget = _ui->dockWidgetInput->titleBarWidget();
	if (!checked) {
		_ui->dockWidgetMainDesc->setTitleBarWidget(new QWidget(_ui->dockWidgetMainDesc));
		_ui->dockWidgetMainDesc->titleBarWidget()->hide();
		_ui->dockWidgetObjects->setTitleBarWidget(new QWidget(_ui->dockWidgetObjects));
		_ui->dockWidgetObjects->titleBarWidget()->hide();
		_ui->dockWidgetActions->setTitleBarWidget(new QWidget(_ui->dockWidgetActions));
		_ui->dockWidgetActions->titleBarWidget()->hide();
		_ui->dockWidgetAdditional->setTitleBarWidget(new QWidget(_ui->dockWidgetAdditional));
		_ui->dockWidgetAdditional->titleBarWidget()->hide();
		_ui->dockWidgetInput->setTitleBarWidget(new QWidget(_ui->dockWidgetInput));
		_ui->dockWidgetInput->titleBarWidget()->hide();
	} else {
		_ui->dockWidgetMainDesc->setTitleBarWidget(nullptr);
		_ui->dockWidgetObjects->setTitleBarWidget(nullptr);
		_ui->dockWidgetActions->setTitleBarWidget(nullptr);
		_ui->dockWidgetAdditional->setTitleBarWidget(nullptr);
		_ui->dockWidgetInput->setTitleBarWidget(nullptr);
	}
	delete mainTitleBarWidget;
	delete objectsTitleBarWidget;
	delete actionsTitleBarWidget;
	delete descTitleBarWidget;
	delete inputTitleBarWidget;
}

void MainWindow::OnToggleMenuBar(bool checked)
{
	_ui->mainMenuBar->setVisible(checked);
}

void MainWindow::OnToggleHotkeys(bool checked)
{
	m_isShowHotkeys = checked;
	RefreshUI();
}

void MainWindow::OnToggleWinMode()
{
	if (isFullScreen()) {
		showNormal();
	} else {
		showFullScreen();
	}
}

void MainWindow::OnToggleShowPlainText(bool checked)
{
	SetShowPlainText(checked);
}

void MainWindow::OnNewGame()
{
	if (!QSPRestartGame(QSP_TRUE)) {
		ShowError();
	}
}

void MainWindow::OnTimer()
{
	if (m_isProcessEvents && !QSPExecCounter(QSP_TRUE)) {
		ShowError();
	}
}

void MainWindow::OnLinkClicked(const QUrl& url)
{
	if (!m_isProcessEvents) {
		return;
	}
	QString href;
	href = QByteArray::fromPercentEncoding(url.toString().toUtf8());

	if (href.startsWith("#")) {
		QObject* obj = sender();
		if (obj == _mainDescTextBox)
#ifndef QT_WEBENGINEWIDGETS_LIB
			_mainDescTextBox->setSource(url);
#else
			_mainDescTextBox->setUrl(url);
#endif
		else
#ifndef QT_WEBENGINEWIDGETS_LIB
			_descTextBox->setSource(url);
#else
			_descTextBox->setUrl(url);
#endif
	} else if (href.startsWith("EXEC:", Qt::CaseInsensitive)) {
		QString string = href.mid(5);
		if (m_isProcessEvents && !QSPExecString(QSPStr(string), QSP_TRUE)) {
			ShowError();
		}
	} else {
		QDesktopServices::openUrl(url);
	}
}

void MainWindow::OnObjectListBoxItemClicked(QListWidgetItem* itemClicked)
{
	if (!m_isProcessEvents) {
		return;
	}
	int object = _objectsListBox->row(itemClicked);
	if (!QSPSetSelObjectIndex(object, QSP_TRUE)) {
		ShowError();
	}
}

void MainWindow::OnActionsListBoxItemClicked(QListWidgetItem* itemClicked)
{
	if (!m_isProcessEvents) {
		return;
	}
	int action = _actionsListBox->row(itemClicked);
	ActionsListBoxDoAction(action);
}

void MainWindow::OnObjectChange(int currentRow)
{
	if (!m_isProcessEvents) {
		return;
	}
	// QThread::msleep(20);
	if (!QSPSetSelObjectIndex(currentRow, QSP_TRUE)) {
		ShowError();
	}
}

void MainWindow::OnActionChange(int currentRow)
{
	if (!m_isProcessEvents) {
		return;
	}
	if (!QSPSetSelActionIndex(currentRow, QSP_TRUE)) {
		ShowError();
	}
}

void MainWindow::OnMenu(QAction* action)
{
	m_menuIndex = action->data().toInt();
}

void MainWindow::OnInputTextChange()
{
	QSPSetInputStrText(QSPStr(_inputTextBox->GetText()));
}

void MainWindow::OnInputTextEnter()
{
	if (!m_isProcessEvents) {
		return;
	}
	QSPSetInputStrText(QSPStr(_inputTextBox->GetText()));
	if (!QSPExecUserInput(QSP_TRUE)) {
		ShowError();
	}
}

void MainWindow::OnWebInspector(bool checked)
{
#ifdef QT_WEBENGINEWIDGETS_LIB
	if (checked) {
		_inspector = new WebInspectorWindow(this);
		_inspector->setViews(_mainDescTextBox, _descTextBox);
		_inspector->show();
		connect(_inspector, &QObject::destroyed, this, [this]() {
			_inspector = nullptr;
			_ui->actionWebInspector->setChecked(false);
		});
	}
#endif
}
