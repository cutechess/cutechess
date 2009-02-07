/*
   A command-line tool for removing unneeded data from a Doxygen tagfile.
   The tool's main purpose is to trim the Qt library's tagfile to a manageable
   size, but it may work with other tagfiles as well.
*/

#include <QtCore>
#include <QtXml>
#include <QtDebug>


/*!
 * Remove the Doxygen-generated "class", "namespace", or whatever prefix
 * from a file name, and make it lowercase.
 */
static void fixFilename(QDomElement& node, const QString& compoundKind)
{
	QDomNode child = node.firstChild();
	QString text = child.nodeValue();
	if (text.startsWith(compoundKind))
	{
		int len = compoundKind.length();
		text = text.mid(len).toLower();
		child.setNodeValue(text);
	}
}

/*! Remove duplicate members from the compound. */
static void trimMembers(QDomElement& compound)
{
	QDomElement elem;
	for (elem = compound.firstChildElement("member");
	     !elem.isNull();
	     elem = elem.nextSiblingElement("member"))
	{
		QDomElement nameElem1 = elem.firstChildElement("name");
		
		QDomElement elem2 = elem.nextSiblingElement("member");
		while (!elem2.isNull())
		{
			// If two members have a "name" tag with the same
			// value, the second one of them gets removed.
			QDomElement nameElem2 = elem2.firstChildElement("name");
			if (nameElem2.text() == nameElem1.text())
			{
				QDomElement old = elem2;
				elem2 = elem2.nextSiblingElement("member");
				compound.removeChild(old);
				continue;
			}
			elem2 = elem2.nextSiblingElement("member");
		}
	}
}

/*! Remove unneeded nodes from a compound. */
static void trimCompound(QDomElement& compound)
{
	QDomElement elem = compound.firstChildElement();
	while (!elem.isNull())
	{
		QString name = elem.tagName();
		if (name == "filename")
			fixFilename(elem, compound.attribute("kind"));
		else if (name == "member"
		     &&  elem.attribute("kind") == "enumeration")
		{
			QDomElement tmp(elem.firstChildElement("anchorfile"));
			if (!tmp.isNull())
				fixFilename(tmp, compound.attribute("kind"));
			
			// The "arglist" tag is never used in class or
			// namespace compounds.
			tmp = elem.firstChildElement("arglist");
			if (!tmp.isNull())
				elem.removeChild(tmp);
		}
		else if (name != "name")
		{
			QDomElement old = elem;
			elem = elem.nextSiblingElement();
			compound.removeChild(old);
			continue;
		}
		elem = elem.nextSiblingElement();
	}
	
	trimMembers(compound);
}

/*! Trim out useless weight from the tagfile. */
static bool trimDoc(QDomDocument& doc)
{
	QDomElement root = doc.documentElement();
	if (root.tagName() != "tagfile")
		return false;
	
	QDomElement compound = root.firstChildElement();
	while (!compound.isNull())
	{
		QString compoundKind = compound.attribute("kind");
		if (compound.tagName() != "compound"
		||  (compoundKind != "class" && compoundKind != "namespace"))
		{
			QDomElement old = compound;
			compound = compound.nextSiblingElement();
			root.removeChild(old);
			continue;
		}
		
		trimCompound(compound);
		compound = compound.nextSiblingElement();
	}
	
	return true;
}

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();
	if (args.size() <= 2)
	{
		qDebug() << "Usage: tagtrimmer SOURCE_FILE DEST_FILE";
		return 1;
	}
	
	QFile file(args[1]);
	if (!file.open(QIODevice::ReadOnly))
	{
		qWarning() << "Can't open file" << args[1];
		return 1;
	}
	
	QDomDocument doc;
	QString errorMsg;
	int line, col;
	if (!doc.setContent(&file, &errorMsg, &line, &col))
	{
		qWarning() << "Error in line" << line << "column" << col;
		qWarning() << errorMsg;
		return 1;
	}
	file.close();
	
	if (!trimDoc(doc))
	{
		qWarning() << "Couldn't trim the Doxytag file" << args[1];
		return 1;
	}

	QFile file2(args[2]);
	if (!file2.open(QIODevice::WriteOnly))
	{
		qWarning() << "Can't open file" << args[2];
		return 1;
	}
	QTextStream out(&file2);
	doc.save(out, 2);

	return 0;
}

