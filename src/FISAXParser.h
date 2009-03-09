#ifndef FI_SAXPARSER_H
#define FI_SAXPARSER_H

#include "FITypes.h"
#include "FIDecoder.h"
#include <string>
#include <fstream>
#include <map>

namespace FI {

class ContentHandler;
class ParserVocabulary;

/**
 * The SAXParser is a specialization of the Decoder that implements callback
 * at those states defined by the SAX mechanism. The given ContentHandler can
 * process those callbacks. The Parser also gives a reference to the current 
 * vocabulary.
 *
 * @see ContentHandler
 */
class OPENFI_EXPORT SAXParser : public Decoder
{
public:
  /// Constructor.
  SAXParser() {};
  /// Destructor.
  virtual ~SAXParser();

  virtual void parse();

  void setContentHandler(ContentHandler* handler);

protected:
  virtual void processDocument();
  virtual void processElement();
  virtual void processAttributes();
  virtual void processCharacterChunk();

  /**
    * Reference to content handler.
    */
  ContentHandler* _contentHandler;

private:
  bool _terminated;
  bool _doubleTerminated;
  Attributes _attributes;
};

}

#endif
