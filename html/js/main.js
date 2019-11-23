var version;
var textarea = document.getElementById("editor");
var myLayout;
var socket;
var lastControlListData;
var patchFile;
var patchList;

function isMobileBrowser() {
  var check = false;
  (function(a){if(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|mobile.+firefox|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows ce|xda|xiino/i.test(a)||/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(a.substr(0,4))) check = true;})(navigator.userAgent||navigator.vendor||window.opera);
  return check;
};

class MidiPatch {
    constructor(port) {
        this.port = port;
    }

    connect() {
        $("#connectionstatus span").html("Connecting");
        socket = new ReconnectingWebSocket("ws://" + window.location.hostname + ":" + this.port);

        socket.onopen = function(e) {
        $("#connectionstatus span").html("Connected");
        };

        socket.onmessage = function(event) {
            var obj;
            try { 
                obj = jQuery.parseJSON(event.data);
            } catch(e) {
                console.log(e);
                return;
            }

            if(obj.type == "config") {
                if(analyser.isInitialized())
                    return;          
                patchFile = obj.data.patchFile;     
                version = obj.data.version;
                midipatch.bufferFrames = obj.data.bufferFrames;
                midipatch.channels = obj.data.channels;
                midipatch.sampleRate = obj.data.sampleRate;
            } else if(obj.type == "user-joined") {
               // midipatch.confirmConcurrentMod();
            } else if(obj.type == "patch-list") {
                patchList = obj.list;
                library.updatePatchList(patchList);
            } else if(obj.type == "audio-buffer") {
                // This gives us the actual array that contains the data
                var data = atob(obj.data);
                var channel0 = analyser.getChannel(0);
                var channel1 = analyser.getChannel(1);
                if(channel0 == null || channel1 == null)
                    return;
                
                for (var i = 0; i < 512; i++) {
                channel0[i] = (data.charCodeAt(i * 2) / 127.0) - 1.0;
                channel1[i] = (data.charCodeAt(i * 2 + 1) / 127.0) -1.0;
                }
            } else if (obj.type == "update-control") {
                $("#" + obj.data.parent + "\\." + obj.data.child).val(obj.data.value);
                $("#" + obj.data.parent + "\\." + obj.data.child).trigger("change");
                $("#" + obj.data.parent + "\\." + obj.data.child).parent().parent().get(0).scrollIntoView();
            } else if (obj.type == "update-log") {
                if(obj.severity == 1) {
                    dialogs.errorDialog(obj.title, obj.msg);
                }
                if (!$("#log").length)
                    return;
 
                log.update(obj)
                 $("#log").parent().attr("id","logparent");
                scrollToBottom("logparent");
            } else if (obj.type == "update-note") {
            if(obj.isOn) {
                $(".key").eq(obj.note).css("background-color","red");
                $(".key").get(obj.note).scrollIntoView();
            }
            else
                $(".key").eq(obj.note).css("background-color","");

            } else if (obj.type == "display") {
            } else if (obj.type == "control-list") {
                if (lastControlListData == event.data) {
                    return;
                }
                lastControlListData = event.data;
                rack.updateControlList(obj);
            }
        };

        socket.onclose = function(event) {
        $("#connectionstatus span").html("Disconnected");
        };

        socket.onerror = function(error) {
        $("#connectionstatus span").html("Disconnected");
        };
    }
    restart() {
        $.notify("Restarting", "success")
        var obj = {
            "type": "restart"
        };
        socket.send(JSON.stringify(obj));
    }
    
    sendControlChange(name, value) {
        var obj = {
            "type": "set-control",
            "name": name,
            "value": parseFloat(value)
        };
        socket.send(JSON.stringify(obj));
    }

    exportToLibrary(name, author, description, expCode, expParameters, expLayout) {
        var code = "";
        var parameters = "";
        var layout = "";
        if(expCode)
        code = editor.getValue();
        if(expParameters)
        parameters = JSON.stringify(getControlParameters());
        if(expLayout)
        layout = JSON.stringify(myLayout.toConfig());
        var obj = {
            "type": "update-patch",
            "name": name,
            "author": author,
            "runtimeName": "midipatch",
            "runtimeVersion": version,
            "description": description,
            "date": Date.now() / 1000,
            "code": code,
            "layout": layout,
            "parameters": parameters,
            "keyboardBindings": null,
            "midiBindings": null
        };
        socket.send(JSON.stringify(obj));
    }

    deleteFromLibrary(name,author) {
        var obj = {
            "type": "delete-patch",
            "name": name,
            "author": author,
            "runtimeName": "",
            "runtimeVersion": "",
            "description": "",
            "date": -1,
            "code": "",
            "layout": "",
            "parameters": "",
            "keyboardBindings": null,
            "midiBindings": null
        };
        socket.send(JSON.stringify(obj));
    }

