#ifndef CLIENTCOMMANDBASE_H
#define CLIENTCOMMANDBASE_H

namespace QtuC
{

/**
 * Class ClientCommandBase
 * Base class of all client command.*/
class ClientCommandBase
{

public:
	ClientCommandBase();


	~ClientCommandBase();


	virtual ClientCommandBase * fromDomElement( const QDomElement & fromElement ) = 0;


	virtual QDomElement getDomElement() const = 0;


	virtual bool isValid() const = 0;

	packetClass getClass();


private:

	static  const QString xmlName;
	const packetClass class;

	static const QString getXmlName() const;
};

}	//QtuC::
#endif //CLIENTCOMMANDBASE_H

