#ifndef FI_PARSERVOCABULARY_H
#define FI_PARSERVOCABULARY_H

#include "FITypes.h"
#include "FIEncodingAlgorithms.h"
#include <string>
#include <fstream>
#include <iostream>
#include <map>

namespace FI {


/**
 * Abstract class that defines a ParserVocabulary
 */
class OPENFI_EXPORT ParserVocabulary 
{
public:
	virtual ~ParserVocabulary() {};

	virtual QualifiedName resolveElementName(const QualifiedNameOrIndex &input) const;
	virtual QualifiedName resolveAttributeName(const QualifiedNameOrIndex &input) const;
	virtual QualifiedName resolveQualifiedName(const QualifiedNameOrIndex &input) const;

	virtual std::string	  resolveAttributeValue(const NonIdentifyingStringOrIndex &input) const;
	virtual std::string	  resolveCharacterChunk(const NonIdentifyingStringOrIndex &input) const;
	

	virtual std::string	  decodeCharacterString(const EncodedCharacterString &input) const;

	virtual QualifiedName getElementName(unsigned int index) const = 0;
	virtual QualifiedName getAttributeName(unsigned int index) const = 0;
	
	virtual std::string getPrefix(unsigned int index) const = 0;
	virtual std::string getNamespaceName(unsigned int index) const = 0;
	virtual std::string getLocalName(unsigned int index) const = 0;
	virtual std::string getAttributeValue(unsigned int index) const = 0;
	virtual std::string getCharacterChunk(unsigned int index) const = 0;
	virtual IEncodingAlgorithm* getEncodingAlgorithm(unsigned int index) const = 0;
	
	virtual void addAttributeValue(std::string value) = 0;
	virtual void addCharacterChunk(std::string value) = 0;
	virtual void addEncodingAlgorithm(IEncodingAlgorithm* algorithm) = 0;
	virtual std::string getExternalVocabularyURI() const = 0;
};

/**
 * The DefaultParserVocabulary implements all the needed functionality of a ParserVocabulary
 * including all the table that are supported yet.
 */
class OPENFI_EXPORT DefaultParserVocabulary : public ParserVocabulary
{
public:
	DefaultParserVocabulary();
	DefaultParserVocabulary(const char* uri);
	virtual ~DefaultParserVocabulary() {};

	virtual inline QualifiedName getElementName(unsigned int index) const { return _elementNames.at(index-1); };
	virtual QualifiedName getAttributeName(unsigned int index) const { return _attributeNames.at(index-1); };

	virtual inline std::string getPrefix(unsigned int index) const { return _prefixNames.at(index-1); };
	virtual inline std::string getNamespaceName(unsigned int index)  const { return _nameSpaceNames.at(index-1); };
	virtual inline std::string getLocalName(unsigned int index) const { return _localNames.at(index-1); };
	virtual inline std::string getAttributeValue(unsigned int index) const { return _attributeValues.at(index-1); };
	virtual inline std::string getCharacterChunk(unsigned int index) const { return _characterChunks.at(index-1); };
	virtual IEncodingAlgorithm* getEncodingAlgorithm(unsigned int index) const;

	virtual void addAttributeValue(std::string value);
	virtual void addCharacterChunk(std::string value);
	virtual void addEncodingAlgorithm(IEncodingAlgorithm* algorithm);

	virtual inline std::string getExternalVocabularyURI() const { return _externalVocabularyURI; };

protected:

	virtual void initEncodingAlgorithms();

	// Tables
	std::vector<QualifiedName> _elementNames;
	std::vector<QualifiedName> _attributeNames;
	std::vector<std::string> _prefixNames;
	std::vector<std::string> _nameSpaceNames;
	std::vector<std::string> _localNames;
	std::vector<std::string> _attributeValues;
	std::vector<std::string> _characterChunks;
	std::vector<IEncodingAlgorithm*> _encodingAlgorithms;
	
	IntEncodingAlgorithm _intEncodingAlgorithm;
	FloatEncodingAlgorithm _floatEncodingAlgorithm;

	std::string _externalVocabularyURI;
private:
	
	
};
}

#endif