    importFromLibrary(name, author, revision, impCode, impParameters, impLayout) {
        for(var i = 0; i < patchList.length; ++i) {
            if(patchList[i].name == name && patchList[i].author == author && patchList[i].revision == revision) {
            if(impParameters) {
                var parObj = JSON.parse(patchList[i].parameters);
                Object.keys(parObj).forEach(function(key) {
                $(("#" + key).replace(".","\\.")).val(parObj[key]);
                $(("#" + key).replace(".","\\.")).trigger("change");
                });
                localStorage.setItem("savedControlParameters", patchList[i].parameters);
            }

            if(impLayout) {
                localStorage.setItem("savedLayoutState",  patchList[i].layout);
                window.location.reload();
            }

            if(impCode) {
                editor.setValue(patchList[i].code);
                this.storePatch();
            }
            }
        }
    }

    noteOn(key, velocity) {
        var obj = {
            "type": "note-on",
            "note": key,
            "velocity": velocity
        };
        socket.send(JSON.stringify(obj));
    }

    noteOff(key, velocity) {
        var obj = {
            "type": "note-off",
            "note": key
        };
        socket.send(JSON.stringify(obj));
    }

    clearAllNotes() {
        $(".playing").each(function( index ) {
        $(this).removeClass('playing');
        });
        var obj = {
            "type": "clear-all-notes"
        };
        socket.send(JSON.stringify(obj));
    }
    
    setEditorMode(mode) {
        editor.setMode(mode);
    }
    
    setEditorValue(v) {
        editor.setValue(v);
    }

    setEditorValueAsk(v) {
        editor.setValueAsk(v);
    }

    resetLayout() {
        localStorage.removeItem('savedLayoutState');
        window.location.reload();
    }
    
    loadPatchAsk() {
        $.get("/cgi-bin/loadPatch.cgi", function(data) {
            if (data.length == 0)
                return;
            $.notify("Loading", "success")
            editor.setValueAsk(data);
        });
    }


    loadPatch() {
        $.get("/cgi-bin/loadPatch.cgi", function(data) {
            if (data.length == 0)
                return;
            $.notify("Loading", "success")
            editor.setValue(data);
        });
    } 

    compareEditorAndServerPatch(editorContent) {
        var mp = this;
        $.get("/cgi-bin/loadPatch.cgi", function(data) {
            if(editorContent.localeCompare(data) != 0) {
                dialogs.askContentDifferDialog(editorContent);
            } else {
                editor.setValue(data);
            }
        });
    }

    storePatch() {
        var mp = this;
        var code = editor.getValue();
        $.post('/cgi-bin/storePatch.cgi', {
                code: code
            },
            function(data, status, jqXHR) {
                $.notify("Patch saved", "success");
                if ($("#restartonsave").prop('checked')) {
                    mp.restart();
                }
            }
        )
    }

    getControlParameters() {
        var cp = {};
        $("#rackdiv input").each(function() {
            cp[$(this).attr("id")] = $(this).val();
        })
        return cp;
    }
}

var midipatch = new MidiPatch(8080);
var dialogs = new MPDialogs(midipatch);
var toolbar = new MPToolbar(midipatch);
var piano = new MPPiano(midipatch);
var analyser = new MPAnalyser(midipatch);
var editor = new MPEditor(midipatch);
var log = new MPLog(editor);
var library = new MPLibrary(midipatch);
var rack = new MPRack(midipatch);

function scrollToBottom(id) {
    var element = document.getElementById(id);
    element.scrollTop = element.scrollHeight - element.clientHeight;
}

