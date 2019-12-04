/* $Id: simplexml.c,v 1.1.1.1 2002/08/23 10:38:57 essmann Exp $
 *
 * Copyright (c) 2001-2002 Bruno Essmann <essmann@users.sourceforge.net>
 * All rights reserved.
 */

/* ---- includes */

#include <stdlib.h>
#include <string.h>
#include "simplexml.h"

/* ---- definitions */

/* result code for various functions indicating a failure */
#define FAIL 0
/* result code for various functions indiciating a success */
#define SUCCESS 1

/* error code value indicating that everything is ok */
#define NO_ERROR 0
/* error code value indicating that parsing hasn't started yet */
#define NOT_PARSED 1
/* error code value indicating that the parser couldn't
 * grow one of its internal structures using malloc */
#define OUT_OF_MEMORY 2
/* error code value indicating that the XML data was early 
 * terminated, i.e. the parser was expecting more data */
#define EARLY_TERMINATION 3
/* error code value indicating that the ampersand character
 * was used in an illegal context */
#define ILLEGAL_AMPERSAND 4
/* error code indicating that a unicode character was read,
 * this parser does not support unicode characters (only
 * 8-bit values) */
#define NO_UNICODE_SUPPORT 5
/* indicates that the parser expected a greater than (>)
   character but read something else */
#define GREATER_THAN_EXPECTED 6
/* indicates that the parser expected a quote (' or ")
   character but read something else */
#define QUOTE_EXPECTED 7
/* indicates that an illegal handler was passed to the parser */
#define ILLEGAL_HANDLER 8
/* indicates that the parser was reused without proper initialization */
#define NOT_INITIALIZED 9
/* indicates that the document doesn't have a single tag */
#define NO_DOCUMENT_TAG 10
/* indicates that the parser encountered an illegal end tag */
#define MISMATCHED_END_TAG 11
/* indicates that the parser was expecting an attribute but
   instead read some other token type */
#define ATTRIBUTE_EXPECTED 12
/* indicates that the parser was expecting an equal sign (=)
   but some other character was read */
#define EQUAL_SIGN_EXPECTED 13

/* next token type describing the opening of a begin tag */
#define TAG_BEGIN_OPENING 0
/* next token type describing the closing of a begin tag */
#define TAG_BEGIN_CLOSING 1
/* next token type describing an end tag */
#define TAG_END 2
/* next token type describing a processing instruction <?...?> */
#define PROCESSING_INSTRUCTION 3
/* next token type describing a document type <!DOCTYPE...> */
#define DOCTYPE 4
/* next token type describing a comment <!--...--> */
#define COMMENT 5
/* next token type describing an attribute */
#define ATTRIBUTE 6
/* next token type describing tag content */
#define CONTENT 7
/* next token type describing an unknown <!xxx> tag */
#define UNKNOWN 8

/* the space constants */
#define SPACE ' '
/* the linefeed constant */
#define LF '\xa'
/* the carriage return constant */
#define CR '\xd'

static char* mystrdup(const char* src)
{
	if (src != NULL)
	{
		char* dst = (char*)malloc(strlen(src) + 1);
		strcpy(dst, src);
		return dst;
	}
	else
		return NULL;
}

/* ---- types */

/**
 * Value buffer.
 * This structure resembles a string buffer that
 * grows automatically when inserting data.
 */
typedef struct simplexml_value_buffer {
	/* buffer data */
	char* sBuffer;
	/* size of the buffer */
	long nSize;
	/* insert position in buffer */
	long nPosition;
} TSimpleXmlValueBuffer, *SimpleXmlValueBuffer;

/**
 * User data stack.
 * This structure is a simple stack for user data.
 */
typedef struct simplexml_user_data {
	void* pData;
	struct simplexml_user_data* next;
} TSimpleXmlUserData, *SimpleXmlUserData;

/**
 * SimpleXmlParser internal state.
 *
 * This structure holds all data necessary for the
 * simple xml parser to operate.
 * 
 * This struct describes the internal representation
 * of a SimpleXmlParserState.
 */
typedef struct simplexml_parser_state {
	/* error code if != NO_ERROR */
	int nError;
	/* value of the next token */
	SimpleXmlValueBuffer vbNextToken;
	/* next token type */
	int nNextToken;
	/* the name of an attribute read */
	char *szAttribute;
	/* allocated buffer size for attribute name */
	int nAttributeBufferSize;
	/* input data buffer */
	char *sInputData;
	/* length of the input data buffer */
	long nInputDataSize;
	/* read cursor on input data */
	long nInputDataPos;
	/* line number at cursor position */
	long nInputLineNumber;
	/* the user data pointer */
	SimpleXmlUserData pUserData;
} TSimpleXmlParserState, *SimpleXmlParserState;

/* ---- prototypes */

SimpleXmlParserState createSimpleXmlParser (const char *sData, long nDataSize);
void destroySimpleXmlParser (SimpleXmlParserState parser);
int initializeSimpleXmlParser (SimpleXmlParserState parser, const char *sData, long nDataSize);
const char* getSimpleXmlParseErrorDescription (SimpleXmlParserState parser);
int parseSimpleXml (SimpleXmlParserState parser, SimpleXmlTagHandler handler);
int parseOneTag (SimpleXmlParserState parser, SimpleXmlTagHandler parentHandler);
int readNextTagToken (SimpleXmlParserState parser);
int readNextContentToken (SimpleXmlParserState parser);
int readChar (SimpleXmlParserState parser);
char peekInputCharAt (SimpleXmlParserState parser, int nOffset);
char peekInputChar (SimpleXmlParserState parser);
int skipWhitespace (SimpleXmlParserState parser);
void skipInputChars (SimpleXmlParserState parser, int nAmount);
void skipInputChar (SimpleXmlParserState parser);
char readInputChar (SimpleXmlParserState parser);
int addNextTokenCharValue (SimpleXmlParserState parser, char c);
int addNextTokenStringValue (SimpleXmlParserState parser, char *szInput);

