#ifndef SRC_PSCRIPT_SINGLETON_HPP_
#define SRC_PSCRIPT_SINGLETON_HPP_

#include <cassert>
#include <mutex>
#include "PatchScript.hpp"

namespace midipatch {

using namespace patchscript;
class PScriptsSingleton {
private:
	static PatchScript* instance_;
public:
	static void init(unsigned int sampleRate) {
		assert(instance_ == nullptr);
		instance_ = new PatchScript(sampleRate);
	}

	static PatchScript* getInstance() {
		assert(instance_ != nullptr);
		return instance_;
	}

	static void destroy() {
		assert(instance_ != nullptr);
		delete instance_;
		instance_ = nullptr;
	}
};
} /* namespace midipatch */

#endif /* SRC_PSCRIPT_SINGLETON_HPP_ */
