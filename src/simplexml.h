/* $Id: simplexml.h,v 1.1.1.1 2002/08/23 10:38:58 essmann Exp $
 *
 * Copyright (c) 2001-2002 Bruno Essmann <essmann@users.sourceforge.net>
 * All rights reserved.
 */

#ifndef __SIMPLE_XML
#define __SIMPLE_XML

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The simple xml parser structure.
 * 
 * SimpleXmlParsers should be created and destroyed using the functions
 * simpleXmlCreateParser, simpleXmlDestoryParser.
 */
typedef void *SimpleXmlParser;

/**
 * Enumeration describing the event types that are sent to an SimpleXmlHandler 
 * by an SimpleXmlParser.
 *
 * @see #SimpleXmlTagHandler
 * @see #SimpleXmlParser
 */
typedef enum simple_xml_event {
	FINISH_TAG, ADD_ATTRIBUTE, FINISH_ATTRIBUTES, ADD_CONTENT, ADD_SUBTAG
} SimpleXmlEvent;

/**
 * Callback function to handle simple xml events.
 *
 * The SimpleXmlTagHandler is invoked by a SimpleXmlParser
 * whenever one of the following event types occur:
 *
 * FINISH_TAG 
 *   indicates that parsing of this tag has finished, szName contains the tag
 *   name, szAttribute and szValue are NULL, the result of the handler is 
 *   ignored.
 * ADD_ATTRIBUTE
 *   indicates that an attribute for this tag has been parsed, szName contains
 *   the tag name, szAttribute the attribute name and szValue contains the 
 *   attribute contents, the result of the handler is ignored.
 * FINISH_ATTRIBUTES,
 *   indicates that parsing of attributes for this tag is finished, szName 
 *   contains the tag name, szAttribute and szValue are NULL, the result of 
 *   the handler is ignored.
 * ADD_CONTENT
 *   indicates that content of this tag has been parsed and should be added, 
 *   szName contains the tag name and szValue contains the data to add, 
 *   szAttribute is NULL and the result of the handler is ignored.
 * ADD_SUBTAG
 *   indicates that a subtag has been parsed, szName contains the name of the 
 *   subtag read, szAttribute and szValue are NULL, the result of the handler 
 *   should either be NULL to indicate that this subtag is not of interest 
 *   and should be skipped a SimpleXmlTagHandler that is used for handling 
 *   the subtag.
 *
 * @see #SimpleXmlEvent
 * @see #SimpleXmlParser
 */
typedef void* (*SimpleXmlTagHandler) (
	SimpleXmlParser parser,
	SimpleXmlEvent event,
	const char* szName,
	const char* szAttribute,
	const char* szValue
);

/**
 * Creates a new simple xml parser for the specified input data.
 *
 * The input data may be parsed with simpleXmlParse and the parser returned
 * by this function as parameter.
 * 
 * Note: The parser will not copy the input data or in any way modify it.
 * However any modifications of the input data in a callback handler while
 * parsing will have an undefined result!
 * 
 * @param sData the input data to parse (must no be NULL).
 * @param nDataSize the size of the input data buffer (sData) to parse (must 
 * be greater than 0).
 * @return the new simple xml parser or NULL if there is not enough memory or 
 * the input data specified cannot be parsed.
 */
extern SimpleXmlParser simpleXmlCreateParser (
	const char *sData, long nDataSize
);

/**
 * Destroys the specified simple xml parser.
 * 
 * @param parser the parser to destroy (must have been created using 
 * simpleXmlCreateParser).
 */
extern void simpleXmlDestroyParser (
	SimpleXmlParser parser
);

/**
 * Reinitializes the specified simple xml parser for parsing the specified 
 * input data.
 * 
 * @param parser the parser to initialize.
 * @param sData the input data to parse (must no be NULL).
 * @param nDataSize the size of the input data buffer (sData) to parse (must 
 * be greater than 0).
 * @return 0 if the parser could not be initialized, > 0 if the parser was 
 * initialized successfully and parsing may be started using simpleXmlParse.
 */