SimpleXmlValueBuffer createSimpleXmlValueBuffer (long nInitialSize);
void destroySimpleXmlValueBuffer (SimpleXmlValueBuffer vb);
int growSimpleXmlValueBuffer (SimpleXmlValueBuffer vb);
int appendCharToSimpleXmlValueBuffer (SimpleXmlValueBuffer vb, char c);
int appendStringToSimpleXmlValueBuffer (SimpleXmlValueBuffer vb, const char *szInput);
int zeroTerminateSimpleXmlValueBuffer (SimpleXmlValueBuffer vb);
int clearSimpleXmlValueBuffer (SimpleXmlValueBuffer vb);
int getSimpleXmlValueBufferContentLength (SimpleXmlValueBuffer vb);
int getSimpleXmlValueBufferContents (SimpleXmlValueBuffer vb, char* szOutput, long nMaxLen);
char* getInternalSimpleXmlValueBufferContents (SimpleXmlValueBuffer vb);

/* ---- public api */

SimpleXmlParser simpleXmlCreateParser (const char *sData, long nDataSize) {
	return (SimpleXmlParser) createSimpleXmlParser(sData, nDataSize);
}

void simpleXmlDestroyParser (SimpleXmlParser parser) {
	destroySimpleXmlParser((SimpleXmlParserState) parser);
}

int simpleXmlInitializeParser (SimpleXmlParser parser, const char *sData, long nDataSize) {
	return initializeSimpleXmlParser((SimpleXmlParserState) parser, sData, nDataSize);
}

int simpleXmlParse (SimpleXmlParser parser, SimpleXmlTagHandler handler) {
	if (parseSimpleXml((SimpleXmlParserState) parser, handler) == FAIL) {
		return ((SimpleXmlParserState) parser)->nError;
	}
	return 0;
}

const char* simpleXmlGetErrorDescription (SimpleXmlParser parser) {
	return getSimpleXmlParseErrorDescription((SimpleXmlParserState) parser);
}

long simpleXmlGetLineNumber (SimpleXmlParser parser) {
	if (parser == NULL) {
		return -1;
	}
	return ((SimpleXmlParserState) parser)->nInputLineNumber + 1;
}

void simpleXmlParseAbort (SimpleXmlParser parser, int nErrorCode) {
	if (parser == NULL || nErrorCode < SIMPLE_XML_USER_ERROR) {
		return;
	}
	((SimpleXmlParserState) parser)->nError= nErrorCode;
}

int simpleXmlPushUserData (SimpleXmlParser parser, void* pData) {
	SimpleXmlUserData newUserData;
	if (parser == NULL || pData == NULL) {
		return 0;
	}
	newUserData = (SimpleXmlUserData)malloc(sizeof(TSimpleXmlUserData));
	if (newUserData == NULL) {
		return 0;
	}
	newUserData->pData= pData;
	if (((SimpleXmlParserState) parser)->pUserData == NULL) {
		newUserData->next= NULL;
	} else {
		newUserData->next= ((SimpleXmlParserState) parser)->pUserData;
	}
	((SimpleXmlParserState) parser)->pUserData= newUserData;
	return 1;
}

void* simpleXmlPopUserData (SimpleXmlParser parser) {
	if (parser == NULL) {
		return NULL;
	}
	if (((SimpleXmlParserState) parser)->pUserData == NULL) {
		return NULL;
	} else {
		void* pData;
		SimpleXmlUserData ud= ((SimpleXmlParserState) parser)->pUserData;
		((SimpleXmlParserState) parser)->pUserData= ud->next;
		pData= ud->pData;
		free(ud);
		return pData;
	}
}

void* simpleXmlGetUserDataAt (SimpleXmlParser parser, int nLevel) {
	if (parser == NULL) {
		return NULL;
	} else {
		SimpleXmlUserData ud= ((SimpleXmlParserState) parser)->pUserData;
		while (ud != NULL && nLevel > 0) {
			ud= ud->next;
			nLevel--;
		}
		if (ud != NULL && nLevel == 0) {
			return ud->pData;
		}
	}
	return NULL;
}

void* simpleXmlGetUserData (SimpleXmlParser parser) {
	return simpleXmlGetUserDataAt(parser, 0);
}

/* ---- xml parser */

/**
 * No operation handler (used internally).
 */
void* simpleXmlNopHandler (SimpleXmlParser parser, SimpleXmlEvent event, 
	const char* szName, const char* szAttribute, const char* szValue) {
	/* simply return the nop handler again */
	return simpleXmlNopHandler;
}

/**
 * Creates a new simple xml parser for the specified input data.
 *
 * @param sData the input data to parse (must no be NULL).
 * @param nDataSize the size of the input data buffer (sData) 
 * to parse (must be greater than 0).
 * @return the new simple xml parser or NULL if there
 * is not enough memory or the input data specified cannot
 * be parsed.
 */
