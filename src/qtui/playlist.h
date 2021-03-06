/*
 * playlist.h
 * Copyright 2014 Michał Lipski
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions, and the following disclaimer in the documentation
 *    provided with the distribution.
 *
 * This software is provided "as is" and without any warranty, express or
 * implied. In no event shall the authors be liable for any damages arising from
 * the use of this software.
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QTreeView>

#include <libaudcore/hook.h>
#include <libaudcore/playlist.h>

#include "playlist_model.h"
#include "filter_input.h"

class QSortFilterProxyModel;

class PlaylistWidget : public QTreeView
{
public:
    PlaylistWidget (QTreeView * parent = nullptr, int uniqueId = -1);
    ~PlaylistWidget ();
    void scrollToCurrent ();
    void updatePlaybackIndicator ();
    void update (const Playlist::Update & update);
    void playCurrentIndex ();
    void deleteCurrentSelection ();
    void setFilter (const QString & text);
    void toggleQueue ();
    int playlist () const;
    int uniqueId () const;

private:
    PlaylistModel * model;
    QSortFilterProxyModel * proxyModel;
    int currentPos = -1;
    bool inUpdate = false;
    bool needIndicatorUpdate = false;
    bool scrollQueued = false;

    QModelIndex rowToIndex (int row);
    int indexToRow (const QModelIndex & index);

    void getSelectedRanges (const Playlist::Update & update,
     QItemSelection & selected, QItemSelection & deselected);

    void keyPressEvent (QKeyEvent * event);
    void mousePressEvent (QMouseEvent * event);
    void mouseDoubleClickEvent (QMouseEvent * event);
    void currentChanged (const QModelIndex & current, const QModelIndex & previous);
    void selectionChanged (const QItemSelection & selected, const QItemSelection & deselected);

    void updateSettings ();
    const HookReceiver<PlaylistWidget>
     settings_hook {"qtui update playlist settings", this, & PlaylistWidget::updateSettings};
};

#endif
