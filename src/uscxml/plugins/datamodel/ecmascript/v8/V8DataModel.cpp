#include "uscxml/Common.h"
#include "V8DataModel.h"
#include "dom/V8Document.h"
#include "dom/V8SCXMLEvent.h"

#include "uscxml/Message.h"
#include <glog/logging.h>

#ifdef BUILD_AS_PLUGINS
#include <Pluma/Connector.hpp>
#endif

namespace uscxml {

#ifdef BUILD_AS_PLUGINS
PLUMA_CONNECTOR
bool connect(pluma::Host& host) {
	host.add( new V8DataModelProvider() );
	return true;
}
#endif

V8DataModel::V8DataModel() {
//  _contexts.push_back(v8::Context::New());
}

boost::shared_ptr<DataModelImpl> V8DataModel::create(Interpreter* interpreter) {
	boost::shared_ptr<V8DataModel> dm = boost::shared_ptr<V8DataModel>(new V8DataModel());
	dm->_interpreter = interpreter;
	v8::Locker locker;
	v8::HandleScope scope;

	dm->_dom = new Arabica::DOM::V8DOM();
//  dom->interpreter = interpreter;
	dm->_dom->xpath = new Arabica::XPath::XPath<std::string>();
	dm->_dom->xpath->setNamespaceContext(interpreter->getNSContext());

	// see http://stackoverflow.com/questions/3171418/v8-functiontemplate-class-instance

	// some free functions
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	global->Set(v8::String::New("In"), v8::FunctionTemplate::New(jsIn, v8::External::New(reinterpret_cast<void*>(dm.get()))), v8::ReadOnly);
	global->Set(v8::String::New("print"), v8::FunctionTemplate::New(jsPrint, v8::External::New(reinterpret_cast<void*>(dm.get()))), v8::ReadOnly);

	v8::Persistent<v8::Context> context = v8::Context::New(0, global);
	v8::Context::Scope contextScope(context);

	// instantiate the document function
	v8::Handle<v8::Function> docCtor = Arabica::DOM::V8Document::getTmpl()->GetFunction();
	v8::Handle<v8::Object> docObj = docCtor->NewInstance();

	Arabica::DOM::V8Document::V8DocumentPrivate* privData = new Arabica::DOM::V8Document::V8DocumentPrivate();
	privData->nativeObj = new Arabica::DOM::Document<std::string>(interpreter->getDocument());
	privData->dom = dm->_dom;
	docObj->SetInternalField(0, Arabica::DOM::V8DOM::toExternal(privData));

	context->Global()->Set(v8::String::New("document"), docObj);

	dm->_contexts.push_back(context);

	// instantiate objects - we have to have a context for that!

	dm->setName(interpreter->getName());
	dm->setSessionId(interpreter->getSessionId());
	dm->eval("_ioprocessors = {};");
	dm->eval("_invokers = {};");
	dm->eval("_x = {};");

	return dm;
}

void V8DataModel::registerIOProcessor(const std::string& name, const IOProcessor& ioprocessor) {
	assign("_ioprocessors['" + name + "']", ioprocessor.getDataModelVariables());
}

void V8DataModel::setSessionId(const std::string& sessionId) {
	_sessionId = sessionId;
	assign("_sessionId", "'" + sessionId + "'");
}

void V8DataModel::setName(const std::string& name) {
	_name = name;
	assign("_name", "'" + name + "'");
}

V8DataModel::~V8DataModel() {
	while(_contexts.size() > 0) {
		_contexts.back().Dispose();
		_contexts.pop_back();
	}
}

void V8DataModel::pushContext() {
	_contexts.push_back(_contexts.back().New(_contexts.back()));
}

void V8DataModel::popContext() {
	if (_contexts.size() > 1) {
		_contexts.back().Dispose();
		_contexts.pop_back();
	}
}

void V8DataModel::initialize() {
}

void V8DataModel::setEvent(const Event& event) {
	_event = event;
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.front());
	v8::Handle<v8::Object> global = _contexts.front()->Global();

	v8::Handle<v8::Function> eventCtor = Arabica::DOM::V8SCXMLEvent::getTmpl()->GetFunction();
	v8::Handle<v8::Object> eventObj = eventCtor->NewInstance();

	Arabica::DOM::V8SCXMLEvent::V8SCXMLEventPrivate* privData = new Arabica::DOM::V8SCXMLEvent::V8SCXMLEventPrivate();
	privData->nativeObj = &_event;
	privData->dom = _dom;
	eventObj->SetInternalField(0, Arabica::DOM::V8DOM::toExternal(privData));