SimpleXmlParserState createSimpleXmlParser (const char *sData, long nDataSize) {
	if (sData != NULL && nDataSize > 0) {
		SimpleXmlParserState parser = (SimpleXmlParserState)malloc(sizeof(TSimpleXmlParserState));
		if (parser == NULL) {
			return NULL;
		}
		parser->nError= NOT_PARSED;
		parser->vbNextToken= createSimpleXmlValueBuffer(512);
		if (parser->vbNextToken == NULL) {
			free(parser);
			return NULL;
		}
		parser->szAttribute= NULL;
		parser->nAttributeBufferSize= 0;
		parser->sInputData= (char*) sData;
		parser->nInputDataSize= nDataSize;
		parser->nInputDataPos= 0;
		parser->nInputLineNumber= 0;
		parser->pUserData= NULL;
		return parser;
	}
	return NULL;
}

/**
 * Destroys the specified simple xml parser.
 * 
 * @param parser the parser to destroy (must have been 
 * created using createSimpleXmlParser).
 */
void destroySimpleXmlParser (SimpleXmlParserState parser) {
	if (parser != NULL) {
		if (parser->vbNextToken != NULL) {
			destroySimpleXmlValueBuffer(parser->vbNextToken);
		}
		if (parser->szAttribute != NULL) {
			free(parser->szAttribute);
		}
		{
			/* free user data structures */
			SimpleXmlUserData ud= parser->pUserData;
			while (ud != NULL) {
				SimpleXmlUserData next= ud->next;
				free(ud);
				ud= next;
			}
		}
		free(parser);
	}
}

/**
 * Reinitializes the specified simple xml parser for
 * parsing the specified input data.
 * 
 * @param parser the parser to initialize.
 * @param sData the input data to parse (must no be NULL).
 * @param nDataSize the size of the input data buffer (sData) 
 * to parse (must be greater than 0).
 * @return 0 if the parser could not be initialized,
 * > 0 if the parser was initialized successfully.
 */
int initializeSimpleXmlParser (SimpleXmlParserState parser, const char *sData, long nDataSize) {
	if (parser != NULL && sData != NULL && nDataSize > 0) {
		if (parser->vbNextToken == NULL) {
			return FAIL;
		}
		parser->nError= NOT_PARSED;
		clearSimpleXmlValueBuffer(parser->vbNextToken);
		parser->sInputData= (char*) sData;
		parser->nInputDataSize= nDataSize;
		parser->nInputDataPos= 0;
		parser->nInputLineNumber= 0;
		parser->pUserData= NULL;
		return SUCCESS;
	}
	return FAIL;
}

/**
 * Returns a description of the error that occured
 * during parsing.
 *
 * @param parser the parser for which to get the error
 * description.
 * @return an error description or NULL if there was
 * no error during parsing.
 */
const char* getSimpleXmlParseErrorDescription (SimpleXmlParserState parser) {
	if (parser == NULL) {
		return NULL;
	}
	switch (parser->nError) {
		case NO_ERROR: return NULL;
		case NOT_PARSED: return "parsing has not yet started";
		case OUT_OF_MEMORY: return "out of memory";
		case EARLY_TERMINATION: return "unexpected end of xml data";
		case ILLEGAL_AMPERSAND: return "illegal use of ampersand (&)";
		case NO_UNICODE_SUPPORT: return "unicode characters are not supported";
		case GREATER_THAN_EXPECTED: return "greater than sign (>) expected";
		case QUOTE_EXPECTED: return "quote (either \' or \") expected";
		case ILLEGAL_HANDLER: return "illegal xml handler specified";
		case NOT_INITIALIZED: return "xml parser not initialized";
		case NO_DOCUMENT_TAG: return "no document tag found";
		case MISMATCHED_END_TAG: return "mismatched end tag";
		case ATTRIBUTE_EXPECTED: return "attribute expected";
		case EQUAL_SIGN_EXPECTED: return "equal sign (=) expected";
	}
	if (parser->nError > SIMPLE_XML_USER_ERROR) {
		return "parsing aborted";
	}
	return "unknown error";
}

/**
 * Starts an initialized (or newly created) xml parser with the
 * specified document tag handler.
 *
 * @param parser the parser to start.
 * @param handler the handler to use for the document tag.
 * @return 0 if there was no error, and error code
 * > 0 if there was an error.
 */
int parseSimpleXml (SimpleXmlParserState parser, SimpleXmlTagHandler handler) {
	if (parser == NULL)
		return FAIL;
	if (handler == NULL) {
		parser->nError= ILLEGAL_HANDLER;
		return FAIL;
	}
	
	/* check if the parser was initialized properly */
	if (parser->nError != NOT_PARSED) {
		parser->nError= NOT_INITIALIZED;
		return FAIL;
	}

	/* reset error code */
	parser->nError= NO_ERROR;

	/* read xml prolog and dtd */
	do {
		skipWhitespace(parser);
		clearSimpleXmlValueBuffer(parser->vbNextToken);
		if (readNextContentToken(parser) == FAIL) {
			if (parser->nError == EARLY_TERMINATION) {
				/* read all data and didn't find any tag */
				parser->nError= NO_DOCUMENT_TAG;
			}
			return FAIL;
		}
	} while (
		parser->nNextToken == PROCESSING_INSTRUCTION ||
		parser->nNextToken == COMMENT ||
		parser->nNextToken == DOCTYPE
	);

	/* parse main xml tag */
	if (parser->nNextToken == TAG_BEGIN_OPENING) {
		return parseOneTag(parser, handler);
	}

	/* no document tag found */
	parser->nError= NO_DOCUMENT_TAG;
	return FAIL;
}

/**
 * Parses exactly one tag.
 */
