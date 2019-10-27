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
	TYPE_AUDIO
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
static int
create_ports(LV2Struct* self)
{
	LilvWorld*     world   = self->world;
	const uint32_t n_ports = lilv_plugin_get_num_ports(self->plugin);

	self->n_ports = n_ports;
	self->ports   = (Port*)calloc(self->n_ports, sizeof(Port));

	/* Get default values for all ports */
	float* values = (float*)calloc(n_ports, sizeof(float));
	lilv_plugin_get_port_ranges_float(self->plugin, NULL, NULL, values);

	LilvNode* lv2_InputPort          = lilv_new_uri(world, LV2_CORE__InputPort);
	LilvNode* lv2_OutputPort         = lilv_new_uri(world, LV2_CORE__OutputPort);
	LilvNode* lv2_AudioPort          = lilv_new_uri(world, LV2_CORE__AudioPort);
	LilvNode* lv2_ControlPort        = lilv_new_uri(world, LV2_CORE__ControlPort);
	LilvNode* lv2_connectionOptional = lilv_new_uri(world, LV2_CORE__connectionOptional);

	for (uint32_t i = 0; i < n_ports; ++i) {
		Port*           port  = &self->ports[i];
		const LilvPort* lport = lilv_plugin_get_port_by_index(self->plugin, i);

		port->lilv_port = lport;
		port->index     = i;
		port->value     = isnan(values[i]) ? 0.0f : values[i];
		port->optional  = lilv_port_has_property(
			self->plugin, lport, lv2_connectionOptional);

		/* Check if port is an input or output */
		if (lilv_port_is_a(self->plugin, lport, lv2_InputPort)) {
			port->is_input = true;
		} else if (!lilv_port_is_a(self->plugin, lport, lv2_OutputPort) &&
		           !port->optional) {
			return fatal(self, 1, "Port %d is neither input nor output\n", i);
		}

		/* Check if port is an audio or control port */
		if (lilv_port_is_a(self->plugin, lport, lv2_ControlPort)) {
			port->type = TYPE_CONTROL;
		} else if (lilv_port_is_a(self->plugin, lport, lv2_AudioPort)) {
			port->type = TYPE_AUDIO;
			if (port->is_input) {
				++self->n_audio_in;
			} else {
				++self->n_audio_out;
			}
		} else if (!port->optional) {
			return fatal(self, 1, "Port %d has unsupported type\n", i);
		}
	}

	lilv_node_free(lv2_connectionOptional);
	lilv_node_free(lv2_ControlPort);
	lilv_node_free(lv2_AudioPort);
	lilv_node_free(lv2_OutputPort);
	lilv_node_free(lv2_InputPort);
	free(values);

	return 0;
}

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
    		NULL, NULL, NULL, 0, 0, 0, NULL
    	};
  	float* in_buf = nullptr;
  	float* out_buf = nullptr;
  	const LilvPlugin*  plugin = nullptr;

    void computeSynthesisBlock( const SynthesisContext_ &context );

  public:
    LV2Effect_() {

    }

    void initialize(const char* pluginUri) {
    	/* Create world and plugin URI */
    	self.world = lilv_world_new();
    	LilvNode* uri = lilv_new_uri(self.world, pluginUri);
    	if (!uri) {
    		fatal(&self, 2, "Invalid plugin URI <%s>\n", pluginUri);
    	}

    	/* Discover world */
    	lilv_world_load_all(self.world);

    	/* Get plugin */
    	const LilvPlugins* plugins = lilv_world_get_all_plugins(self.world);
    	plugin  = lilv_plugins_get_by_uri(plugins, uri);
    	lilv_node_free(uri);
    	if (!(self.plugin = plugin)) {
    		fatal(&self, 3, "Plugin <%s> not found\n", pluginUri);
    	}

    	/* Create port structures */
    	if (create_ports(&self)) {
    		fatal(&self,3, "Couldn't create ports");
    	}

        	/* Instantiate plugin and connect ports */
    	const uint32_t n_ports = lilv_plugin_get_num_ports(plugin);
    	in_buf = new float[self.n_audio_in];
    	out_buf = new float[self.n_audio_out];
    	std::cerr << self.n_audio_in << std::endl;
    	std::cerr << self.n_audio_out << std::endl;
    	self.instance = lilv_plugin_instantiate(
    		self.plugin, Tonic::sampleRate(), NULL);
    	for (uint32_t p = 0, i = 0, o = 0; p < n_ports; ++p) {
    		if (self.ports[p].type == TYPE_CONTROL) {
    			lilv_instance_connect_port(self.instance, p, &self.ports[p].value);
    		} else if (self.ports[p].type == TYPE_AUDIO) {
    			if (self.ports[p].is_input) {
    				lilv_instance_connect_port(self.instance, p, in_buf + i++);
    			} else {
    				lilv_instance_connect_port(self.instance, p, out_buf + o++);
    			}
    		} else {
    			lilv_instance_connect_port(self.instance, p, NULL);
    		}
    	}

    	lilv_instance_activate(self.instance);
    }

    ~LV2Effect_() {
    	cleanup(0, &self);
    }

    void setControl(const char* name, ControlParameter p) {
    		params[name] = p;
    }

    std::vector<const char*> getControlNames() {
    	std::vector<const char*> names;
  	 	for (uint32_t p = 0, i = 0, o = 0; p < lilv_plugin_get_num_ports(plugin); ++p) {
  	 		const char* symbol = lilv_node_as_string (lilv_port_get_symbol(plugin, self.ports[p].lilv_port));
  	 		std::cerr << symbol << std::endl;
  	 		if (self.ports[p].type == TYPE_CONTROL && !(ends_with(string(symbol), string("_in")) || ends_with(string(symbol), string("_out")) || ends_with(string(symbol), string("_outL"))|| ends_with(string(symbol), string("_outR")) || ends_with(string(symbol), string("_inL")) || ends_with(string(symbol), string("_inR")))) {
    			names.push_back(symbol);
    		}
  	 	}
  	 	LilvNode* node;
  	 	return names;
    }

    void setInput( Generator input );
    void setIsStereoInput(bool stereo);
  };

  inline void LV2Effect_::computeSynthesisBlock( const SynthesisContext_ &context ){

  	for(auto& pair : params) {
  		TonicFloat value = pair.second.tick(context).value;
  		LilvNode*       sym   = lilv_new_string(self.world, pair.first);
  		const LilvPort* port  = lilv_plugin_get_port_by_symbol(plugin, sym);
  		LilvNode *min, *max, *def;
  		lilv_port_get_range(plugin, port, &def, &min, &max);


  		lilv_node_free(sym);
  		if (!port) {
  			fatal(&self, 7, "Unknown port `%s'\n", pair.first);
  		}
  		self.ports[lilv_port_get_index(plugin, port)].value = map_value(0,1,lilv_node_as_float(min),lilv_node_as_float(max), value);
  		lilv_node_free(min);
  		lilv_node_free(max);
  		lilv_node_free(def);
  	}

  	TonicFloat *inptr = &dryFrames_[0];
  	TonicFloat *outptr = &outputFrames_[0];
    unsigned int nSamples = (unsigned int)outputFrames_.size();



    while (nSamples--){
      if(self.n_audio_in > 0) {
        in_buf[0] = *(inptr++);
     	}
//    in_buf[1] = *(inptr++);
   	lilv_instance_run(self.instance, 1);
  	*(outptr++) = out_buf[0];
//  	*(outptr++) = out_buf[1];
    }
  }

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
  };
}
#endif
