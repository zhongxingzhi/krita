/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KOSECTION_H
#define KOSECTION_H

class KoXmlElement;
class KoShapeSavingContext;
class KoTextSharedLoadingData;
/**
 * Contains the information about the current text:section.
 *
 * The <text:section> element has the following attributes:
 *
 * <ul>
 * <li>text:condition
 * <li>text:display
 * <li>text:name
 * <li>text:protected
 * <li>text:protection-key
 * <li>text:protection-key-digest-algorithm
 * <li>text:style-name
 * <li>xml:id
 * </ul>
 * (odf spec v.12)
 */
class KoSection
{
public:

    KoSection();
    ~KoSection();

    bool loadOdf(const KoXmlElement &element, KoTextSharedLoadingData *sharedData, bool stylesDotXml);
    void saveOdf(KoShapeSavingContext &context);
private:
    class Private;
    Private * const d;

};

#endif // KOSECTION_H