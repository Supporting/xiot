#include <iostream>
#include <sstream>
#include <xiot/FIContentHandler.h>
#include <xiot/FIParserVocabulary.h>

using namespace std;

namespace FI {

void DefaultContentHandler::startDocument() {
}

void DefaultContentHandler::endDocument() {
}

void DefaultContentHandler::startElement(const ParserVocabulary *, const Element &, const Attributes &) {
}

void DefaultContentHandler::endElement(const ParserVocabulary *, const Element &) {
}

void DefaultContentHandler::characters(const ParserVocabulary *, const CharacterChunk &) {
}


}  // namespace FI