int parseOneTag (SimpleXmlParserState parser, SimpleXmlTagHandler parentHandler) {
	SimpleXmlTagHandler handler;
	char* szTagName;
		
	if (getInternalSimpleXmlValueBufferContents(parser->vbNextToken) == NULL) {
		parser->nError= OUT_OF_MEMORY;
		return FAIL;
	}

	szTagName= mystrdup(getInternalSimpleXmlValueBufferContents(parser->vbNextToken));
	if (szTagName == NULL) {
		parser->nError= OUT_OF_MEMORY;
		return FAIL;
	}
	clearSimpleXmlValueBuffer(parser->vbNextToken);
	
	handler = (SimpleXmlTagHandler)parentHandler((SimpleXmlParser) parser, ADD_SUBTAG, szTagName, NULL, NULL);
	if (parser->nError != NO_ERROR) {
		return FAIL;
	}
	if (handler == NULL) {
		handler= simpleXmlNopHandler;
	}
	
	if (readNextTagToken(parser) == FAIL) {
		free(szTagName);
		return FAIL;
	}
	while (
		parser->nNextToken != TAG_END &&
		parser->nNextToken != TAG_BEGIN_CLOSING
	) {
		/* read attributes */
		if (parser->nNextToken == ATTRIBUTE) {
			if (getInternalSimpleXmlValueBufferContents(parser->vbNextToken) == NULL) {
				parser->nError= OUT_OF_MEMORY;
				free(szTagName);
				return FAIL;
			}
			handler((SimpleXmlParser) parser, ADD_ATTRIBUTE, szTagName, parser->szAttribute, 
				getInternalSimpleXmlValueBufferContents(parser->vbNextToken));
			if (parser->nError != NO_ERROR) {
				free(szTagName);
				return FAIL;
			}
			clearSimpleXmlValueBuffer(parser->vbNextToken);
		} else {
			/* attribute expected */
			parser->nError= ATTRIBUTE_EXPECTED;
			free(szTagName);
			return FAIL;
		}
		if (readNextTagToken(parser) == FAIL) {
			free(szTagName);
			return FAIL;
		}
	}

	handler((SimpleXmlParser) parser, FINISH_ATTRIBUTES, szTagName, NULL, NULL);
	if (parser->nError != NO_ERROR) {
		free(szTagName);
		return FAIL;
	}

	if (parser->nNextToken == TAG_BEGIN_CLOSING) {
		if (readNextContentToken(parser) == FAIL) {
			free(szTagName);
			return FAIL;
		}
		while (parser->nNextToken != TAG_END) {
			/* read content */
			if (parser->nNextToken == TAG_BEGIN_OPENING) {
				/* read subtag -> recurse */
				if (parseOneTag(parser, handler) == FAIL) {
					free(szTagName);
					return FAIL;
				}
			} else if (parser->nNextToken == CONTENT) {
				/* read content value */
				if (getInternalSimpleXmlValueBufferContents(parser->vbNextToken) == NULL) {
					parser->nError= OUT_OF_MEMORY;
					free(szTagName);
					return FAIL;
				}
				handler((SimpleXmlParser) parser, ADD_CONTENT, szTagName, NULL, 
					getInternalSimpleXmlValueBufferContents(parser->vbNextToken));
				if (parser->nError != NO_ERROR) {
					free(szTagName);
					return FAIL;
				}
				clearSimpleXmlValueBuffer(parser->vbNextToken);
			} else if (parser->nNextToken == COMMENT) {
				/* ignore comment for the moment (maybe we should call the handler) */
			}
			/* discard current token value */
			clearSimpleXmlValueBuffer(parser->vbNextToken);
			/* get the next token */
			if (readNextContentToken(parser) == FAIL) {
				free(szTagName);
				return FAIL;
			}
		}
		/* check the name of the closing tag */
		if (getInternalSimpleXmlValueBufferContents(parser->vbNextToken) == NULL) {
			parser->nError= OUT_OF_MEMORY;
			free(szTagName);
			return FAIL;
		}
		if (
			strcmp(
				szTagName, getInternalSimpleXmlValueBufferContents(parser->vbNextToken)
			) != 0
		) {
			parser->nError= MISMATCHED_END_TAG;
			free(szTagName);
			return FAIL;
		}
	}

	/* flush closing tag token data */
	clearSimpleXmlValueBuffer(parser->vbNextToken);

	handler((SimpleXmlParser) parser, FINISH_TAG, szTagName, NULL, NULL);
	if (parser->nError != NO_ERROR) {
		free(szTagName);
		return FAIL;
	}

	free(szTagName);
	return SUCCESS;
}

/**
 * Scanner that reads the contents of a tag and
 * sets the nNextToken type and the value buffer of
 * the parser. Must not be invoked unless the last
 * token read was a TAG_BEGIN (see readNextContentToken
 * in such a case).
 *
 * The following token types are supported:
 * 
 * Type                   | ValueBuffer | Example
 * -----------------------+-------------+-------------
 * TAG_END                | <unchanged> | />
 * TAG_BEGIN_CLOSING      | <unchanged> | >
 * ATTRIBUTE              | bar         | foo="bar"
 * 
 * Note: The name of an attribute (e.g. foo in the above
 * example) is stored in the attribute name field of the
 * parser (szAttributeName).
 * 
 * @param parser the parser for which to read the next token.
 * @return SUCCESS or FAIL.
 */
