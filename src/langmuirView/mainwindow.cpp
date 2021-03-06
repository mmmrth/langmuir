#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "langmuirviewer.h"
#include "pointdialog.h"
#include "isosurfacedialog.h"

#include <QMetaEnum>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QAction>
#include <QDebug>
#include <QStyle>
#include <QIcon>

namespace LangmuirView {

MainWindow::MainWindow(const QString &inputFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pointdialog(NULL),
    m_isosurfacedialog(NULL)
{
    ui->setupUi(this);
    m_viewer = ui->qglwidget;
    init();
    m_viewer->load(inputFile);
    m_currentDir = QDir::current();
}

void MainWindow::init()
{
    setIcon(ui->actionExit, "application-exit", QStyle::SP_DialogCloseButton);
    setIcon(ui->actionStart, "media-playback-start", QStyle::SP_MediaPlay);
    setIcon(ui->actionStop, "media-playback-stop", QStyle::SP_MediaStop);
    setIcon(ui->actionOpen, "document-open", QStyle::SP_DialogOpenButton);
    setIcon(ui->actionShow, "camera-photo", QStyle::SP_ArrowBack);
    setIcon(ui->actionUnload, "", QStyle::SP_DialogDiscardButton);
    setIcon(ui->actionSave, "document-save", QStyle::SP_DialogSaveButton);
    setIcon(ui->actionScreenshot, "camera-photo", QStyle::SP_DialogSaveButton);
    setIcon(ui->actionReset, "", QStyle::SP_BrowserReload);
    setIcon(ui->actionSaveSettings, "document-save", QStyle::SP_DialogSaveButton);
    setIcon(ui->actionLoadSettings, "document-open", QStyle::SP_DialogOpenButton);

    connect(m_viewer, SIGNAL(clearMessage()), ui->statusbar, SLOT(clearMessage()));
    connect(m_viewer, SIGNAL(showMessage(QString,int)), ui->statusbar, SLOT(showMessage(QString,int)));

    connect(m_viewer, SIGNAL(openGLInitFinished()), this, SLOT(initAfter()));

    ui->colorButtonElectrons->colorDialog().setCustomColor( 0, QColor(  0, 43, 94)); //! PittBlue
    ui->colorButtonElectrons->colorDialog().setCustomColor( 2, QColor(197,168,118)); //! PittGold

    ui->colorButtonElectrons->colorDialog().setCustomColor( 4, QColor("#FE2712"));   //! R1
    //ui->colorButtonElectrons->colorDialog().setCustomColor( 4, QColor("#EC3424")); //! HR
    ui->colorButtonElectrons->colorDialog().setCustomColor( 6, QColor("#FD5308"));   //! R2

    ui->colorButtonElectrons->colorDialog().setCustomColor( 8, QColor("#FB9902"));   //! O1
    //ui->colorButtonElectrons->colorDialog().setCustomColor(10, QColor("#FF9200")); //! HO
    ui->colorButtonElectrons->colorDialog().setCustomColor(10, QColor("#FABC02"));   //! O2

    ui->colorButtonElectrons->colorDialog().setCustomColor(12, QColor("#FEFE33"));   //! Y1

    ui->colorButtonElectrons->colorDialog().setCustomColor(14, QColor("#D0EA2B"));   //! G1
    ui->colorButtonElectrons->colorDialog().setCustomColor( 1, QColor("#66B032"));   //! G2
    ui->colorButtonElectrons->colorDialog().setCustomColor( 3, QColor("#5BB95C"));   //! HG

    ui->colorButtonElectrons->colorDialog().setCustomColor( 5, QColor("#0392CE"));   //! B1
    ui->colorButtonElectrons->colorDialog().setCustomColor( 7, QColor("#0247FE"));   //! B2
    ui->colorButtonElectrons->colorDialog().setCustomColor( 9, QColor("#4492C5"));   //! HB

    ui->colorButtonElectrons->colorDialog().setCustomColor(11, QColor("#3D01A4"));   //! P1
    ui->colorButtonElectrons->colorDialog().setCustomColor(13, QColor("#8601AF"));   //! P2
    ui->colorButtonElectrons->colorDialog().setCustomColor(15, QColor("#A7194B"));   //! P3
}

void MainWindow::initAfter()
{
    connect(ui->colorButtonDefects, SIGNAL(selectedColor(QColor)), &m_viewer->defects(), SLOT(setColor(QColor)));
    connect(&m_viewer->defects(), SIGNAL(colorChanged(QColor)), ui->colorButtonDefects, SLOT(setButtonColor(QColor)));
    ui->colorButtonDefects->setButtonColor(m_viewer->defects().getColor());

    connect(ui->colorButtonElectrons, SIGNAL(selectedColor(QColor)), &m_viewer->electrons(), SLOT(setColor(QColor)));
    connect(&m_viewer->electrons(), SIGNAL(colorChanged(QColor)), ui->colorButtonElectrons, SLOT(setButtonColor(QColor)));
    ui->colorButtonElectrons->setButtonColor(m_viewer->electrons().getColor());

    connect(ui->colorButtonHoles, SIGNAL(selectedColor(QColor)), &m_viewer->holes(), SLOT(setColor(QColor)));
    connect(&m_viewer->holes(), SIGNAL(colorChanged(QColor)), ui->colorButtonHoles, SLOT(setButtonColor(QColor)));
    ui->colorButtonHoles->setButtonColor(m_viewer->holes().getColor());

    connect(ui->colorButtonGrid, SIGNAL(selectedColor(QColor)), &m_viewer->grid(), SLOT(setColor(QColor)));
    connect(&m_viewer->grid(), SIGNAL(colorChanged(QColor)), ui->colorButtonGrid, SLOT(setButtonColor(QColor)));
    ui->colorButtonGrid->setButtonColor(m_viewer->grid().getColor());

    connect(ui->colorButtonTraps, SIGNAL(selectedColor(QColor)), m_viewer, SLOT(setTrapColor(QColor)));
    connect(ui->colorButtonTraps, SIGNAL(selectedColor(QColor)), &m_viewer->traps(), SLOT(setColor(QColor)));
    connect(m_viewer, SIGNAL(trapColorChanged(QColor)), ui->colorButtonTraps, SLOT(setButtonColor(QColor)));
    ui->colorButtonTraps->setButtonColor(m_viewer->trapColor());

    connect(ui->colorButtonBackground, SIGNAL(selectedColor(QColor)), m_viewer, SLOT(setBackgroundColor(QColor)));
    connect(m_viewer, SIGNAL(backgroundColorChanged(QColor)), ui->colorButtonBackground, SLOT(setButtonColor(QColor)));
    ui->colorButtonBackground->setButtonColor(m_viewer->backgroundColor());

    connect(ui->colorButtonBase, SIGNAL(selectedColor(QColor)), &m_viewer->baseBox(), SLOT(setColor(QColor)));
    connect(ui->colorButtonBase, SIGNAL(selectedColor(QColor)), &m_viewer->trapBox(), SLOT(setColor(QColor)));
    connect(&m_viewer->baseBox(), SIGNAL(colorChanged(QColor)), ui->colorButtonBase, SLOT(setButtonColor(QColor)));
    connect(&m_viewer->trapBox(), SIGNAL(colorChanged(QColor)), ui->colorButtonBase, SLOT(setButtonColor(QColor)));
    connect(&m_viewer->baseBox(), SIGNAL(colorChanged(QColor)), m_viewer, SLOT(initTraps()));
    ui->colorButtonBase->setButtonColor(m_viewer->baseBox().getColor());

    connect(ui->colorButtonLeft, SIGNAL(selectedColor(QColor)), &m_viewer->leftBox(), SLOT(setColor(QColor)));
    connect(&m_viewer->leftBox(), SIGNAL(colorChanged(QColor)), ui->colorButtonLeft, SLOT(setButtonColor(QColor)));
    ui->colorButtonLeft->setButtonColor(m_viewer->leftBox().getColor());

    connect(ui->colorButtonRight, SIGNAL(selectedColor(QColor)), &m_viewer->rightBox(), SLOT(setColor(QColor)));
    connect(&m_viewer->rightBox(), SIGNAL(colorChanged(QColor)), ui->colorButtonRight, SLOT(setButtonColor(QColor)));
    ui->colorButtonRight->setButtonColor(m_viewer->rightBox().getColor());

    connect(ui->colorButtonStage, SIGNAL(selectedColor(QColor)), &m_viewer->stageBox(), SLOT(setColor(QColor)));
    connect(&m_viewer->stageBox(), SIGNAL(colorChanged(QColor)), ui->colorButtonStage, SLOT(setButtonColor(QColor)));
    connect(&m_viewer->stageBox(), SIGNAL(colorChanged(QColor)), m_viewer, SLOT(initStage()));
    ui->colorButtonStage->setButtonColor(m_viewer->stageBox().getColor());

    connect(ui->colorButtonStageTexture, SIGNAL(selectedColor(QColor)), m_viewer, SLOT(setStageColor2(QColor)));
    connect(m_viewer, SIGNAL(stageColor2Changed(QColor)), ui->colorButtonStageTexture, SLOT(setButtonColor(QColor)));
    ui->colorButtonStageTexture->setButtonColor(m_viewer->stageColor2());

    connect(&m_viewer->trapBox(), SIGNAL(imageOnChanged(bool)), ui->actionTraps, SLOT(setChecked(bool)));
    ui->actionTraps->setChecked(m_viewer->trapBox().imageIsOn());

    connect(&m_viewer->stageBox(), SIGNAL(imageOnChanged(bool)), ui->pushButtonStageTexture, SLOT(setChecked(bool)));
    ui->pushButtonStageTexture->setChecked(m_viewer->stageBox().imageIsOn());

    connect(&m_viewer->cornerAxis(), SIGNAL(visibleChanged(bool)), ui->actionCornerAxis, SLOT(setChecked(bool)));

    connect(m_viewer, SIGNAL(iterationsPrintChanged(int)), this, SLOT(updateSpinBox(int)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), m_viewer, SLOT(setIterationsPrint(int)));

    connect(ui->pushButtonElectrons, SIGNAL(clicked(bool)), &m_viewer->electrons(), SLOT(setVisible(bool)));
    ui->pushButtonElectrons->setChecked(m_viewer->electrons().isVisible());

    connect(ui->pushButtonDefects, SIGNAL(clicked(bool)), &m_viewer->defects(), SLOT(setVisible(bool)));
    ui->pushButtonDefects->setChecked(m_viewer->defects().isVisible());

    connect(ui->pushButtonHoles, SIGNAL(clicked(bool)), &m_viewer->holes(), SLOT(setVisible(bool)));
    ui->pushButtonHoles->setChecked(m_viewer->holes().isVisible());

    connect(ui->pushButtonBase, SIGNAL(clicked(bool)), &m_viewer->baseBox(), SLOT(setVisible(bool)));
    connect(ui->pushButtonBase, SIGNAL(clicked(bool)), &m_viewer->trapBox(), SLOT(setVisible(bool)));
    ui->pushButtonBase->setChecked(m_viewer->baseBox().isVisible());

    connect(ui->pushButtonLeft, SIGNAL(clicked(bool)), &m_viewer->leftBox(), SLOT(setVisible(bool)));
    ui->pushButtonLeft->setChecked(m_viewer->leftBox().isVisible());

    connect(ui->pushButtonRight, SIGNAL(clicked(bool)), &m_viewer->rightBox(), SLOT(setVisible(bool)));
    ui->pushButtonRight->setChecked(m_viewer->rightBox().isVisible());

    connect(ui->pushButtonStage, SIGNAL(clicked(bool)), &m_viewer->stageBox(), SLOT(setVisible(bool)));
    ui->pushButtonStage->setChecked(m_viewer->stageBox().isVisible());

    connect(ui->pushButtonGrid, SIGNAL(clicked(bool)), &m_viewer->grid(), SLOT(setVisible(bool)));
    ui->pushButtonGrid->setChecked(m_viewer->grid().isVisible());

    connect(ui->pushButtonStageTexture, SIGNAL(clicked(bool)), &m_viewer->stageBox(), SLOT(showImage(bool)));
    ui->pushButtonStageTexture->setChecked(m_viewer->stageBox().imageIsOn());

    connect(ui->pushButtonTraps, SIGNAL(clicked(bool)), &m_viewer->trapBox(), SLOT(showImage(bool)));
    ui->pushButtonTraps->setChecked(m_viewer->trapBox().imageIsOn());

    m_viewer->resetSettings();
}

void MainWindow::updateSpinBox(int value)
{
    ui->spinBox->setValue(value);
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_pointdialog != NULL) {
        m_pointdialog->deleteLater();
        m_pointdialog = NULL;
    }

    if (m_isosurfacedialog != NULL) {
        m_isosurfacedialog->deleteLater();
        m_isosurfacedialog = NULL;
    }
}

void MainWindow::on_actionScreenshot_triggered()
{
    m_viewer->pause();
    m_viewer->openSnapshotFormatDialog();
    m_viewer->saveSnapshot(false, false);
}

void MainWindow::on_actionOpen_triggered()
{
    m_viewer->pause();

    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open Input File"), m_currentDir.absolutePath(), "Simulation (*.inp* *.chk*);; All Files (*)");

    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.absoluteDir().exists()) {
            m_currentDir = info.absoluteDir();
        }
    }

    m_viewer->load(fileName);
}

