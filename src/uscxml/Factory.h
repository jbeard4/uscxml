#ifndef FACTORY_H_5WKLGPRB
#define FACTORY_H_5WKLGPRB

#include "uscxml/Message.h"

#ifdef BUILD_AS_PLUGINS
#include "Pluma/Pluma.hpp"
#endif

#include <string>
#include <set>
#include <boost/shared_ptr.hpp>

namespace uscxml {

// see http://stackoverflow.com/questions/228005/alternative-to-itoa-for-converting-integer-to-string-c
template <typename T> std::string toStr(T tmp) {
	std::ostringstream out;
	out << tmp;
	return out.str();
}

template <typename T> T strTo(std::string tmp) {
	T output;
	std::istringstream in(tmp);
	in >> output;
	return output;
}

inline bool isNumeric( const char* pszInput, int nNumberBase) {
	std::string base = "0123456789ABCDEF";
	std::string input = pszInput;
	return (input.find_first_not_of(base.substr(0, nNumberBase)) == std::string::npos);
}

class Interpreter;

#if 0
class ExecutableContent {
public:
	ExecutableContent() {};
	virtual boost::shared_ptr<ExecutableContentImpl>* create(Interpreter* interpreter) = 0;
};
#endif

class IOProcessorImpl {
public:
	IOProcessorImpl() {};
	virtual ~IOProcessorImpl() {};
	virtual boost::shared_ptr<IOProcessorImpl> create(Interpreter* interpreter) = 0;
	virtual std::set<std::string> getNames() = 0;

	virtual void setInterpreter(Interpreter* interpreter) {
		_interpreter = interpreter;
	}
	void setInvokeId(const std::string& invokeId) {
		_invokeId = invokeId;
	}
	void setType(const std::string& type) {
		_type = type;
	}

	virtual Data getDataModelVariables() = 0;
	virtual void send(const SendRequest& req) = 0;

	virtual void runOnMainThread() {};

	void returnEvent(Event& event);

protected:
	Interpreter* _interpreter;
	std::string _invokeId;
	std::string _type;
};

class IOProcessor {
public:
	IOProcessor() : _impl() {}
	IOProcessor(boost::shared_ptr<IOProcessorImpl> const impl) : _impl(impl) { }
	IOProcessor(const IOProcessor& other) : _impl(other._impl) { }
	virtual ~IOProcessor() {};

	operator bool()                           const     {
		return _impl;
	}
	bool operator< (const IOProcessor& other) const     {
		return _impl < other._impl;
	}
	bool operator==(const IOProcessor& other) const     {
		return _impl == other._impl;
	}
	bool operator!=(const IOProcessor& other) const     {
		return _impl != other._impl;
	}
	IOProcessor& operator= (const IOProcessor& other)   {
		_impl = other._impl;
		return *this;
	}

	virtual Data getDataModelVariables() const {
		return _impl->getDataModelVariables();
	};
	virtual void send(const SendRequest& req)  {
		return _impl->send(req);
	};
	virtual void runOnMainThread()             {
		return _impl->runOnMainThread();
	}

	void setInterpreter(Interpreter* interpreter) {
		_impl->setInterpreter(interpreter);
	}
	void setInvokeId(const std::string& invokeId) {
		_impl->setInvokeId(invokeId);
	}
	void setType(const std::string& type) {
		_impl->setType(type);
	}

protected:
	boost::shared_ptr<IOProcessorImpl> _impl;
};

class InvokerImpl : public IOProcessorImpl {
public:
	virtual void invoke(const InvokeRequest& req) = 0;
	virtual boost::shared_ptr<IOProcessorImpl> create(Interpreter* interpreter) = 0;
};

class Invoker : public IOProcessor {
public:
	Invoker() : _impl() {}
	Invoker(boost::shared_ptr<InvokerImpl> const impl) : IOProcessor(impl), _impl(impl) { }
	Invoker(const Invoker& other) : IOProcessor(other._impl), _impl(other._impl) { }
	virtual ~Invoker() {};

	operator bool()                       const {
		return _impl;
	}
	bool operator< (const Invoker& other) const {
		return _impl < other._impl;
	}
	bool operator==(const Invoker& other) const {
		return _impl == other._impl;
	}
	bool operator!=(const Invoker& other) const {
		return _impl != other._impl;
	}
	Invoker& operator= (const Invoker& other)   {
		_impl = other._impl;
		IOProcessor::_impl = _impl;
		return *this;
	}

