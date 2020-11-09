/********************************************************************************
** Form generated from reading UI file 'plugindetailsview.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLUGINDETAILSVIEW_H
#define UI_PLUGINDETAILSVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace ExtensionSystem {
namespace Internal {

class Ui_PluginDetailsView
{
public:
    QGridLayout *gridLayout;
    QLabel *platforms;
    QVBoxLayout *vboxLayout;
    QLabel *label_8;
    QSpacerItem *spacerItem;
    QLabel *version;
    QLabel *label;
    QLabel *label_3;
    QTextEdit *license;
    QVBoxLayout *vboxLayout1;
    QLabel *label_10;
    QSpacerItem *spacerItem1;
    QLabel *name;
    QLabel *label_2;
    QLabel *label_4;
    QListWidget *dependencies;
    QLabel *label_7;
    QLabel *location;
    QLabel *platformsDescriptionLabel;
    QLabel *label_6;
    QLabel *url;
    QLabel *vendor;
    QLabel *label_11;
    QLabel *compatVersion;
    QTextEdit *description;
    QLabel *label_5;
    QLabel *copyright;
    QVBoxLayout *vboxLayout2;
    QLabel *label_9;
    QSpacerItem *spacerItem2;
    QLabel *component;

    void setupUi(QWidget *ExtensionSystem__Internal__PluginDetailsView)
    {
        if (ExtensionSystem__Internal__PluginDetailsView->objectName().isEmpty())
            ExtensionSystem__Internal__PluginDetailsView->setObjectName(QString::fromUtf8("ExtensionSystem__Internal__PluginDetailsView"));
        ExtensionSystem__Internal__PluginDetailsView->resize(674, 505);
        gridLayout = new QGridLayout(ExtensionSystem__Internal__PluginDetailsView);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(2, 2, 2, 2);
        platforms = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        platforms->setObjectName(QString::fromUtf8("platforms"));

        gridLayout->addWidget(platforms, 7, 1, 1, 1);

        vboxLayout = new QVBoxLayout();
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label_8 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_8->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        vboxLayout->addWidget(label_8);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem);


        gridLayout->addLayout(vboxLayout, 8, 0, 1, 1);

        version = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        version->setObjectName(QString::fromUtf8("version"));
        version->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(version, 1, 1, 1, 1);

        label = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_3 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_3->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        license = new QTextEdit(ExtensionSystem__Internal__PluginDetailsView);
        license->setObjectName(QString::fromUtf8("license"));
        license->setTabChangesFocus(true);
        license->setReadOnly(true);

        gridLayout->addWidget(license, 10, 1, 1, 1);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        label_10 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_10->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        vboxLayout1->addWidget(label_10);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem1);


        gridLayout->addLayout(vboxLayout1, 11, 0, 1, 1);

        name = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        name->setObjectName(QString::fromUtf8("name"));
        name->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(name, 0, 1, 1, 1);

        label_2 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_2->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_4 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_4->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        dependencies = new QListWidget(ExtensionSystem__Internal__PluginDetailsView);
        dependencies->setObjectName(QString::fromUtf8("dependencies"));

        gridLayout->addWidget(dependencies, 11, 1, 1, 1);

        label_7 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_7->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        location = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        location->setObjectName(QString::fromUtf8("location"));
        location->setWordWrap(false);
        location->setTextInteractionFlags(Qt::TextBrowserInteraction);

        gridLayout->addWidget(location, 6, 1, 1, 1);

        platformsDescriptionLabel = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        platformsDescriptionLabel->setObjectName(QString::fromUtf8("platformsDescriptionLabel"));
        platformsDescriptionLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(platformsDescriptionLabel, 7, 0, 1, 1);

        label_6 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        url = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        url->setObjectName(QString::fromUtf8("url"));
        url->setOpenExternalLinks(true);
        url->setTextInteractionFlags(Qt::TextBrowserInteraction);

        gridLayout->addWidget(url, 5, 1, 1, 1);

        vendor = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        vendor->setObjectName(QString::fromUtf8("vendor"));
        vendor->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(vendor, 3, 1, 1, 1);

        label_11 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_11->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_11, 4, 0, 1, 1);

        compatVersion = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        compatVersion->setObjectName(QString::fromUtf8("compatVersion"));
        compatVersion->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(compatVersion, 2, 1, 1, 1);

        description = new QTextEdit(ExtensionSystem__Internal__PluginDetailsView);
        description->setObjectName(QString::fromUtf8("description"));
        description->setTabChangesFocus(true);
        description->setReadOnly(true);

        gridLayout->addWidget(description, 8, 1, 1, 1);

        label_5 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(label_5, 9, 0, 1, 1);

        copyright = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        copyright->setObjectName(QString::fromUtf8("copyright"));
        copyright->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(copyright, 9, 1, 1, 1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        label_9 = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_9->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        vboxLayout2->addWidget(label_9);

        spacerItem2 = new QSpacerItem(17, 13, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout2->addItem(spacerItem2);


        gridLayout->addLayout(vboxLayout2, 10, 0, 1, 1);

        component = new QLabel(ExtensionSystem__Internal__PluginDetailsView);
        component->setObjectName(QString::fromUtf8("component"));
        component->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout->addWidget(component, 4, 1, 1, 1);


        retranslateUi(ExtensionSystem__Internal__PluginDetailsView);

        QMetaObject::connectSlotsByName(ExtensionSystem__Internal__PluginDetailsView);
    } // setupUi

    void retranslateUi(QWidget *ExtensionSystem__Internal__PluginDetailsView)
    {
        label_8->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Description:", nullptr));
        label->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Name:", nullptr));
        label_3->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Compatibility version:", nullptr));
        label_10->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Dependencies:", nullptr));
        label_2->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Version:", nullptr));
        label_4->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Vendor:", nullptr));
        label_7->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Location:", nullptr));
        platformsDescriptionLabel->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Platforms:", nullptr));
        label_6->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "URL:", nullptr));
        label_11->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Group:", nullptr));
        label_5->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "Copyright:", nullptr));
        label_9->setText(QCoreApplication::translate("ExtensionSystem::Internal::PluginDetailsView", "License:", nullptr));
        component->setText(QString());
        Q_UNUSED(ExtensionSystem__Internal__PluginDetailsView);
    } // retranslateUi

};

} // namespace Internal
} // namespace ExtensionSystem

namespace ExtensionSystem {
namespace Internal {
namespace Ui {
    class PluginDetailsView: public Ui_PluginDetailsView {};
} // namespace Ui
} // namespace Internal
} // namespace ExtensionSystem

#endif // UI_PLUGINDETAILSVIEW_H
