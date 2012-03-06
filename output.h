#ifndef OUTPUT_H
#define OUTPUT_H

#include "parameters.h"
#include <QTextStream>
#include <QObject>
#include <QFile>

//! put a newline character in the stream that ignores the streams current FieldWidth
QTextStream & newline(QTextStream& s);

//! put a space in the stream that ignores the streams current FieldWidth
QTextStream & space(QTextStream& s);

namespace Langmuir
{

//! Back up a file
/*!
  Back up the file using the current time and a revision number.
  The file is backed up as path/file.date.num, where num is
  determined by examing existing files in path with a similiar
  form (path/file.current_date.a_number).  The file is renamed,
  not copied.
  \param name a relative or absolute file name path
  \warning gives an error if a directory is passed instead of a file
  \warning gives an error if the file can not be renamed
*/
void backupFile(const QString& name);

//! A Class to generate file names using the SimulationParameters
class OutputInfo : public QFileInfo
{
public:
    //! Flag to be passed to constructor
    enum Option
    {
        AppendMode = 1 //<! The QFile will be appended to, so it is allowed to already exist
    };
    Q_DECLARE_FLAGS(Options,Option)

    //! Generate file name according to SimulationParameters
    /*!
      The constructor makes useful substitutions into the passed name (deatiled below)
      as well as making sure the name generated is valid (according to the passed
      SimulationParameters).  If the directory of the passed name doesn't exist, it
      will be created.
      \param name the file name desired.  The following substitutions can be made:
        - \b "%path", substitutes in SimulationParameters::outputPath
        - \b "%stub", substitutes in SimulationParameters::outputStub
        - \b "%step", substitutes in SimulationParameters::currentStep
        - \b "%sim" , substitutes in SimulationParameters::currentSimulation
      \param par pointer to a SimulationParameters object
        - if 0 or NULL, then all substitutions become empty strings
      \param options flags to determine certain constructor behavoirs
        - Option::AppendMode though no file is opened, lets the constructor know if
        we will being appending to the file, so that no errors are thrown if the
        file already exists and SimulationParameters::outputOverwrite is false
      \warning \b "%path" must be at the start of the name and only appear once
      */
    OutputInfo(const QString &name,
               const SimulationParameters *par = 0,
               Options options = 0);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(OutputInfo::Options)

//! A Class to combine QFile, QTextStream and OutputInfo (QFileInfo).
/*! Only for used for output.  Derived from QObject so destruction
  ensures streams are flushed and files are closed.
  */
class OutputStream : public QObject, public QTextStream
{
    Q_OBJECT
public:

    //! Setup the QTextStream, QFile, and OutputInfo
    /*!
      The parameters are the same as OutputInfo.  Opens the file as
      QIODevice::Text|QIODevice::WriteOnly.  Will open with QIODevice::Append
      if Outout::Options::AppendMode is given.
      \sa OutputInfo::OutputInfo
     */
    OutputStream(const QString &name,
               const SimulationParameters *par = 0,
               OutputInfo::Options options = 0,
               QObject *parent = 0);

    //! Flush the stream and close the file
   ~OutputStream();

    //! Get the info object to get things like file name and path
    const OutputInfo& info();

    //! Get the file object, though you probably have no need for it
    const QFile& file();

private:
    OutputInfo m_info; //!< OutputInfo object that generated file name
    QFile m_file; //!< QFile object, the device of this QTextStream
};

}
#endif // OUTPUT_H