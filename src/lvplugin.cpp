#include "lvplugin.hpp"


int
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
	LilvNode* lv2_CVPort        		 = lilv_new_uri(world, LV2_CORE__CVPort);
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
		} else if (lilv_port_is_a(self->plugin, lport, lv2_CVPort)) {
			port->type = TYPE_CV;
			if (port->is_input) {
				++self->n_cv_in;
			} else {
				++self->n_cv_out;
			}
		}else if (!port->optional) {
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

namespace Tonic {

  // -----------------------------------------
  //                 ADDER
  // -----------------------------------------

  namespace Tonic_{
  using namespace Tonic_;

  void LV2Effect_::setInput( Generator input ) {
    input_ = input;
    setIsStereoInput(input_.isStereoOutput());
  }

  void LV2Effect_::initialize(const char* pluginUri) {
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
   	incv_buf = new float[self.n_cv_in];
   	outcv_buf = new float[self.n_cv_out]
														;
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
   		} else if (self.ports[p].type == TYPE_CV) {
   			if (self.ports[p].is_input) {
   				lilv_instance_connect_port(self.instance, p, incv_buf + i++);
   			} else {
   				lilv_instance_connect_port(self.instance, p, outcv_buf + o++);
   			}
   		}
   		else {
   			lilv_instance_connect_port(self.instance, p, NULL);
   		}
   	}

   	lilv_instance_activate(self.instance);
   }

  void LV2Effect_::setIsStereoInput( bool stereo ) {
    if (stereo != isStereoInput_){
      dryFrames_.resize(kSynthesisBlockSize, stereo ? 2 : 1, 0);
      outputFrames_.resize(kSynthesisBlockSize, stereo ? 2 : 1, 0);
    }
    isStereoInput_ = stereo;
    isStereoOutput_ = stereo;
  }

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

    std::cerr << "DS:" + dryFrames_.size() << std::endl;
    for(size_t i = 0; i < nSamples; ++i){
      for(size_t j = 0; j < self.n_audio_in; ++j) {
        in_buf[j] = *inptr;
     	}
      ++inptr;
			lilv_instance_run(self.instance, 1);
      for(size_t j = 0; j < self.n_audio_out; ++j) {
        (*outptr) = out_buf[j];
     	}
      ++outptr;
    }
  }
}}
