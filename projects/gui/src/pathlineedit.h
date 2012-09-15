#ifndef PATHLINEEDIT_H
#define PATHLINEEDIT_H

#include <QLineEdit>
class QToolButton;

class PathLineEdit : public QLineEdit
{
	Q_OBJECT

	public:
		enum PathType
		{
			FilePath,
			FolderPath
		};

		explicit PathLineEdit(PathType pathType, QWidget* parent = 0);

	protected:
		virtual void resizeEvent(QResizeEvent* event);

	private slots:
		void browse();

	private:
		PathType m_pathType;
		QToolButton* m_browseBtn;
};

#endif // PATHLINEEDIT_H