void MainWindow::on_actionSave_triggered()
{
    m_viewer->pause();

    QString fileName = QFileDialog::getSaveFileName(this, "Save simulation checkpoint",
        m_currentDir.absolutePath(), "Simulation (*.inp *.chk);; All Files (*)");

    if (!fileName.isEmpty()) {

        QFileInfo info(fileName);

        if (info.absoluteDir().exists()) {
            m_currentDir = info.absoluteDir();
        }

        if (!(info.suffix() == "inp" || info.suffix() == "chk"))
        {
            QMessageBox::StandardButton choice = QMessageBox::warning(this, "Langmuir",
                "Change to extension to .chk?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

            switch(choice)
            {
                case QMessageBox::Yes:
                {
                    fileName = info.absoluteDir().absoluteFilePath(info.completeBaseName() + ".chk");
                    break;
                }
                case QMessageBox::No:
                {
                    break;
                }
            }
        }
    }

    m_viewer->save(fileName);
}

void MainWindow::on_actionLoadSettings_triggered()
{
    m_viewer->pause();

    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open settings file"), m_currentDir.absolutePath(), "Settings (*.ini);; All Files (*)");

    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.absoluteDir().exists()) {
            m_currentDir = info.absoluteDir();
        }
    }

    m_viewer->loadSettings(fileName);
}

