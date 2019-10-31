#ifndef LV2PLUGIN_HPP_
#define LV2PLUGIN_HPP_
/*
  Copyright 2007-2019 David Robillard <http://drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "lilv-0/lilv/lilv.h"

#include "lv2.h"

#include "../third/Tonic/src/Tonic/Effect.h"
#include "../third/Tonic/src/Tonic/ControlParameter.h"
#include <cassert>
/** Port type (only float ports are supported) */
typedef enum {
	TYPE_CONTROL,
	TYPE_AUDIO,
	TYPE_CV,
	TYPE_OTHER
} PortType;

/** Runtime port information */
typedef struct {
	const LilvPort* lilv_port;  ///< Port description
	PortType        type;       ///< Datatype
	uint32_t        index;      ///< Port index
	float           value;      ///< Control value (if applicable)
	bool            is_input;   ///< True iff an input port
	bool            optional;   ///< True iff connection optional
} Port;

/** Application state */
typedef struct {
	LilvWorld*        world;
	const LilvPlugin* plugin;
	LilvInstance*     instance;
	unsigned          n_ports;
	unsigned          n_audio_in;
	unsigned          n_audio_out;
	unsigned          n_cv_in;
	unsigned          n_cv_out;
	Port*             ports;
} LV2Struct;

/** Clean up all resources. */
static int
cleanup(int status, LV2Struct* self)
{
	lilv_instance_free(self->instance);
	lilv_world_free(self->world);
	free(self->ports);
	return status;
}

/** Print a fatal error and clean up for exit. */
static int
fatal(LV2Struct* self, int status, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return self ? cleanup(status, self) : status;
}

/**
   Create port structures from data (via create_port()) for all ports.
*/
static int create_ports(LV2Struct* self);

static inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

static float map_value(float a0, float a1, float b0, float b1, float a)
{
	return b0 + (b1 - b0) * ((a-a0)/(a1-a0));
}
namespace Tonic {

  // -----------------------------------------
  //                 ADDER
  // -----------------------------------------

  namespace Tonic_{
  using namespace Tonic_;
  class LV2Effect_ : public WetDryEffect_ {

  protected:
  	std::map<const char*, ControlParameter> params;
    LV2Struct self = {
    		NULL, NULL, NULL, 0, 0, 0, 0, 0, NULL
    	};
  	float* in_buf = nullptr;
  	float* out_buf = nullptr;
  	float* incv_buf = nullptr;
  	float* outcv_buf = nullptr;
  	const LilvPlugin*  plugin = nullptr;

    void computeSynthesisBlock( const SynthesisContext_ &context );

  public:
    LV2Effect_() {

    }


    ~LV2Effect_() {
    	cleanup(0, &self);
    }

    void initialize(const char* pluginUri);
    void setControl(const char* name, ControlParameter p) {
    		params[name] = p;
    }

    std::vector<const char*> getControlNames() {
    	std::vector<const char*> names;
  	 	for (uint32_t p = 0, i = 0, o = 0; p < lilv_plugin_get_num_ports(plugin); ++p) {
  	 		const char* symbol = lilv_node_as_string (lilv_port_get_symbol(plugin, self.ports[p].lilv_port));
  	 		if (self.ports[p].type == TYPE_CONTROL && !(ends_with(string(symbol), string("_in")) || ends_with(string(symbol), string("_out")) || ends_with(string(symbol), string("_outL"))|| ends_with(string(symbol), string("_outR")) || ends_with(string(symbol), string("_inL")) || ends_with(string(symbol), string("_inR")))) {
    			names.push_back(symbol);
    		}
  	 	}
  	 	return names;
    }

    std::vector<float> getControlDefaults() {
    	std::vector<float> defaults;
  	 	for (uint32_t p = 0, i = 0, o = 0; p < lilv_plugin_get_num_ports(plugin); ++p) {
    		const LilvNode* sym = lilv_port_get_symbol(plugin, self.ports[p].lilv_port);
    		const char* symbol = lilv_node_as_string(sym);
    		const LilvPort* port  = lilv_plugin_get_port_by_symbol(plugin, sym);
    		LilvNode *min, *max, *def;
    		lilv_port_get_range(plugin, port, &def, &min, &max);
  	 		if (self.ports[p].type == TYPE_CONTROL && !(ends_with(string(symbol), string("_in")) || ends_with(string(symbol), string("_out")) || ends_with(string(symbol), string("_outL"))|| ends_with(string(symbol), string("_outR")) || ends_with(string(symbol), string("_inL")) || ends_with(string(symbol), string("_inR")))) {
    			defaults.push_back(lilv_node_as_float(def));
    		}
    		lilv_node_free(min);
    		lilv_node_free(max);
    		lilv_node_free(def);
  	 	}
  	 	return defaults;
    }

    void setInput( Generator input );
    void setIsStereoInput(bool stereo);
  };



}
  class LV2Effect : public TemplatedWetDryEffect<LV2Effect, Tonic_::LV2Effect_>{
  public:
  		LV2Effect(const char* uri) {
  			gen()->initialize(uri);
  		}

      void setControl(const char* name, ControlParameter param) {
      	gen()->setControl(name,param);
      }

      std::vector<const char*> getControlNames() {
      	return gen()->getControlNames();
      }

      std::vector<float> getControlDefaults() {
      	return gen()->getControlDefaults();
      }
  };
}
#endif