function makeLayoutDesktop() {
    //not used but saved as an example
    var vanillaConfig = {
        content: [{
            type: 'column',
            content: [{
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'Toolbar',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'Editor',
                }, {
                    type: 'component',
                    componentName: 'Rack',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'Keyboard',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'Log',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'Analyser',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'Library',
                }]
            }]
        }]
    };
    //generated by creating a layout and grabbing the config from golden layout
    var config = {"settings":{"hasHeaders":true,"constrainDragToContainer":true,"reorderEnabled":true,"selectionEnabled":false,"popoutWholeStack":false,"blockedPopoutsThrowError":true,"closePopoutsOnUnload":true,"showPopoutIcon":true,"showMaximiseIcon":true,"showCloseIcon":true,"responsiveMode":"onload"},"dimensions":{"borderWidth":5,"minItemHeight":10,"minItemWidth":10,"headerHeight":20,"dragProxyWidth":300,"dragProxyHeight":200},"labels":{"close":"close","maximise":"maximise","minimise":"minimise","popout":"open in new window","popin":"pop in","tabDropdown":"additional tabs"},"content":[{"type":"column","isClosable":true,"reorderEnabled":true,"title":"","content":[{"type":"row","isClosable":true,"reorderEnabled":true,"title":"","height":6.036174480217071,"content":[{"type":"stack","width":100,"isClosable":true,"reorderEnabled":true,"title":"","activeItemIndex":0,"content":[{"type":"component","componentName":"Toolbar","isClosable":true,"reorderEnabled":true,"title":"Toolbar"}]}]},{"type":"row","isClosable":true,"reorderEnabled":true,"title":"","height":56.1677924052178,"content":[{"type":"stack","width":49.01459070195102,"isClosable":true,"reorderEnabled":true,"title":"","activeItemIndex":0,"content":[{"type":"component","componentName":"Editor","isClosable":true,"reorderEnabled":true,"title":"Editor"}]},{"type":"stack","width":24.548420830167295,"isClosable":true,"reorderEnabled":true,"title":"","activeItemIndex":0,"content":[{"type":"component","componentName":"Rack","isClosable":true,"reorderEnabled":true,"title":"Rack"}]},{"type":"stack","header":{},"isClosable":true,"reorderEnabled":true,"title":"","activeItemIndex":0,"width":26.436988467881683,"content":[{"type":"component","componentName":"Analyser","isClosable":true,"reorderEnabled":true,"title":"Analyser"}]}]},{"type":"row","isClosable":true,"reorderEnabled":true,"title":"","height":37.79603311456511,"content":[{"type":"stack","width":33.50042033149725,"isClosable":true,"reorderEnabled":true,"title":"","activeItemIndex":1,"content":[{"type":"component","componentName":"Log","isClosable":true,"reorderEnabled":true,"title":"Log"},{"type":"component","componentName":"Library","isClosable":true,"reorderEnabled":true,"title":"Library"}]},{"type":"stack","header":{},"isClosable":true,"reorderEnabled":true,"title":"","activeItemIndex":0,"width":66.49957966850272,"content":[{"type":"component","componentName":"Keyboard","isClosable":true,"reorderEnabled":true,"title":"Keyboard"}]}]}]}],"isClosable":true,"reorderEnabled":true,"title":"","openPopouts":[],"maximisedItemId":null};

    var savedState = localStorage.getItem('savedLayoutState');

    if (savedState !== null) {
        myLayout = new GoldenLayout(JSON.parse(savedState));
    } else {
        myLayout = new GoldenLayout(config);
    }

    myLayout.registerComponent('Toolbar', function(container, componentState) {
        container.getElement().html(toolbar.makeHTML());
    });

    myLayout.registerComponent('Analyser', function(container, componentState) {
        container.getElement().html(analyser.makeHTML());
    });

    myLayout.registerComponent('Library', function(container, componentState) {
        container.getElement().html(library.makeHTML());
    });

   myLayout.registerComponent('Editor', function(container, componentState) {
        container.getElement().html(editor.makeHTML());

        window.setTimeout(function() {
            editor.init();
        }, 100);
    });

    myLayout.registerComponent('Rack', function(container, componentState) {
        container.getElement().html(rack.makeHTML());
    });

    myLayout.registerComponent('Keyboard', function(container, componentState) {
        container.getElement().html(piano.makeHTML());
    });

    myLayout.registerComponent('Log', function(container, componentState) {
        container.getElement().html(log.makeHTML());
    });

    myLayout.on('stateChanged', function() {
        var state = JSON.stringify(myLayout.toConfig());
        localStorage.setItem('savedLayoutState', state);
    });

    myLayout.init();
        myLayout.updateSize($(window).width(), $(window).height() - 10);

    $(window).resize(function() {
        myLayout.updateSize($(window).width(), $(window).height() - 10);
    });

}

function makeLayoutMobile() {
    $("body").append(rack.makeHTML());
    $("body").append(analyser.makeHTML());
}

$(document).ready(function() {
    $("body").append(dialogs.makeHTML());

    if(isMobileBrowser()) {
        makeLayoutMobile();
        midipatch.connect();
        analyser.bind();
    } else {
        makeLayoutDesktop();
        piano.prepare();
        midipatch.connect();
        toolbar.bind();
        piano.bind();
        analyser.bind();
        
        //alt, shift, ctrl, etc must be lowercase 
        keyboardJS.bind('alt+r', function(e) {
            midipatch.restart();
            e.preventDefault();
        });

        keyboardJS.bind('alt+s', function(e) {
            midipatch.storePatch();
            e.preventDefault();
        });
    }

    var cpStr = localStorage.getItem("savedControlParameters"); 
    if(cpStr) {
      var cp = JSON.parse(cpStr);
      Object.keys(cp).forEach(function(key) {
        $(("#" + key).replace(".","\\.")).val(cp[key]);
        $(("#" + key).replace(".","\\.")).trigger("change");
      });
    }
});