extern int simpleXmlInitializeParser (
	SimpleXmlParser parser, const char *sData, long nDataSize
);

/**
 * Starts an initialized (or newly created) xml parser with the specified 
 * document tag handler.
 *
 * Note: This function may only be called once after creation or 
 * initialization of a parser. To reuse the parser it has to be freshly
 * initialized (using simpleXmlInitializeParser) prior to calling the 
 * function again.
 * 
 * @param parser the parser to start.
 * @param handler the handler to use for the document tag.
 * @return 0 if there was no error, and error code > 0 if there was an error.
 */
int simpleXmlParse (SimpleXmlParser parser, SimpleXmlTagHandler handler);

/**
 * Returns a description of the error that occured during parsing.
 *
 * @param parser the parser for which to get the error description.
 * @return an error description or NULL if there was no error during parsing.
 */
const char* simpleXmlGetErrorDescription (SimpleXmlParser parser);

/**
 * Returns the line number of the current input line that the parser has read.
 *
 * In case of an error this method will return the line number on which the
 * error was encountered after a call to simpleXmlParse.
 *
 * If called from a handler during parsing this function will return the
 * current line number.
 * 
 * If called after a successfull simpleXmlParse run this function will return
 * the line number of the last line parsed in the xml data.
 *
 * @return the current input line number of the parser or -1 if it is unknown.
 */
long simpleXmlGetLineNumber (SimpleXmlParser parser);

/**
 * Minimum value for a user abort.
 * @see #simpleXmlParseAbort
 */
#define SIMPLE_XML_USER_ERROR 1000

/**
 * Causes the simple xml parser to abort parsing of the input data.
 *
 * This method may only be called from a tag handler.
 * 
 * The active simpleXmlParse run will be aborted and the simpleXmlParse
 * function will return with the specified error code.
 *
 * @param nErrorCode the error code with which to abort (the error code must 
 * be >= SIMPLE_XML_USER_ERROR else the abort request is ignored!)
 */
void simpleXmlParseAbort (SimpleXmlParser parser, int nErrorCode);

/**
 * Pushes the specified pointer on the user data stack of the parser.
 *
 * @param parser the parser on which to push the user data pointer.
 * @param pData pointer to the user data to set (NULL is not a valid value).
 * @return > 0 on success, 0 if there is not enough memory left or it was
 * tried to push a NULL data pointer.
 */
int simpleXmlPushUserData (SimpleXmlParser parser, void* pData);

/**
 * Pops the last pointer pushed on the user data stack of the parser.
 *
 * @param parser the parser on which to pop the user data pointer.
 * @return the last pointer pushed or NULL if the stack is empty.
 */
void* simpleXmlPopUserData (SimpleXmlParser parser);

/**
 * Peeks at the top of the user data stack.
 *
 * The last pointer pushed on the user data stack is returned.
 *
 * Note: This function does not modify the stack.
 *
 * This function is a convenience function for
 * simpleXmlGetUserDataAt(parser, 0);
 *
 * @param parser the parser from which to get the user data pointer.
 * @return the value of the user data pointer or NULL if the pointer has not 
 * been set yet.
 */
void* simpleXmlGetUserData (SimpleXmlParser parser);

/**
 * Peeks at a specified location from to the top of the user data stack.
 *
 * The level indicates the level from the top of the stack that is
 * inspected.
 *
 * Note: This method does not modify the stack.
 *
 * @param parser the parser from which to get the user data pointer.
 * @param nLevel the level (from the top of the stack) to inspect.
 * @return the value of the user data pointer at the specified level or
 * NULL if there is no such entry (i.e. the level is higher than the
 * stack depth).
 */
void* simpleXmlGetUserDataAt (SimpleXmlParser parser, int nLevel);

#ifdef __cplusplus
}
#endif

#endif

