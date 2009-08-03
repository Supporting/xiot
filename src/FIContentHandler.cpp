#include <xiot/FIContentHandler.h>
#include <xiot/FIParserVocabulary.h>
#include <iostream>
#include <sstream>

using namespace std;

namespace FI {

void DefaultContentHandler::startDocument()
{
}

void DefaultContentHandler::endDocument()
{
}

void DefaultContentHandler::startElement(const ParserVocabulary* vocab, const Element &element, const Attributes &attributes)
{
}

void DefaultContentHandler::endElement(const ParserVocabulary* vocab, const Element &element)
{
}

void DefaultContentHandler::characters(const ParserVocabulary* vocab, const CharacterChunk &chunk)
{
}


}
