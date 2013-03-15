#include "mime.h"
#include "log.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

Mime * Mime::m_instance = NULL;

Mime & Mime::instance()
{
    if (m_instance == NULL)
        m_instance = new Mime();
    return * m_instance;
}

Mime::Mime()
{
    QFile mime_type_file(MIME_TYPE_FILE);
    if (!mime_type_file.open(QIODevice::ReadOnly))
    {
        Log::instance() << "Can not open mime type defination file." << Log::NEWLINE << Log::FLUSH;
        return;
    }

    QTextStream stream(&mime_type_file);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        int pos = line.indexOf('\t');
        if (pos == -1)
        {
            Log::instance() << "Mime type defination file format error." << Log::NEWLINE << Log::FLUSH;
            continue;
        }

        QString mime_type = line.mid(0, pos);
        QStringList exts = line.mid(pos + 1).split(' ');

        for (QStringList::iterator i = exts.begin(); i != exts.end(); i++)
        {
            mime_types[*i] = mime_type;
        }
    }
    mime_type_file.close();
}

QString Mime::getMimeType(QString extension)
{
    if (mime_types.contains(extension))
        return mime_types[extension];
    return mime_types[""];
}
