/*!
*
* \author: einhornimmond
*
* \date: 07.03.19
*
* \brief: error
*/

#ifndef DR_LUA_WEB_MODULE_ERROR_ERROR_LIST_H
#define DR_LUA_WEB_MODULE_ERROR_ERROR_LIST_H

#include "Error.h"
#include "Warning.h"
#include <stack>
#include <vector>

#include "rapidjson/document.h"

class NotificationList : public INotificationCollection
{
public:
	NotificationList();
	~NotificationList();

	// push error, error will be deleted in deconstructor
	virtual void addError(Error* error, bool log = true);
	virtual void addWarning(Warning* warning, bool log = true);

	// return error on top of stack, please delete after using
	Error* getLastError();
	Warning* getLastWarning();

	inline size_t errorCount() { return mErrorStack.size(); }
	inline size_t warningCount() { return mWarningStack.size(); }

	// delete all errors
	void clearErrors();

	static int moveErrors(NotificationList* recv, NotificationList* send) {
		return recv->getErrors(send);
	}
	int getErrors(NotificationList* send);
	int getWarnings(NotificationList* send);

	void printErrors();
	std::vector<std::string> getErrorsArray();
	rapidjson::Value getErrorsArray(rapidjson::Document::AllocatorType& alloc);
	std::vector<std::string> getWarningsArray();
	rapidjson::Value getWarningsArray(rapidjson::Document::AllocatorType& alloc);	

	// must be overload by parent classes
	virtual void sendErrorsAsEmail(std::string rawHtml = "", bool copy = false) { printf("sendErrorsAsEmail not implemented\n"); }

protected:
	std::stack<Error*> mErrorStack;
	std::stack<Warning*> mWarningStack;	
};

#endif // DR_LUA_WEB_MODULE_ERROR_ERROR_LIST_H
