/* This file is part of the KDE project
 * Copyright (C) 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ANCHORSTRATEGY_H_
#define ANCHORSTRATEGY_H_

#include "KoTextAnchor.h"

class KoTextShapeContainerModel;
class KoTextShapeData;
class KoTextLayoutRootArea;
class KoShape;
class QTextBlock;
class QTextLayout;


class AnchorStrategy  : public KoAnchorStrategy
{
public:
    AnchorStrategy(KoTextAnchor *anchor, KoTextLayoutRootArea *rootArea);
    virtual ~AnchorStrategy();

    /**
     * Moves the obstruction to it's right position.
     *
     * @return true if new position for shape was found
     */
    virtual bool moveObstruction() = 0;

    virtual void detachFromModel();

    virtual void updatePosition(KoShape *shape, const QTextDocument *document, int position);

private:
    KoTextShapeContainerModel *m_model;
    KoTextAnchor * const m_anchor;
    KoTextLayoutRootArea *m_rootArea;
};

#endif /* INLINEANCHORSTRATEGY_H_ */