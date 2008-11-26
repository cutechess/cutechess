/*
    Copyright (c) 2008 Arto Jonsson

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QtDesigner>
#include <QtGui>
#include <QtPlugin>

#include <HintLineEdit>

#include "hintlineeditplugin.h"

HintLineEditPlugin::HintLineEditPlugin(QObject* parent)
	: QObject(parent)
{
	m_initialized = false;
}

QString HintLineEditPlugin::name() const
{
	return "HintLineEdit";
}

QString HintLineEditPlugin::group() const
{
	return "Input Widgets";
}

QString HintLineEditPlugin::toolTip() const
{
	return "";
}

QString HintLineEditPlugin::whatsThis() const
{
	return "";
}

QString HintLineEditPlugin::includeFile() const
{
	return "HintLineEdit";
}

QIcon HintLineEditPlugin::icon() const
{
	return QIcon();
}

bool HintLineEditPlugin::isContainer() const
{
	return false;
}

QWidget* HintLineEditPlugin::createWidget(QWidget* parent)
{
	return new HintLineEdit(parent);
}

bool HintLineEditPlugin::isInitialized() const
{
	return m_initialized;
}

void HintLineEditPlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
	if (m_initialized)
		return;

	m_initialized = true;
}

Q_EXPORT_PLUGIN2(hintlineedit, HintLineEditPlugin)

