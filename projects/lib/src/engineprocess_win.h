/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ENGINEPROCESS_WIN_H
#define ENGINEPROCESS_WIN_H

#include <windows.h>
#include <QIODevice>
#include <QString>
#include <QMutex>
class PipeReader;


/*!
 * \brief A replacement for QProcess on Windows
 *
 * Due to polling, QProcess' response times on Windows are too slow for
 * chess engines. EngineProcess is a different implementation which reads
 * new data immediately (no polling) when it's available. The interface is
 * the same as QProcess' with some unneeded features left out.
 *
 * On non-Windows platforms EngineProcess is just a typedef to QProcess.
 *
 * \sa QProcess
 * \sa PipeReader
 */
class LIB_EXPORT EngineProcess : public QIODevice
{
	Q_OBJECT

	public:
		/*! The process' exit status. */
		enum ExitStatus
		{
			NormalExit,	//!< The process exited normally
			CrashExit	//!< The process crashed
		};

		/*! Creates a new EngineProcess. */
		explicit EngineProcess(QObject* parent = nullptr);
		/*!
		 * Destructs the EngineProcess and frees all resources.
		 * If the process is still running, it is killed.
		 */
		virtual ~EngineProcess();

		// Inherited from QIODevice
		virtual qint64 bytesAvailable() const;
		virtual bool canReadLine() const;
		virtual void close();
		virtual bool isSequential() const;

		/*! Returns the exit code of the last process that finished. */
		int exitCode() const;
		/*! Returns the exit status of the last process that finished. */
		ExitStatus exitStatus() const;

		/*!
		 * Returns the process' working directory.
		 * Returns an empty string if the working directory wasn't
		 * set with setWorkingDirectory().
		 */
		QString workingDirectory() const;
		/*!
		 * Sets the working directory to dir.
		 * EngineProcess will start the process in this directory.
		 */
		void setWorkingDirectory(const QString& dir);
		/*!
		 * Redirects the process' standard error to the file fileName.
		 * The file will be appended to if mode is Append; otherwise
		 * it will be truncated.
		 */
		void setStandardErrorFile(const QString& fileName,
					  OpenMode mode = Truncate);

		/*!
		 * Starts the program \a program in a new process, passing the
		 * command line arguments in \a arguments. The OpenMode is set
		 * to \a mode.
		 *
		 * \note Unlike the same function in QProcess, this one will
		 * block until the process has started.
		 *
		 * \note To check if the process started successfully, call
		 * the waitForStarted() method.
		 */
		void start(const QString& program,
			   const QStringList& arguments,
			   OpenMode mode = ReadWrite);
		/*! Starts the program \a program with OpenMode \a mode. */
		void start(const QString& program,
			   OpenMode mode = ReadWrite);

		/*!
		 * Blocks until the process has finished and the finished()
		 * signal has been emitted.
		 *
		 * Times out after \a msecs milliseconds. If \a msecs is -1
		 * the function will not time out.
		 *
		 * \return true if the process finished.
		 */
		bool waitForFinished(int msecs = 30000);

		/*!
		 * Returns true if the process started successfully.
		 * Doesn't really wait for anything since the start() method
		 * already did the waiting.
		 */
		bool waitForStarted(int msecs = 30000);

	public slots:
		/*! Kills the process, causing it to exit immediately. */
		void kill();

	signals:
		/*!
		 * Emitted when the process finishes.
		 * \param exitCode exit code of the process
		 * \param exitStatus exit status of the process
		 */
		void finished(int exitCode, ExitStatus exitStatus);

	protected:
		// Inherited from QIODevice
		virtual qint64 readData(char* data, qint64 maxSize);
		virtual qint64 writeData(const char* data, qint64 maxSize);

	private slots:
		void onFinished();

	private:
		static QString quote(QString str);
		static QString unquote(QString str);
		static QString cmdLine(const QString& wdir,
				       const QString& prog,
				       const QStringList& args);
		static HANDLE createFile(const QString& fileName, OpenMode mode);
		static HANDLE mainJob();

		static HANDLE s_job;
		static QMutex s_mutex;

		void cleanup();
		void killHandle(HANDLE* handle);

		bool m_started;
		bool m_finished;
		DWORD m_exitCode;
		ExitStatus m_exitStatus;
		QString m_workDir;
		QString m_stdErrFile;
		OpenMode m_stdErrFileMode;
		PROCESS_INFORMATION m_processInfo;
		HANDLE m_inWrite;
		HANDLE m_outRead;
		HANDLE m_errRead;
		PipeReader* m_reader;
};

#endif // ENGINEPROCESS_WIN_H
