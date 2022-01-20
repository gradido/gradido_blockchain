#include "NotificationList.h"



NotificationList::NotificationList()
{

}

NotificationList::~NotificationList()
{
	while (mErrorStack.size() > 0) {
		delete mErrorStack.top();
		mErrorStack.pop();
	}

	while (mWarningStack.size() > 0) {
		delete mWarningStack.top();
		mWarningStack.pop();
	}
}

void NotificationList::addError(Error* error, bool log/* = true */)
{
	mErrorStack.push(error);
}

void NotificationList::addWarning(Warning* warning, bool log/* = true*/)
{
	mWarningStack.push(warning);
}

Error* NotificationList::getLastError()
{
	if (mErrorStack.size() == 0) {
		return nullptr;
	}

	Error* error = mErrorStack.top();
	if (error) {
		mErrorStack.pop();
	}

	return error;
}

Warning* NotificationList::getLastWarning()
{
	if (mWarningStack.size() == 0) {
		return nullptr;
	}

	Warning* warning = mWarningStack.top();
	if (warning) {
		mWarningStack.pop();
	}

	return warning;
}

void NotificationList::clearErrors()
{
	while (mErrorStack.size()) {
		auto error = mErrorStack.top();
		if (error) {
			delete error;
		}
		mErrorStack.pop();
	}
}


int NotificationList::getErrors(NotificationList* send)
{
	Error* error = nullptr;
	int iCount = 0;
	while (error = send->getLastError()) {
		addError(error, false);
		iCount++;
	}
	return iCount;
}

int NotificationList::getWarnings(NotificationList* send)
{
	Warning* warning = nullptr;
	int iCount = 0;
	while (warning = send->getLastWarning()) {
		addWarning(warning, false);
		iCount++;
	}
	return iCount;
}

void NotificationList::printErrors()
{
	while (mErrorStack.size() > 0) {
		auto error = mErrorStack.top();
		mErrorStack.pop();
		printf(error->getString().data());
		delete error;
	}
}

std::vector<std::string> NotificationList::getErrorsArray()
{
	std::vector<std::string> result;
	result.reserve(mErrorStack.size());

	while (mErrorStack.size() > 0) {
		auto error = mErrorStack.top();
		mErrorStack.pop();
		//result->add(error->getString());
		result.push_back(error->getString());
		delete error;
	}
	return result;
}

rapidjson::Value NotificationList::getErrorsArray(rapidjson::Document::AllocatorType& alloc)
{
	rapidjson::Value value;
	value.SetArray();
	while (mErrorStack.size() > 0) {
		auto error = mErrorStack.top();
		mErrorStack.pop();
		value.PushBack(rapidjson::Value(error->getString().data(), alloc), alloc);
		delete error;
	}
	return value;
}

std::vector<std::string> NotificationList::getWarningsArray()
{
	std::vector<std::string> result;
	result.reserve(mWarningStack.size());

	while (mWarningStack.size() > 0) {
		auto warning = mWarningStack.top();
		mWarningStack.pop();
		//result->add(error->getString());
		result.push_back(warning->getString());
		delete warning;
	}
	return result;
}

rapidjson::Value NotificationList::getWarningsArray(rapidjson::Document::AllocatorType& alloc)
{
	rapidjson::Value value;
	value.SetArray();
	while (mWarningStack.size() > 0) {
		auto warning = mWarningStack.top();
		mWarningStack.pop();
		value.PushBack(rapidjson::Value(warning->getString().data(), alloc), alloc);
		delete warning;
	}
	return value;
}