int readNextTagToken (SimpleXmlParserState parser) {
	/* read tag closing or attribute */
	if (peekInputChar(parser) == '/') {
		/* read tag closing (combined end tag) */
		skipInputChar(parser);
		if (peekInputChar(parser) == '>') {
			parser->nNextToken= TAG_END;
			skipInputChar(parser);
		} else {
			parser->nError= GREATER_THAN_EXPECTED;
			return FAIL;
		}
	} else if (peekInputChar(parser) == '>') {
		/* read tag closing */
		parser->nNextToken= TAG_BEGIN_CLOSING;
		skipInputChar(parser);
	} else {
		/* read attribute */
		char cQuote; /* the quote type used (either ' or " -> a='b' or a="b") */
		parser->nNextToken= ATTRIBUTE;
		while (peekInputChar(parser) != '=' && peekInputChar(parser) > SPACE) {
			/* read one character into next token value buffer */
			if (readChar(parser) == FAIL) {
				return FAIL;
			}
		}
		/* skip whitespace */
		if (skipWhitespace(parser) == FAIL) {
			return FAIL;
		}
		if (peekInputChar(parser) != '=') {
			parser->nError= EQUAL_SIGN_EXPECTED;
			return FAIL;
		}
		/* skip '=' */
		skipInputChar(parser);
		/* copy contents of value buffer to attribute name */
		if (
			parser->szAttribute == NULL ||
			parser->nAttributeBufferSize < getSimpleXmlValueBufferContentLength(parser->vbNextToken)
		) {
			if (parser->szAttribute != NULL) {
				free(parser->szAttribute);
			}
			parser->nAttributeBufferSize= getSimpleXmlValueBufferContentLength(parser->vbNextToken);
			parser->szAttribute = (char*)malloc(parser->nAttributeBufferSize);
		}
		if (parser->szAttribute == NULL) {
			parser->nError= OUT_OF_MEMORY;
			return FAIL;
		}
		if (
			getSimpleXmlValueBufferContents(
				parser->vbNextToken, parser->szAttribute, parser->nAttributeBufferSize
			) == FAIL
		) {
			parser->nError= OUT_OF_MEMORY;
			return FAIL;
		}
		clearSimpleXmlValueBuffer(parser->vbNextToken);
		/* skip whitespace */
		if (skipWhitespace(parser) == FAIL) {
			return FAIL;
		}
		cQuote= readInputChar(parser);
		if (parser->nError != NO_ERROR) {
			return FAIL;
		}
		if (cQuote != '\'' && cQuote != '"') {
			parser->nError= QUOTE_EXPECTED;
			return FAIL;
		}
		while (peekInputChar(parser) != cQuote) {
			/* read one character into next token value buffer */
			if (readChar(parser) == FAIL) {
				return FAIL;
			}
		}
		/* skip quote character */
		skipInputChar(parser);
		/* skip whitespace */
		if (skipWhitespace(parser) == FAIL) {
			return FAIL;
		}
	}
	return SUCCESS;
}

/** 
 * Scanner that reads the next token type and sets
 * the nNextToken type and the value buffer of the
 * parser. Must not be invoked when the last token
 * read was a TAG_BEGIN (use readNextTagToken in
 * such a case).
 * 
 * The following token types are supported:
 * 
 * Type                   | ValueBuffer | Example
 * -----------------------+-------------+-------------
 * TAG_BEGIN_OPENING      | foo         | <foo
 * TAG_END                | foo         | </foo>
 * CONTENT                | foo         | foo
 * PROCESSING_INSTRUCTION | XML         | <?XML?>
 * UNKNOWN                | WHATEVER    | <!WHATEVER>
 * COMMENT                | foo         | <!--foo-->
 * DOCTYPE                | foo         | <!DOCTYPEfoo>
 * 
 * @param parser the parser for which to read the next token.
 * @return SUCCESS or FAIL.
 */
int readNextContentToken (SimpleXmlParserState parser) {
	/* read markup or content */
	if (peekInputChar(parser) == '<') {
		/* read markup */
		skipInputChar(parser);
		if (peekInputChar(parser) == '/') {
			/* read tag closing */
			parser->nNextToken= TAG_END;
			skipInputChar(parser);
			while (
				peekInputChar(parser) > SPACE &&
				peekInputChar(parser) != '>'
			) {
				/* read one character into next token value buffer */
				if (readChar(parser) == FAIL) {
					return FAIL;
				}
			}
			while (peekInputChar(parser) != '>') {
				skipInputChar(parser);
			}
			if (peekInputChar(parser) != '>') {
				parser->nError= EARLY_TERMINATION;
				return FAIL;
			}
			/* skip closing '>' */
			skipInputChar(parser);
		} else if (peekInputChar(parser) == '?') {
			/* read processing instruction (e.g. <?XML ... ?> prolog) */
			parser->nNextToken= PROCESSING_INSTRUCTION;
			skipInputChar(parser);
			while (
				peekInputCharAt(parser, 0) != '?' ||
				peekInputCharAt(parser, 1) != '>'
			) {
				/* read one character into next token value buffer */
				if (readChar(parser) == FAIL) {
					return FAIL;
				}
			}
			/* skip closing '?>' */
			skipInputChars(parser, 2);
		} else if (peekInputChar(parser) == '!') {
			/* read comment, doctype or cdata */
			skipInputChar(parser);
			if (
				peekInputCharAt(parser, 0) == '-' &&
				peekInputCharAt(parser, 1) == '-'
			) {
				/* read comment */
				parser->nNextToken= COMMENT;
				skipInputChars(parser, 2);
				while (
					peekInputCharAt(parser, 0) != '-' ||
					peekInputCharAt(parser, 1) != '-' ||
					peekInputCharAt(parser, 2) != '>'
				) {
					/* read one character into next token value buffer */
					if (readChar(parser) == FAIL) {
						return FAIL;
					}
				}
				/* skip closing '-->' */
				skipInputChars(parser, 3);
			} else if (
				peekInputCharAt(parser, 0) == 'D' &&
				peekInputCharAt(parser, 1) == 'O' &&
				peekInputCharAt(parser, 2) == 'C' &&
				peekInputCharAt(parser, 3) == 'T' &&
				peekInputCharAt(parser, 4) == 'Y' &&
				peekInputCharAt(parser, 5) == 'P' &&
				peekInputCharAt(parser, 6) == 'E'
			) {
				/* read doctype declaration (external only) */
				int nCount= 1;
				parser->nNextToken= DOCTYPE;
				skipInputChars(parser, 7);
				while (nCount > 0) {
					if (peekInputChar(parser) == '>') {
						nCount--;
					} else if (peekInputChar(parser) == '<') {
						nCount++;
					}
					/* read one character into next token value buffer */
					if (nCount > 0 && readChar(parser) == FAIL) {
						return FAIL;
					}
				}
				/* skip closing '>' */
				skipInputChar(parser);
			} else {
				/* read cdata, not supported yet \,
				   simply skip to the next closing '>' */
				parser->nNextToken= UNKNOWN;
				while (peekInputChar(parser) != '>') {
					/* read one character into next token value buffer */
					if (readChar(parser) == FAIL) {
						return FAIL;
					}
				}
				/* skip closing '>' */
				skipInputChar(parser);
			}
		} else {
			/* read tag opening (without '>' or '/>') */
			parser->nNextToken= TAG_BEGIN_OPENING;
			while (
				peekInputChar(parser) > SPACE &&
				peekInputChar(parser) != '/' &&
				peekInputChar(parser) != '>'
			) {
				/* read one character into next token value buffer */
				if (readChar(parser) == FAIL) {
					return FAIL;
				}
			}
			/* skip whitespace */
			if (skipWhitespace(parser) == FAIL) {
				return FAIL;
			}
		}
	} else {
		/* read content */
		parser->nNextToken= CONTENT;
		while (peekInputChar(parser) != '<') {
			/* read one character into next token value buffer */
			if (readChar(parser) == FAIL) {
				return FAIL;
			}
		}
	}
	return SUCCESS;
}