	virtual void invoke(InvokeRequest& req)     {
		_impl->invoke(req);
	}

protected:
	boost::shared_ptr<InvokerImpl> _impl;
};

class DataModelImpl {
public:
	virtual ~DataModelImpl() {}
	virtual boost::shared_ptr<DataModelImpl> create(Interpreter* interpreter) = 0;
	virtual std::set<std::string> getNames() = 0;

	virtual bool validate(const std::string& location, const std::string& schema) = 0;
	virtual void setEvent(const Event& event) = 0;
	virtual Data getStringAsData(const std::string& content) = 0;

	virtual void registerIOProcessor(const std::string& name, const IOProcessor& ioprocessor) = 0;

	// foreach
	virtual uint32_t getLength(const std::string& expr) = 0;
	virtual void pushContext() = 0;
	virtual void popContext() = 0;

	virtual void eval(const std::string& expr) = 0;
	virtual std::string evalAsString(const std::string& expr) = 0;
	virtual bool evalAsBool(const std::string& expr) = 0;
	virtual void assign(const std::string& location, const std::string& expr) = 0;
	virtual void assign(const std::string& location, const Data& data) = 0;

protected:
	Interpreter* _interpreter;
	std::string _sessionId;
	std::string _name;

};

class DataModel {
public:
	DataModel() : _impl() {}
	DataModel(boost::shared_ptr<DataModelImpl> const impl) : _impl(impl) { }
	DataModel(const DataModel& other) : _impl(other._impl) { }
	virtual ~DataModel() {};

	operator bool()                         const     {
		return _impl;
	}
	bool operator< (const DataModel& other) const     {
		return _impl < other._impl;
	}
	bool operator==(const DataModel& other) const     {
		return _impl == other._impl;
	}
	bool operator!=(const DataModel& other) const     {
		return _impl != other._impl;
	}
	DataModel& operator= (const DataModel& other)     {
		_impl = other._impl;
		return *this;
	}

	virtual bool validate(const std::string& location, const std::string& schema) {
		return _impl->validate(location, schema);
	}
	virtual void setEvent(const Event& event) {
		return _impl->setEvent(event);
	}
	virtual Data getStringAsData(const std::string& content) {
		return _impl->getStringAsData(content);
	}

	virtual uint32_t getLength(const std::string& expr) {
		return _impl->getLength(expr);
	}
	virtual void pushContext() {
		return _impl->pushContext();
	}
	virtual void popContext() {
		return _impl->popContext();
	}

	virtual void registerIOProcessor(const std::string& name, const IOProcessor& ioprocessor) {
		_impl->registerIOProcessor(name, ioprocessor);
	}

	virtual void eval(const std::string& expr) {
		return _impl->eval(expr);
	}
	virtual std::string evalAsString(const std::string& expr) {
		return _impl->evalAsString(expr);
	}
	virtual bool evalAsBool(const std::string& expr) {
		return _impl->evalAsBool(expr);
	}

	virtual void assign(const std::string& location, const std::string& expr) {
		return _impl->assign(location, expr);
	}
	virtual void assign(const std::string& location, const Data& data) {
		return _impl->assign(location, data);
	}

protected:
	boost::shared_ptr<DataModelImpl> _impl;
};

class Factory {
public:
	void registerIOProcessor(IOProcessorImpl* ioProcessor);
	void registerDataModel(DataModelImpl* dataModel);
	void registerInvoker(InvokerImpl* invoker);

	static boost::shared_ptr<DataModelImpl> createDataModel(const std::string& type, Interpreter* interpreter);
	static boost::shared_ptr<IOProcessorImpl> createIOProcessor(const std::string& type, Interpreter* interpreter);
	static boost::shared_ptr<InvokerImpl> createInvoker(const std::string& type, Interpreter* interpreter);

	static Factory* getInstance();

	std::map<std::string, DataModelImpl*> _dataModels;
	std::map<std::string, std::string> _dataModelAliases;
	std::map<std::string, IOProcessorImpl*> _ioProcessors;
	std::map<std::string, std::string> _ioProcessorAliases;
	std::map<std::string, InvokerImpl*> _invokers;
	std::map<std::string, std::string> _invokerAliases;

	static std::string pluginPath;

protected:
#ifdef BUILD_AS_PLUGINS
	pluma::Pluma pluma;
#endif

	Factory();
	~Factory();
	static Factory* _instance;

};


}

#endif /* end of include guard: FACTORY_H_5WKLGPRB */
