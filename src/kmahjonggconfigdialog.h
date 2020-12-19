/*
    SPDX-FileCopyrightText: 2006 Mauricio Piacentini <mauricio@tabuleiro.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KMAHJONGGCONFIGDIALOG_H
#define KMAHJONGGCONFIGDIALOG_H

// KF
#include <KConfigDialog>
#include <KConfigSkeleton>

// LibKMahjongg
#include <libkmahjongg_export.h>

class KMahjonggConfigDialogPrivate;

class LIBKMAHJONGG_EXPORT KMahjonggConfigDialog : public KConfigDialog
{
Q_OBJECT
  public:
    KMahjonggConfigDialog( QWidget *parent, const QString& name,
                 KConfigSkeleton *config);
    ~KMahjonggConfigDialog();
    void addTilesetPage();
    void addBackgroundPage();

  protected Q_SLOTS:
    void updateWidgetsDefault() override;

  private:
    friend class KMahjonggConfigDialogPrivate;
    KMahjonggConfigDialogPrivate *const d;

    Q_DISABLE_COPY(KMahjonggConfigDialog)
};

#endif // KMAHJONGGCONFIGDIALOG_H