	eventObj->Set(v8::String::New("data"), getDataAsValue(event)); // set data part of _event
	global->Set(v8::String::New("_event"), eventObj);
}

Data V8DataModel::getStringAsData(const std::string& content) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.front());
	v8::Handle<v8::Value> result = evalAsValue(content);
	Data data = getValueAsData(result);
	return data;
}

Data V8DataModel::getValueAsData(const v8::Handle<v8::Value>& value) {
	Data data;
	if (false) {
	} else if (value->IsArray()) {
		v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(value);
		for (int i = 0; i < array->Length(); i++) {
			data.array.push_back(getValueAsData(array->Get(i)));
		}
	} else if (value->IsBoolean()) {
		data.atom = (value->ToBoolean()->Value() ? "true" : "false");
	} else if (value->IsBooleanObject()) {
		LOG(ERROR) << "IsBooleanObject is unimplemented" << std::endl;
	} else if (value->IsDate()) {
		LOG(ERROR) << "IsDate is unimplemented" << std::endl;
	} else if (value->IsExternal()) {
		LOG(ERROR) << "IsExternal is unimplemented" << std::endl;
	} else if (value->IsFalse()) {
		LOG(ERROR) << "IsFalse is unimplemented" << std::endl;
	} else if (value->IsFunction()) {
		LOG(ERROR) << "IsFunction is unimplemented" << std::endl;
	} else if (value->IsInt32()) {
		int32_t prop = value->Int32Value();
		data.atom = toStr(prop);
	} else if (value->IsNativeError()) {
		LOG(ERROR) << "IsNativeError is unimplemented" << std::endl;
	} else if (value->IsNull()) {
		LOG(ERROR) << "IsNull is unimplemented" << std::endl;
	} else if (value->IsNumber()) {
		v8::String::AsciiValue prop(v8::Handle<v8::String>::Cast(v8::Handle<v8::Number>::Cast(value)));
		data.atom = *prop;
	} else if (value->IsNumberObject()) {
		LOG(ERROR) << "IsNumberObject is unimplemented" << std::endl;
	} else if (value->IsObject()) {
		v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(value);
		v8::Local<v8::Array> properties = object->GetPropertyNames();
		for (int i = 0; i < properties->Length(); i++) {
			assert(properties->Get(i)->IsString());
			v8::String::AsciiValue key(v8::Handle<v8::String>::Cast(properties->Get(i)));
			v8::Local<v8::Value> property = object->Get(properties->Get(i));
			data.compound[*key] = getValueAsData(property);
		}
	} else if (value->IsRegExp()) {
		LOG(ERROR) << "IsRegExp is unimplemented" << std::endl;
	} else if(value->IsString()) {
		v8::String::AsciiValue property(v8::Handle<v8::String>::Cast(value));
		data.atom = *property;
	} else if(value->IsStringObject()) {
		LOG(ERROR) << "IsStringObject is unimplemented" << std::endl;
	} else if(value->IsTrue()) {
		LOG(ERROR) << "IsTrue is unimplemented" << std::endl;
	} else if(value->IsUint32()) {
		LOG(ERROR) << "IsUint32 is unimplemented" << std::endl;
	} else if(value->IsUndefined()) {
    data.atom = "undefined";
	}
	return data;
}

v8::Handle<v8::Value> V8DataModel::getDataAsValue(const Data& data) {
	if (data.compound.size() > 0) {
		v8::Handle<v8::Object> value = v8::Object::New();
		std::map<std::string, Data>::const_iterator compoundIter = data.compound.begin();
		while(compoundIter != data.compound.end()) {
			value->Set(v8::String::New(compoundIter->first.c_str()), getDataAsValue(compoundIter->second));
			compoundIter++;
		}
		return value;
	}
	if (data.array.size() > 0) {
		v8::Handle<v8::Object> value = v8::Array::New();
		std::list<Data>::const_iterator arrayIter = data.array.begin();
		uint32_t index = 0;
		while(arrayIter != data.array.end()) {
			value->Set(index++, getDataAsValue(*arrayIter));
			arrayIter++;
		}
		return value;
	}
	if (data.type == Data::VERBATIM) {
		return v8::String::New(data.atom.c_str());
	} else {
		return evalAsValue(data.atom);
	}
}

