#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <stack>
#include <vector>


#include <Poco/String.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/SAX/SAXParser.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/XMLFilterImpl.h>
#include <Poco/SAX/InputSource.h>

using Poco::XML::XMLChar;
using Poco::XML::XMLString;
using Poco::XML::Attributes;
using Poco::XML::InputSource;

const XMLString lSampleXml1("<Subtraction><int>1</int><int>2</int> </Subtraction>");
const XMLString lSampleXml2("<Subtraction><int>2</int><int>1</int></Subtraction>");
const XMLString lSampleXml3("<Division><int>1</int><int>2</int></Division>");
const XMLString lSampleXml4("<Division><int>2</int><int>1</int></Division>");
const XMLString lSampleXml5("<Addition><int>3</int><int>5</int></Addition>");

const XMLString lSampleXml("\
     <Division> \
          <Addition> \
               <int>5</int> \
               <Subtraction> \
                    <int>2</int> \
                    <int>1</int> \
               </Subtraction> \
          </Addition> \
          <Division> \
               <int>10</int> \
               <int>2</int> \
          </Division> \
     </Division> \
");

class Op;
typedef std::shared_ptr< Op > OpPtr;

class Op
{
public:
	virtual void addOperand( OpPtr aOp )
	{
		iOps.push_back( aOp );
	}

	virtual double execute() = 0;

protected:
	std::vector< OpPtr > iOps;
};

class OpCondition: public Op
{
public:
	virtual double execute()
	{
		return iOps[0]->execute() > iOps[1]->execute();
	}
};

class OpAdd: public Op
{
public:
	virtual double execute()
	{
		return iOps[0]->execute() + iOps[1]->execute();
	}
};

class OpSub: public Op
{
public:
	virtual double execute()
	{
		return iOps[0]->execute() - iOps[1]->execute();
	}
};

class OpDiv: public Op
{
public:
	virtual double execute()
	{
		return iOps[0]->execute() / iOps[1]->execute();
	}
};

class OpInt: public Op
{
public:
	virtual double execute()
	{
		return iOps[0]->execute();
	}

private:
	double iValue;
};

class OpChars: public Op
{
public:
	explicit OpChars( const std::string & aValue )
	{
		std::stringstream lStr( aValue );
		lStr >> iValue;
	}

	virtual double execute()
	{
		return iValue;
	}

private:
	double iValue;
};


class OpFactory
{
public:
	static OpPtr createOp( const XMLString & localName,
                               const Attributes & attrList )
	{
		if ( localName == "Addition" )
		{
			return OpPtr( new OpAdd );
		}
		else if ( localName == "Subtraction" )
		{
			return OpPtr( new OpSub );
		}
		else if ( localName == "Division" )
		{
			return OpPtr( new OpDiv );
		}
		else if ( localName == "int" )
		{
			return OpPtr( new OpInt );
		}

		std::cout << "W: invalid op" << std::endl;
		return OpPtr();
	}
};

class MyHandler: public Poco::XML::XMLFilterImpl
{
public:
	void startElement(
	    const XMLString & uri,
	    const XMLString & localName,
	    const XMLString & qname,
	    const Attributes & attrList
	)
	{
		std::cout << "enters: " << localName << std::endl;
		iExecStack.push( OpFactory::createOp( localName, attrList ) );
	}

	void endElement(
		const XMLString & uri,
		const XMLString & localName,
		const XMLString & qname
	)
	{
		std::cout << "leaves: " << localName << std::endl;
		std::cout << "iExecStack.size(): " << iExecStack.size() << std::endl;
		OpPtr lOp = iExecStack.top();
		iExecStack.pop();
		if ( iExecStack.empty() )
		{
			std::cout << "result: " << lOp->execute() << std::endl;
		}
		else
		{
			iExecStack.top()->addOperand( lOp );
		}
	}

	void characters(
		    const XMLChar ch[],
		    int start,
		    int length
	)
	{
		iChars = Poco::trim( std::string( &ch[start], &ch[start+length] ) );
		
		if ( ! iChars.empty() )
		{
			iExecStack.top()->addOperand( OpPtr( new OpChars( iChars ) ) );
		}
		std::cout << "chars: '" << iChars << "'" << std::endl;
	}

private:
	std::stack< OpPtr > iExecStack;
	std::string iChars;
};

int main( void )
{
	Poco::XML::DOMParser lParser;
	Poco::XML::SAXParser lSaxParser;

	lSaxParser.setContentHandler( new MyHandler() );

	{
		std::stringstream lStream( lSampleXml1 );
		lSaxParser.parse( new InputSource( lStream ) );
	}
	{
		std::stringstream lStream( lSampleXml2 );
		lSaxParser.parse( new InputSource( lStream ) );
	}
	{
		std::stringstream lStream( lSampleXml3 );
		lSaxParser.parse( new InputSource( lStream ) );
	}
	{
		std::stringstream lStream( lSampleXml4 );
		lSaxParser.parse( new InputSource( lStream ) );
	}
	{
		std::stringstream lStream( lSampleXml5 );
		lSaxParser.parse( new InputSource( lStream ) );
	}
	{
		std::stringstream lStream( lSampleXml );
		lSaxParser.parse( new InputSource( lStream ) );
	}

	return 0;
}