/**
 * Reads the next character from the input data and
 * appends it to the next token value buffer of the parser.
 * 
 * Note: This method does not support unicode and 8-bit
 * characters are read using the default platform encoding.
 * 
 * @param parser the parser for which to read the next input character.
 * @return SUCCESS or FAIL.
 */
int readChar (SimpleXmlParserState parser) {
	char c= readInputChar(parser);
	if (c == '\0' && parser->nError != NO_ERROR) {
		return FAIL;
	} else if (c == '&') {
		/* read entity */
		if (peekInputCharAt(parser, 0) == '#') {
			int nCode= 0;
			skipInputChar(parser);
			c= readInputChar(parser);
			if (c == 'x') {
				/* &#x<hex>; */
				c= readInputChar(parser);
				while (c != ';') {
					if (c >= '0' && c <= '9') {
						nCode= (nCode * 16) + (c - '0');
					} else if (c >= 'A' && c <= 'F') {
						nCode= (nCode * 16) + (c - 'A' + 10);
					} else if (c >= 'a' && c <= 'f') {
						nCode= (nCode * 16) + (c - 'a' + 10);
					} else {
						parser->nError= ILLEGAL_AMPERSAND;
						return FAIL;
					}
					c= readInputChar(parser);
				}
			} else if (c >= '0' && c <= '9') {
				/* &#<dec>; */
				c= readInputChar(parser);
				while (c != ';') {
					if (c >= '0' && c <= '9') {
						nCode= (nCode * 16) + (c - '0');
					} else {
						parser->nError= ILLEGAL_AMPERSAND;
						return FAIL;
					}
					c= readInputChar(parser);
				}
			} else {
				/* illegal use of ampersand */
				parser->nError= ILLEGAL_AMPERSAND;
				return FAIL;
			}
			if (nCode > 255) {
				parser->nError= NO_UNICODE_SUPPORT;
				return FAIL;
			}
			return addNextTokenCharValue(parser, (char) nCode);
		} else if (
			peekInputCharAt(parser, 0) == 'a' &&
			peekInputCharAt(parser, 1) == 'm' &&
			peekInputCharAt(parser, 2) == 'p' &&
			peekInputCharAt(parser, 3) == ';'
		) {
			/* &amp; -> & */
			skipInputChars(parser, 4);
			return addNextTokenCharValue(parser, '&');
		} else if (
			peekInputCharAt(parser, 0) == 'a' &&
			peekInputCharAt(parser, 1) == 'p' &&
			peekInputCharAt(parser, 2) == 'o' &&
			peekInputCharAt(parser, 3) == 's' &&
			peekInputCharAt(parser, 4) == ';'
		) {
			/* &apos; -> ' */
			skipInputChars(parser, 5);
			return addNextTokenCharValue(parser, '\'');
		} else if (
			peekInputCharAt(parser, 0) == 'q' &&
			peekInputCharAt(parser, 1) == 'u' &&
			peekInputCharAt(parser, 2) == 'o' &&
			peekInputCharAt(parser, 3) == 't' &&
			peekInputCharAt(parser, 4) == ';'
		) {
			/* &quot; -> " */
			skipInputChars(parser, 5);
			return addNextTokenCharValue(parser, '"');
		} else if (
			peekInputCharAt(parser, 0) == 'l' &&
			peekInputCharAt(parser, 1) == 't' &&
			peekInputCharAt(parser, 2) == ';'
		) {
			/* &lt; -> < */
			skipInputChars(parser, 3);
			return addNextTokenCharValue(parser, '<');
		} else if (
			peekInputCharAt(parser, 0) == 'g' &&
			peekInputCharAt(parser, 1) == 't' &&
			peekInputCharAt(parser, 2) == ';'
		) {
			/* &gt; -> > */
			skipInputChars(parser, 3);
			return addNextTokenCharValue(parser, '>');
		} else {
			/* illegal use of ampersand */
			parser->nError= ILLEGAL_AMPERSAND;
			return FAIL;
		}
	} else {
		/* read simple character */
		return addNextTokenCharValue(parser, c);
	}
}

