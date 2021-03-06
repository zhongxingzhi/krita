/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_XMP_IO_H_
#define _KIS_XMP_IO_H_

#include <metadata/kis_meta_data_io_backend.h>

#include <klocalizedstring.h>

class KisXMPIO : public KisMetaData::IOBackend
{
    struct Private;
public:
    KisXMPIO();
    virtual ~KisXMPIO();
    virtual QString id() const {
        return "xmp";
    }
    virtual QString name() const {
        return i18n("XMP");
    }
    virtual BackendType type() const {
        return Text;
    }
    virtual bool supportSaving() const {
        return true;
    }
    virtual bool saveTo(KisMetaData::Store* store, QIODevice* ioDevice, HeaderType headerType = NoHeader) const;
    virtual bool canSaveAllEntries(KisMetaData::Store*) const {
        return true;
    }
    virtual bool supportLoading() const {
        return true;
    }
    virtual bool loadFrom(KisMetaData::Store* store, QIODevice* ioDevice) const;
};

#endif
