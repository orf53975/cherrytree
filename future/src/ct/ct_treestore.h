/*
 * ct_treestore.h
 *
 * Copyright 2017-2019 Giuseppe Penone <giuspen@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include <set>

class CtAnchoredWidget : public Gtk::EventBox
{
public:
    CtAnchoredWidget(const int& charOffset, const std::string& justification);
    void insertInTextBuffer(Glib::RefPtr<Gsv::Buffer> rTextBuffer);
    Glib::RefPtr<Gtk::TextChildAnchor> getTextChildAnchor() { return _rTextChildAnchor; }
    virtual void applyWidthHeight(int parentTextWidth) {}
protected:
    Gtk::Frame _frame;
    Gtk::Label _labelWidget;
    int _charOffset;
    std::string _justification;
    Glib::RefPtr<Gtk::TextChildAnchor> _rTextChildAnchor;
};

struct CtNodeData
{
    gint64         nodeId{0};
    Glib::ustring  name;
    std::string    syntax;
    Glib::ustring  tags;
    bool           isRO{false};
    guint32        customIconId{0};
    bool           isBold{false};
    bool           fgOverride{false};
    char           foregroundRgb24[8];
    gint64         tsCreation{0};
    gint64         tsLastSave{0};
    Glib::RefPtr<Gsv::Buffer>  rTextBuffer{nullptr};
    std::list<CtAnchoredWidget*> anchoredWidgets;
};

class CtTreeModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    CtTreeModelColumns()
    {
        add(rColPixbuf); add(colNodeName); add(rColTextBuffer); add(colNodeUniqueId);
        add(colSyntaxHighlighting); add(colNodeSequence); add(colNodeTags); add(colNodeRO);
        add(rColPixbufAux); add(colCustomIconId); add(colWeight); add(colForeground);
        add(colTsCreation); add(colTsLastSave); add(colAnchoredWidgets);
    }
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>>  rColPixbuf;
    Gtk::TreeModelColumn<Glib::ustring>              colNodeName;
    Gtk::TreeModelColumn<Glib::RefPtr<Gsv::Buffer>>  rColTextBuffer;
    Gtk::TreeModelColumn<gint64>                     colNodeUniqueId;
    Gtk::TreeModelColumn<std::string>                colSyntaxHighlighting;
    Gtk::TreeModelColumn<guint16>                    colNodeSequence;
    Gtk::TreeModelColumn<Glib::ustring>              colNodeTags;
    Gtk::TreeModelColumn<bool>                       colNodeRO;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>>  rColPixbufAux;
    Gtk::TreeModelColumn<guint16>                    colCustomIconId;
    Gtk::TreeModelColumn<guint16>                    colWeight;
    Gtk::TreeModelColumn<Glib::ustring>              colForeground;
    Gtk::TreeModelColumn<gint64>                     colTsCreation;
    Gtk::TreeModelColumn<gint64>                     colTsLastSave;
    Gtk::TreeModelColumn<std::list<CtAnchoredWidget*>> colAnchoredWidgets;
};

class CtTextView;
class CtSQLiteRead;

class CtTreeStore : public sigc::trackable
{
public:
    CtTreeStore();
    virtual ~CtTreeStore();

    void          viewConnect(Gtk::TreeView* pTreeView);
    void          viewAppendColumns(Gtk::TreeView* pTreeView);
    bool          readNodesFromFilepath(const char* filepath, const bool isImport, const Gtk::TreeIter* pParentIter=nullptr);
    Gtk::TreeIter appendNode(CtNodeData* pNodeData, const Gtk::TreeIter* pParentIter=nullptr);

    void          onRequestAddBookmark(gint64 nodeId);
    Gtk::TreeIter onRequestAppendNode(CtNodeData* pNodeData, const Gtk::TreeIter* pParentIter);

    void applyTextBufferToCtTextView(const Gtk::TreeIter& treeIter, CtTextView* pTextView);
    const Gtk::TreeModel::Children getRootChildren() { return _rTreeStore->children(); }
    void setExpandedCollapsed(Gtk::TreeView* pTreeView,
                              const Gtk::TreeModel::Children& children,
                              const std::map<gint64,bool>& mapExpandedCollapsed);
    void expandToTreeRow(Gtk::TreeView* pTreeView, Gtk::TreeRow& row);
    void setTreePathTextCursorFromConfig(Gtk::TreeView* pTreeView, Gsv::View* pTextView);
    void treeviewSafeSetCursor(Gtk::TreeView* pTreeView, Gtk::TreeIter& iter);

protected:
    guint16                   _getPangoWeight(bool isBold);
    Glib::RefPtr<Gdk::Pixbuf> _getNodeIcon(int nodeDepth, std::string &syntax, guint32 customIconId);
    void                      _iterDeleteAnchoredWidgets(const Gtk::TreeModel::Children& children);

    Glib::RefPtr<Gsv::Buffer> _getNodeTextBuffer(const Gtk::TreeIter& treeIter);

    CtTreeModelColumns           _columns;
    Glib::RefPtr<Gtk::TreeStore> _rTreeStore;
    std::set<gint64>             _bookmarks;
    CtSQLiteRead*                _pCtSQLiteRead{nullptr};
};