/**
 * Peeks at the character with the specified offset from
 * the cursor (i.e. the last character read).
 *
 * Note: To peek at the next character that will be read
 * use and offset of 0.
 * 
 * @param parser the parser for which to peek.
 * @param nOffset the peek offset relative to the
 * position of the last char read.
 * @return the peeked character or '\0' if there are
 * no more data.
 */
char peekInputCharAt (SimpleXmlParserState parser, int nOffset) {
	int nPos= parser->nInputDataPos + nOffset;
	if (nPos < 0 || nPos >= parser->nInputDataSize) {
		return '\0';
	}
	return parser->sInputData[nPos];
}

/**
 * Peeks at the current input character at the read cursor
 * position of the specified parser.
 *
 * @param parser the parser for which to peek.
 * @return the peeked character or '\0' if there are
 * no more data.
 */
char peekInputChar (SimpleXmlParserState parser) {
	return peekInputCharAt(parser, 0);
}

/**
 * Skips any whitespace at the cursor position of the
 * parser specified.
 * 
 * Note: All characters smaller than the space character
 * are considered to be whitespace.
 *
 * @param parser the parser for which to skip whitespace.
 * @return SUCCESS or FAIL.
 */
int skipWhitespace (SimpleXmlParserState parser) {
	while (peekInputChar(parser) <= SPACE) {
		/* skip whitespace but make sure we don't
			 read more than is available */
		readInputChar(parser);
		if (parser->nError != NO_ERROR) {
			return FAIL;
		}
	}
	return SUCCESS;
}

/**
 * Moves the read cursor of the specified parser by the
 * specified amount.
 *
 * @param parser the parser whose read cursor is to be moved.
 * @param nAmount the amount by which to move the cursor (> 0).
 */
void skipInputChars (SimpleXmlParserState parser, int nAmount) {
	int i;
	for (i= 0; i < nAmount; i++) {
		skipInputChar(parser);
	}
}

/**
 * Skips the current input read character.
 *
 * @param parser the parser whose read cursor should be incremented.
 * @see #skipInputChars
 * @see #peekInputChar
 */
void skipInputChar (SimpleXmlParserState parser) {
	if (parser->nInputDataPos >= 0 && parser->nInputDataPos < parser->nInputDataSize) {
		if (parser->sInputData[parser->nInputDataPos] == LF) {
			/* always increment line counter on a linefeed */
			parser->nInputLineNumber++;
		} else if (parser->sInputData[parser->nInputDataPos] == CR) {
			/* only increment on an carriage return if no linefeed follows
			   (this is for the mac filetypes) */
			if (parser->nInputDataPos + 1 < parser->nInputDataSize) {
				if (parser->sInputData[parser->nInputDataPos + 1] != LF) {
					parser->nInputLineNumber++;
				}
			}
		}
	}
	parser->nInputDataPos++;
}

/**
 * Reads the next input character from the specified parser
 * and returns it.
 * 
 * Note: If an error is encountered '\0' is returned and the
 * nError flag of the parser is set to EARLY_TERMINATION.
 * 
 * @param parser the parser from which to read the next 
 * input character.
 * @return the next input character or '\0' if there is none.
 */
char readInputChar (SimpleXmlParserState parser) {
	char cRead;
	if (parser->nInputDataPos < 0 || parser->nInputDataPos >= parser->nInputDataSize) {
		parser->nError= EARLY_TERMINATION;
		return '\0';
	}
	cRead= parser->sInputData[parser->nInputDataPos];
	skipInputChar(parser);
	return cRead;
}

/**
 * Appends a character to the next token value string.
 *
 * @param parser the parser whose next token value string
 * is to be modified.
 * @param c the character to append.
 * @return SUCCESS or FAIL (if there is not enough memory).
 */
int addNextTokenCharValue (SimpleXmlParserState parser, char c) {
	if (appendCharToSimpleXmlValueBuffer(parser->vbNextToken, c) == FAIL) {
		parser->nError= OUT_OF_MEMORY;
		return FAIL;
	}
	return SUCCESS;
}

/**
 * Appends a zero terminated string to the next token value string.
 *
 * @param parser the parser whose next token value string
 * is to be modified.
 * @param szInput the zero terminated string to append.
 * @return SUCCESS or FAIL (if there is not enough memory).
 */
int addNextTokenStringValue (SimpleXmlParserState parser, char *szInput) {
	while (*szInput != '\0') {
		if (addNextTokenCharValue(parser, *szInput) == FAIL) {
			return FAIL;
		}
		szInput++;
	}
	return SUCCESS;

}

/* ---- SimpleXmlValueBuffer */

/**
 * Creates a new value buffer of the specified size.
 *
 * The value buffer automatically grows when appending characters
 * if it is not large enough.
 *
 * The value buffer uses 'malloc' to allocate buffer space.
 * The user is responsible for freeing the value buffer created
 * using destroySimpleXmlValueBuffer.
 * 
 * @param nInitialSize the initial size of the value buffer in chars.
 * @return NULL if the value buffer could not be allocated, 
 * the newly allocated value buffer otherwise (to be freed by the 
 * caller).
 * @see #destroySimpleXmlValueBuffer
 */