void MainWindow::on_actionPoints_triggered()
{
    if (m_pointdialog == NULL) {
        m_pointdialog = new PointDialog(*m_viewer, NULL);
    }

    m_pointdialog->remember();
    m_pointdialog->show();
    m_pointdialog->raise();
}

void MainWindow::on_actionIsoSurface_triggered()
{
    if (m_isosurfacedialog == NULL) {
        m_isosurfacedialog = new IsoSurfaceDialog(*m_viewer, NULL);
    }

    m_isosurfacedialog->update();
    m_isosurfacedialog->show();
    m_isosurfacedialog->raise();
}

void MainWindow::on_actionChecker_triggered()
{
    bool ok = false;
    double size = QInputDialog::getDouble(this, "Langmuir", "size", 32.0, 1.0, 256.0, 0, &ok);
    if (ok) {
        m_viewer->setCheckerSize(size);
    }
}

void MainWindow::on_actionResetSettings_triggered()
{
    m_viewer->resetSettings();
    m_viewer->showMessage("reset parameters!");
}

void MainWindow::on_actionSaveSettings_triggered()
{
    m_viewer->pause();

    QString fileName = QFileDialog::getSaveFileName(this, "Save settings file",
        m_currentDir.absolutePath(), "Settings (*.ini);; All Files (*)");

    if (!fileName.isEmpty()) {

        QFileInfo info(fileName);

        if (info.absoluteDir().exists()) {
            m_currentDir = info.absoluteDir();
        }

        if (info.suffix() != "ini")
        {
            QMessageBox::StandardButton choice = QMessageBox::warning(this, "Langmuir",
                "Change to extension to .ini?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

            switch(choice)
            {
                case QMessageBox::Yes:
                {
                    fileName = info.absoluteDir().absoluteFilePath(info.completeBaseName() + ".ini");
                    break;
                }
                case QMessageBox::No:
                {
                    break;
                }
            }
        }
    }

    m_viewer->saveSettings(fileName);
}

void MainWindow::setStopEnabled(bool enabled)
{
    ui->actionStart->setDisabled(enabled);
    ui->actionStop->setEnabled(enabled);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_viewer->pause();

    QMessageBox::StandardButton choice = QMessageBox::warning(this, "Langmuir",
        "Do you really want to quit?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

    if (choice == QMessageBox::Yes) {
        if (m_isosurfacedialog != NULL) {
            m_isosurfacedialog->close();
        }
        if (m_pointdialog != NULL) {
            m_pointdialog->close();
        }
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::setIcon(QAction *action, QString themeIcon, QStyle::StandardPixmap standardPixmap)
{
    if (action == NULL) {
        return;
    }
    if (QIcon::hasThemeIcon(themeIcon)) {
        action->setIcon(QIcon::fromTheme(themeIcon));
    }
    else {
        action->setIcon(this->style()->standardIcon(standardPixmap));
    }
}

}
