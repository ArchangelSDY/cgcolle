#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMenu *menuFile = ui->menuBar->addMenu(tr("File"));
    menuFile->addAction(tr("Open"), this, &MainWindow::openFile);
    menuFile->addAction(tr("Save"), this, &MainWindow::saveFile);
    QMenu *menuEdit = ui->menuBar->addMenu(tr("Edit"));
    menuEdit->addAction(tr("Sync Type + Offset + LayerId"), this, &MainWindow::syncTypeOffsetLayerId);

    ui->tabWidget->setTabText(0, tr("Meta"));
    ui->tabWidget->setTabText(1, tr("Composite Rules"));

    // Meta tab
    ui->listEntries->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->listEntries, &QListWidget::currentRowChanged,
            this, &MainWindow::showEntry);

#define GET_ENTRY \
    int index = ui->listEntries->currentRow(); \
    CGColleV1Entry &entry = m_file->entrys()[index];

    ui->comboType->addItems({"Base Image", "Layer Image"});
    connect(ui->comboType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int type){
        GET_ENTRY
        entry.type = (CGColleV1Entry::Type)type;
    });

#define INIT_LINE_EDIT(field, fieldCap) \
    connect(ui->edit##fieldCap, &QLineEdit::textEdited, [this](const QString &text){ \
        GET_ENTRY \
        entry.##field = text; \
        ui->listEntries->currentItem()->setText(text); \
    });
#define INIT_LINE_EDIT_NUM(field, fieldCap, type) \
    connect(ui->edit##fieldCap, &QLineEdit::textEdited, [this](const QString &text){ \
        GET_ENTRY \
        entry.##field = (type)text.toInt(); \
    });
#define INIT_SPIN_NUM(field, fieldCap, type) \
    connect(ui->edit##fieldCap, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val){ \
        GET_ENTRY \
        entry.##field = (type)val; \
    });

    INIT_LINE_EDIT(scene, Scene)
    connect(ui->editName, &QLineEdit::textEdited, [this](const QString &text){
        GET_ENTRY
        entry.name = text;
        ui->listEntries->currentItem()->setText(text);
    });
    INIT_LINE_EDIT_NUM(offsetX, OffsetX, uint32_t)
    INIT_LINE_EDIT_NUM(offsetY, OffsetY, uint32_t)
    INIT_SPIN_NUM(layerId, LayerId, uchar)

    ui->comboCompositionMethod->addItems({"Source Over"});
    connect(ui->comboCompositionMethod, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int type){
        GET_ENTRY
        entry.compositionMethod = (uchar)type;
    });

    // Composite rules tab
    connect(ui->listCompositeRules, &QListWidget::currentRowChanged,
            this, &MainWindow::showCompositeRule);
    connect(ui->btnApplyCompositeRule, &QPushButton::pressed, [this](){
        int index = ui->listCompositeRules->currentRow();
        CGColleV1CompositeRule *rule = m_file->compositeRules()[index];
        rule->fromEditString(ui->editCompositeRule->toPlainText());
        ui->editCompositeRule->setPlainText(rule->toEditString());
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open"), "F:\\", "*.cgc");
    m_file.reset(new CGColleV1File(path));

    if (!m_file->open()) {
        return;
    }

    ui->listEntries->clear();
    const QList<CGColleV1Entry> &entries = m_file->entrys();
    for (const auto &entry : entries) {
        ui->listEntries->addItem(entry.name);
    }

    ui->listCompositeRules->clear();
    const QList<CGColleV1CompositeRule *> &compositeRules = m_file->compositeRules();
    for (CGColleV1CompositeRule *rule : compositeRules) {
        ui->listCompositeRules->addItem(rule->matcher());
    }
}

void MainWindow::saveFile()
{
    if (m_file->save()) {
        QMessageBox::information(this, tr("Save"), tr("Success!"));
    } else {
        QMessageBox::critical(this, tr("Save"), tr("Failed!"));
    }
}

void MainWindow::showEntry()
{
    int index = ui->listEntries->currentRow();
    const CGColleV1Entry &entry = m_file->entrys()[index];

#define SHOW_LINE_EDIT(field, fieldCap) \
    ui->edit##fieldCap->setText(field);
#define SHOW_SPIN(field, fieldCap) \
    ui->edit##fieldCap->setValue(field);

    ui->comboType->setCurrentIndex((int)entry.type);
    SHOW_LINE_EDIT(QString::number(entry.dataOffset), DataOffset)
    SHOW_LINE_EDIT(entry.scene, Scene)
    SHOW_LINE_EDIT(entry.name, Name)
    SHOW_LINE_EDIT(QString::number(entry.fileSize), FileSize)
    SHOW_LINE_EDIT(QString::number(entry.width), Width)
    SHOW_LINE_EDIT(QString::number(entry.height), Height)
    SHOW_LINE_EDIT(QString::number(entry.offsetX), OffsetX)
    SHOW_LINE_EDIT(QString::number(entry.offsetY), OffsetY)
    SHOW_SPIN(entry.layerId, LayerId)
    ui->comboCompositionMethod->setCurrentIndex((int)entry.compositionMethod);

    QImage i = m_file->readImage(index);
    qDebug() << i.isNull();
    ui->lblImage->setPixmap(QPixmap::fromImage(i));

    ui->statusBar->showMessage(QString("Data Start: %1, Data Length: %2, Meta Start: %3, Meta Length: %4")
                               .arg(m_file->dataStart())
                               .arg(m_file->dataLength())
                               .arg(m_file->metaStart())
                               .arg(m_file->metaLength()));
}

void MainWindow::showCompositeRule()
{
    int index = ui->listCompositeRules->currentRow();
    CGColleV1CompositeRule *rule = m_file->compositeRules()[index];
    ui->editCompositeRule->setPlainText(rule->toEditString());
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_F3) {
        int index = ui->listEntries->currentRow();
        if (index > 0) {
            ui->listEntries->setCurrentRow(index - 1);
        }
    } else if (ev->key() == Qt::Key_F4) {
        int index = ui->listEntries->currentRow();
        if (index < ui->listEntries->count() - 1) {
            ui->listEntries->setCurrentRow(index + 1);
        }
    }
}

void MainWindow::syncTypeOffsetLayerId()
{
    QList<QListWidgetItem *> selectedEntryItems = ui->listEntries->selectedItems();
    if (selectedEntryItems.count() <= 1) {
        return;
    }

    int srcIndex = ui->listEntries->row(selectedEntryItems[0]);
    const CGColleV1Entry &srcEntry = m_file->entrys()[srcIndex];

    for (int i = 1; i < selectedEntryItems.count(); i++) {
        int index = ui->listEntries->row(selectedEntryItems[i]);
        CGColleV1Entry &entry = m_file->entrys()[index];
        entry.type = srcEntry.type;
        entry.offsetX = srcEntry.offsetX;
        entry.offsetY = srcEntry.offsetY;
        entry.layerId = srcEntry.layerId;
    }
}