SimpleXmlValueBuffer createSimpleXmlValueBuffer (long nInitialSize) {
	SimpleXmlValueBuffer vb = (SimpleXmlValueBuffer)malloc(sizeof(TSimpleXmlValueBuffer));
	if (vb == NULL) {
		return NULL;
	}
	vb->sBuffer = (char*)malloc(nInitialSize);
	if (vb->sBuffer == NULL) {
		free(vb);
		return NULL;
	}
	vb->nSize= nInitialSize;
	vb->nPosition= 0;
	return vb;
}

/**
 * Destroys a value buffer created using createSimpleXmlValueBuffer.
 *
 * @param vb the value buffer to destroy.
 * @see #destroySimpleXmlValueBuffer
 */
void destroySimpleXmlValueBuffer (SimpleXmlValueBuffer vb) {
	if (vb != NULL) {
		if (vb->sBuffer != NULL) {
			free(vb->sBuffer);
		}
		free(vb);
	}
}

/**
 * Grows the internal data buffer of the value buffer.
 *
 * @param vb the value buffer to grow.
 * @return SUCCESS or FAIL (if there is not enough memory).
 */
int growSimpleXmlValueBuffer (SimpleXmlValueBuffer vb) {
	char* sOldBuffer = vb->sBuffer;
	char* sNewBuffer = (char*)malloc(vb->nSize * 2);
	if (sNewBuffer == NULL) {
		return FAIL;
	}
	memcpy(sNewBuffer, vb->sBuffer, vb->nSize);
	vb->sBuffer= sNewBuffer;
	vb->nSize= vb->nSize * 2;
	free(sOldBuffer);
	return SUCCESS;
}

/**
 * Appends a character to the value buffer.
 *
 * @param vb the value buffer to append to.
 * @param c the character to append.
 * @return SUCCESS or FAIL (if there is not enough memory).
 */
int appendCharToSimpleXmlValueBuffer (SimpleXmlValueBuffer vb, char c) {
	if (vb == NULL) {
		return FAIL;
	}
	if (vb->nPosition >= vb->nSize) {
		if (growSimpleXmlValueBuffer(vb) == FAIL) {
			return FAIL;
		}
	}
	vb->sBuffer[vb->nPosition++]= c;
	return SUCCESS;
}

/**
 * Appends a zero terminated string to the value buffer.
 *
 * @param vb the value buffer to append to.
 * @param szInput the input string to append.
 * @return SUCCESS or FAIL (if there is not enough memory).
 */
int appendStringToSimpleXmlValueBuffer (SimpleXmlValueBuffer vb, const char *szInput) {
	while (*szInput != '\0') {
		if (appendCharToSimpleXmlValueBuffer(vb, *szInput) == FAIL) {
			return FAIL;
		}
		szInput++;
	}
	return SUCCESS;
}

/**
 * Zero terminates the internal buffer without appending
 * any characters (i.e. the append location is not modified).
 *
 * @param vb the value buffer to zero terminate.
 * @return SUCCESS or FAIL (if there is not enough memory).
 */
int zeroTerminateSimpleXmlValueBuffer (SimpleXmlValueBuffer vb) {
	if (vb == NULL) {
		return FAIL;
	}
	if (vb->nPosition >= vb->nSize) {
		if (growSimpleXmlValueBuffer(vb) == FAIL) {
			return FAIL;
		}
	}
	vb->sBuffer[vb->nPosition]= '\0';
	return SUCCESS;
}

/**
 * Resets the append location of the value buffer.
 *
 * @param vb the value buffer to clear.
 * @return SUCCESS or FAIL.
 */
int clearSimpleXmlValueBuffer (SimpleXmlValueBuffer vb) {
	if (vb == NULL) {
		return FAIL;
	}
	vb->nPosition= 0;
	return SUCCESS;
}

/**
 * Returns the content length of the value buffer 
 * (including a trailing zero termination character).
 *
 * @param vb the value buffer whose content length should
 * be determined.
 * @return the content length (i.e. 1 if the content is
 * empty, 0 in case of a failure).
 */
int getSimpleXmlValueBufferContentLength (SimpleXmlValueBuffer vb) {
	if (vb == NULL) {
		return 0;
	}
	return vb->nPosition + 1;
}

/**
 * Retrieves the buffer content and stores it to the
 * specified output array.
 *
 * @param vb the value buffer whose content should be retrieved.
 * @param szOutput the output character array to store it to,
 * the output array is in any case zero terminated!
 * @param nMaxLen the maximum number of characters to write to
 * the output array.
 * @return SUCCESS or FAIL.
 */
int getSimpleXmlValueBufferContents (SimpleXmlValueBuffer vb, char* szOutput, long nMaxLen) {
	int nMax; /* max. number of chars to copy */
	if (vb == NULL) {
		return FAIL;
	}
	nMaxLen-= 1; /* reserve space for terminating zero */
	nMax= nMaxLen < vb->nPosition ? nMaxLen : vb->nPosition;
	memcpy(szOutput, vb->sBuffer, nMax);
	szOutput[nMax]= '\0';
	return SUCCESS;
}

/**
 * Returns the zero terminated internal string buffer of
 * the value buffer specified.
 *
 * Warning: Modifying the array returned modifies the 
 * internal data of the value buffer!
 *
 * @param vb the value buffer whose string buffer should
 * be returned.
 * @return the string buffer or NULL (if there is not enough memory).
 */
char* getInternalSimpleXmlValueBufferContents (SimpleXmlValueBuffer vb) {
	if (zeroTerminateSimpleXmlValueBuffer(vb) == FAIL) {
		return NULL;
	}
	return vb->sBuffer;
}

