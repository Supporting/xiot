#ifndef FI_FITYPES_H
#define FI_FITYPES_H

#if defined(_WIN32)
# if defined(openFI_EXPORTS)
#  define OPENFI_EXPORT __declspec(dllexport)
# else
#  define OPENFI_EXPORT __declspec(dllimport)
# endif
#else
# define OPENFI_EXPORT
#endif

#if defined(_MSC_VER) 
# pragma warning (disable: 4275) /* non-DLL-interface base class used */
# pragma warning (disable: 4251) /* needs to have dll-interface to be used by clients */
/* No warning for safe windows only functions */
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <vector>
#include <sstream>
#include <stdexcept>
#include <exception>

namespace FI {

	/// Constant that identifies that there is no index set for
	/// a table and that the ocetets need to be processed
	static const unsigned int INDEX_NOT_SET = 0;

	/// Possible encoding formats of the EncodedCharacterString
	enum EncodingFormat {
		ENCODINGFORMAT_UTF8,
		ENCODINGFORMAT_UTF16,
		ENCODINGFORMAT_RESTRICTED_ALPHABET,
		ENCODINGFORMAT_ENCODING_ALGORITHM
	} ;
	
	/** @defgroup ASN1Types ASN.1 types
	  * These are the implementations of the types 
	  * as defined in the chapter 7 of the 
	  * Fast InfoSet standard.
	  */



	/**
     * The NonEmptyOctetString type is:
	 * <code>NonEmptyOctetString ::= OCTET STRING (SIZE(1-four-gig))</code>
	 * @ingroup ASN1Types
     */
	typedef std::vector<char> NonEmptyOctetString;
	
	/**
     * 7.17 The EncodedCharacterString type
	 * @ingroup ASN1Types
     */
	struct EncodedCharacterString {
		EncodingFormat _encodingFormat;
		union {
			int _restrictedAlphabet;
			int _encodingAlgorithm;
		};
		NonEmptyOctetString _octets;
	};

	/**
	 * 7.13 The IdentifyingStringOrIndex type
	 * @ingroup ASN1Types
	 */
	struct IdentifyingStringOrIndex {
		IdentifyingStringOrIndex() : _stringIndex(INDEX_NOT_SET) {};
		NonEmptyOctetString _literalCharacterString;
		unsigned int _stringIndex;
	};

	/**
	 * 7.16 The QualifiedNameOrIndex type
	 * @ingroup ASN1Types
	 */
	struct QualifiedNameOrIndex {
		QualifiedNameOrIndex() : _nameSurrogateIndex(INDEX_NOT_SET) {};
		IdentifyingStringOrIndex _prefix;
		IdentifyingStringOrIndex _namespaceName;
		IdentifyingStringOrIndex _localName;
		unsigned int _nameSurrogateIndex;
	};

	/**
	 * 7.15 The NameSurrogate type
	 * @ingroup ASN1Types
	 */
	struct NameSurrogate {
		NameSurrogate() : _prefixStringIndex(INDEX_NOT_SET), _namespaceNameStringIndex(INDEX_NOT_SET), _localNameStringIndex(INDEX_NOT_SET) {};
		unsigned int _prefixStringIndex;
		unsigned int _namespaceNameStringIndex;
		unsigned int _localNameStringIndex;
	};
	
	/**
	 * 7.14 The NonIdentifyingStringOrIndex type
	 * @ingroup ASN1Types
	 */
	struct NonIdentifyingStringOrIndex {
		NonIdentifyingStringOrIndex() : _stringIndex(INDEX_NOT_SET), _addToTable(false) {};
		bool _addToTable;
		EncodedCharacterString _characterString;
		unsigned int _stringIndex;
	};



	/**
	 * 7.7 The CharacterChunk type
	 * @ingroup ASN1Types
	 */
	struct CharacterChunk {
		NonIdentifyingStringOrIndex characterCodes;
	};


	/**
	 * 7.4 The Attribute type
	 * @ingroup ASN1Types
	 */
	struct Attribute
	{
		QualifiedNameOrIndex _qualifiedName;
		NonIdentifyingStringOrIndex  _normalizedValue;
	};

	typedef std::vector<Attribute> Attributes;

	/**
	 * 7.3 The Element type
	 * @warning incomplete
	 * @ingroup ASN1Types
	 */
	struct Element
	{
		// std::vector<NamespaceAttribute> _namespaceAttribute;
		QualifiedNameOrIndex _qualifiedName;
		std::vector<Attribute> _attributes;
		// _children
	};

	/**
	 * 7.2 The Document type
	 * @ingroup ASN1Types
	 * @warning incomplete
	 */
	struct Document
	{

	};

	/**
	 * A resolved Qualified Name
	 */
	struct QualifiedName
	{
		QualifiedName(std::string prefix, std::string namespaceName, std::string localName) : _prefix(prefix), _namespaceName(namespaceName), _localName(localName) {};
		QualifiedName(std::string localName) : _prefix(""), _namespaceName(""), _localName(localName) {};
		std::string _prefix;
		std::string _namespaceName;
		std::string _localName;

		static inline std::string getQName(const std::string &p, const std::string &l) {
			if (p.empty()) return l;
			std::string result(p);
			return result.append(":").append(l);
		}
	};

	inline std::ostream& operator<<(std::ostream& o, const QualifiedName &qn)
	{
		return o << QualifiedName::getQName(qn._prefix, qn._localName).c_str();
	}



} // end namespace FI

#endif