v8::Handle<v8::Value> V8DataModel::jsPrint(const v8::Arguments& args) {
	if (args.Length() > 0) {
		v8::String::AsciiValue printMsg(args[0]->ToString());
		std::cout << *printMsg;
	}
	return v8::Undefined();
}

v8::Handle<v8::Value> V8DataModel::jsIn(const v8::Arguments& args) {
	V8DataModel* INSTANCE = static_cast<V8DataModel*>(v8::External::Unwrap(args.Data()));
	for (unsigned int i = 0; i < args.Length(); i++) {
		if (args[i]->IsString()) {
			std::string stateName(*v8::String::AsciiValue(args[i]->ToString()));
			if (Interpreter::isMember(INSTANCE->_interpreter->getState(stateName), INSTANCE->_interpreter->getConfiguration())) {
				continue;
			}
		}
		return v8::Boolean::New(false);
	}
	return v8::Boolean::New(true);
}

bool V8DataModel::validate(const std::string& location, const std::string& schema) {
	return true;
}

uint32_t V8DataModel::getLength(const std::string& expr) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.back());
	v8::Handle<v8::Array> result = evalAsValue(expr).As<v8::Array>();
	return result->Length();
}

void V8DataModel::eval(const std::string& expr) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.back());
	evalAsValue(expr);
}

bool V8DataModel::evalAsBool(const std::string& expr) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.back());
	v8::Handle<v8::Value> result = evalAsValue(expr);
	return(result->ToBoolean()->BooleanValue());
}

std::string V8DataModel::evalAsString(const std::string& expr) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.back());
	v8::Handle<v8::Value> result = evalAsValue(expr);
  if (result->IsObject()) {
    Data data = getValueAsData(result);
    return toStr(data);
  }
	v8::String::AsciiValue data(result->ToString());
	return std::string(*data);
}

void V8DataModel::assign(const std::string& location, const Data& data) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.front());

	std::stringstream ssJSON;
	ssJSON << data;
	assign(location, ssJSON.str());
}

void V8DataModel::assign(const std::string& location, const std::string& expr) {
	v8::Locker locker;
	v8::HandleScope handleScope;
	v8::Context::Scope contextScope(_contexts.back());
	evalAsValue(location + " = " + expr);
}

v8::Handle<v8::Value> V8DataModel::evalAsValue(const std::string& expr) {
	v8::TryCatch tryCatch;
	v8::Handle<v8::String> source = v8::String::New(expr.c_str());
	v8::Handle<v8::Script> script = v8::Script::Compile(source);

	v8::Handle<v8::Value> result;
	if (!script.IsEmpty())
		result = script->Run();

	if (script.IsEmpty() || result.IsEmpty()) {
		// throw an exception
		assert(tryCatch.HasCaught());
		Event exceptionEvent;
		exceptionEvent.name = "error.execution";

		std::string exceptionString(*v8::String::AsciiValue(tryCatch.Exception()));
		exceptionEvent.compound["exception"] = Data(exceptionString, Data::VERBATIM);;

		v8::Handle<v8::Message> message = tryCatch.Message();
		if (!message.IsEmpty()) {
			std::string filename(*v8::String::AsciiValue(message->GetScriptResourceName()));
			exceptionEvent.compound["filename"] = Data(filename, Data::VERBATIM);

			std::string sourceLine(*v8::String::AsciiValue(message->GetSourceLine()));
			exceptionEvent.compound["sourceline"] = Data(sourceLine, Data::VERBATIM);

			std::stringstream ssLineNumber;
			int lineNumber = message->GetLineNumber();
			ssLineNumber << lineNumber;
			exceptionEvent.compound["linenumber"] = Data(ssLineNumber.str());

			int startColumn = message->GetStartColumn();
			int endColumn = message->GetEndColumn();
			std::stringstream ssUnderline;
			for (int i = 0; i < startColumn; i++)
				ssUnderline << " ";
			for (int i = startColumn; i < endColumn; i++)
				ssUnderline << "^";
			exceptionEvent.compound["sourcemark"] = Data(ssUnderline.str(), Data::VERBATIM);

			std::string stackTrace(*v8::String::AsciiValue(tryCatch.StackTrace()));
			exceptionEvent.compound["stacktrace"] = Data(stackTrace, Data::VERBATIM);

		}

		_interpreter->receiveInternal(exceptionEvent);
		throw(exceptionEvent);
	}

	return result;
}

}