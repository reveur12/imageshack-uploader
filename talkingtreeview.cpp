/*
Copyright (c) 2009, ImageShack Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the ImageShack nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "talkingtreeview.h"
#include "medialistmodel.h"
#include <QPainter>
#include <QKeyEvent>

TalkingTreeView::TalkingTreeView(QWidget *w)
{
    setParent(w);
}

void TalkingTreeView::paintEvent ( QPaintEvent * event )
{
    QTreeView::paintEvent(event);
    if (model()->rowCount()==0)
    {
        QPainter painter (this->viewport ());
        int w = this->viewport ()->width();
        int h = this->viewport ()->height();
        QFont f = painter.font();
        f.setItalic(true);
        painter.setFont(f);
        QFontMetrics fm(f);
        QString text = tr("Drag and drop one or more image\\n or video files here");
        QStringList list = text.split("\\n");
        if (list.size()<2)
            return ;
        text = list.at(0);
        QString text2 = list.at(1);
        int textWidth = fm.width(text);
        int textWidth2 = fm.width(text2);
        int posX = w/2 - (textWidth/2);
        int posY = h/2 - fm.height()/2;
        int posX2 = w/2 - (textWidth2/2);
        int posY2 = h/2 - fm.height()/2 +fm.height();

        painter.drawText (posX, posY, text);
        painter.drawText (posX2, posY2, text2);
    }
}

void TalkingTreeView::keyPressEvent(QKeyEvent* event)
{
    if ( !(event->key() == Qt::Key_Delete)) return QTreeView::keyPressEvent(event);
    QModelIndexList rows = this->selectionModel()->selectedRows(0);
    if (!rows.isEmpty())
    {
        ((MediaListModel*)(this->model()))->removeMedia(rows.at(0).row());
    }
}
