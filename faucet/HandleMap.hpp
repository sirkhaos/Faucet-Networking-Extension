#pragma once

#include "HandlePool.hpp"
#include <faucet/Handled.hpp>

#include <map>
#include <boost/integer.hpp>

/**
 * Provides unique handles to objects and allows access by that handle.
 * The objects must extend the "Handled" class.
 */
class HandleMap {
public:
	HandleMap() {};
	virtual ~HandleMap();

	/**
	 * Associate the element with a unique handle, which is returned.
	 */
	uint32_t allocate(Handled *element);

	/**
	 * Return the element associated with the given handle, or a NULL pointer
	 * if this Manager does not hold an object with the given handle and of
	 * the requested type.
	 *
	 * Note that you will also get NULL as result if the given handle has
	 * previously been associated with NULL.
	 */
	template<typename RequestedType>
	RequestedType *find(uint32_t handle);

	/**
	 * Release the handle-element association.
	 */
	void release(uint32_t handle);

private:
	HandlePool handlePool_;
	std::map<uint32_t, Handled *> content_;
};

HandleMap::~HandleMap() {
	std::map<uint32_t, Handled *>::iterator iter = content_.begin();
	for(;iter != content_.end(); ++iter) {
		handlePool_.release(iter->first);
	}
}

uint32_t HandleMap::allocate(Handled *element) {
	uint32_t handle = handlePool_.allocate();
	content_[handle] = element;
	return handle;
}

template<typename RequestedType>
RequestedType *HandleMap::find(uint32_t handle) {
	std::map<uint32_t, Handled *>::iterator iter = content_.find(handle);
	if(iter == content_.end()) {
		return 0;
	} else {
		return dynamic_cast<RequestedType *>(iter->second);
	}
}

void HandleMap::release(uint32_t handle) {
	content_.erase(handle);
	handlePool_.release(handle);
}
