#include "AboutDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About NovaImage"));

    setWindowFlag(Qt::Popup);

    initUI();

    resize(400, 160);
}

void AboutDialog::initUI()
{
    QLabel* logoLabel = new QLabel;
    logoLabel->setFixedSize(60, 60);
    logoLabel->setStyleSheet("QLabel{"
        "border-image:url(:/icon/svg/view.svg) 4 4 4 4 stretch stretch;"
        "}");

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addStretch();
    vLayout->addWidget(new QLabel("NovaImage"));
    vLayout->addStretch();
    vLayout->addWidget(new QLabel("Version 1.0.1"));
    vLayout->addStretch();
    vLayout->addWidget(new QLabel("Copyright (C) 2025 DevonChen"));
    vLayout->addWidget(new QLabel("All rights reserved."));
    vLayout->addStretch();
    QLabel* linkLabel = new QLabel("<a href=\"https://github.com/devonchenc/\">https://github.com/devonchenc/");
    linkLabel->setOpenExternalLinks(true);
    vLayout->addWidget(linkLabel);
    vLayout->addStretch();

    QPushButton* acceptButton = new QPushButton(tr("OK"));
    acceptButton->setFixedWidth(80);
    connect(acceptButton, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(logoLabel);
    hLayout->addStretch();
    hLayout->addLayout(vLayout);
    hLayout->addStretch();
    hLayout->addWidget(acceptButton);
    setLayout(hLayout);
}

