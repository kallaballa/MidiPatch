var version;
var textarea = document.getElementById("editor");
var myLayout;
var socket;
var lastControlListData;
var patchFile;
var patchList;


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
                var bufferFrames = obj.data.bufferFrames;
                var channels = obj.data.channels;
                var sampleRate = obj.data.sampleRate;

                analyser.init(sampleRate, bufferFrames, channels);
                
            } else if(obj.type == "user-joined") {
                confirmConcurrentMod();
            } else if(obj.type == "patch-list") {
                patchList = obj.list;
                library.updatePatchList(patchList);
            } else if(obj.type == "audio-buffer") {
                // This gives us the actual array that contains the data
                var data = atob(obj.data);
                var channel0 = analyser.getChannel(0);
                var channel1 = analyser.getChannel(1);
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
                    midipatch.errorDialog(obj.title, obj.msg);
                }
                
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
    
    errorDialog(title, msg) {
        $("#errordialog" ).attr("title", title);
        $("#errordialog .errormessage" ).html(msg);
        $( function() {
        $( "#errordialog" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
            "Ok": function() {
            $( this ).dialog( "close" );
            },
        }
        });
        });
    }

    confirmOverwriteDialog(content) {
        $( function() {
        $( "#confirmoverwrite" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
            "Overwrite": function() {
                editor.setValue(content);
                $( this ).dialog( "close" );
            },
            Cancel: function() {
                $( this ).dialog( "close" );
            }
            }
        });
        });
    }

    confirmConcurrentMod() {
        $( function() {
        $( "#confirmconcurrentmod" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
            "I understand": function() {
                $( this ).dialog( "close" );
            }
            }
        });
        });
    }

    deleteFromLibraryDialog(name,author) {
        var mp = this;
        $( function() {
        $( "#deletefromlibrary" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
            "Delete": function() {
                mp.deleteFromLibrary(name, author);
                $( this ).dialog( "close" );
            },
            "Cancel": function() {
                $( this ).dialog( "close" );
            }
            }
        });
        });
    }

    exportToLibraryDialog(name, author, desc) {
        if(name)
            $("#sessionname").val(name);
        if(author)
            $("#sessionauthor").val(author);
        if(desc)
            $("#sessiondescription").val(desc);

        var mp = this;
        
        $( function() {
        $( "#exporttolibrary" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
            "Export": function() {
                var name = $("#sessionname" ).val();
                var desc = $("#sessiondescription").val();
                var author = $("#sessionauthor").val();
                var expCode = $("#exportcode" ).is(':checked');
                var expParameters = $("#exportparameters").is(':checked');
                var expLayout = $("#exportlayout").is(':checked');
                mp.exportToLibrary(name, author, desc, expCode, expParameters, expLayout);
                $( this ).dialog( "close" );
            },
            "Cancel": function() {
                $( this ).dialog( "close" );
            }
            }
        });
        });
    }

    importFromLibraryDialog(name, author, revision) {
        var code;
        var parameters;
        var layout;
        for(var i = 0; i < patchList.length; ++i) {
            if(patchList[i].name == name && patchList[i].author == author && patchList[i].revision == revision) {
                code = patchList[i].code;
                parameters = patchList[i].parameters;
                layout = patchList[i].layout;
                break;
            }
        }
        if(!code || code.length == 0)
            $("#importcode" ).attr("disabled", true);
        else
            $("#importcode" ).removeAttr("disabled");

        if(!parameters || parameters.length == 0)
            $("#importparameters" ).attr("disabled", true);
        else
            $("#importparameters" ).removeAttr("disabled");

        if(!layout || layout.length == 0)
            $("#importlayout" ).attr("disabled", true);
        else
            $("#importlayout" ).removeAttr("disabled");

        var mp = this;

        $( function() {
            $( "#importfromlibrary" ).dialog({
                resizable: false,
                height: "auto",
                width: 400,
                modal: true,
                buttons: {
                    "Import": function() {
                        var impCode = $("#importcode" ).is(':checked');
                        var impParameters = $("#importparameters").is(':checked');
                        var impLayout = $("#importlayout").is(':checked');

                        mp.importFromLibrary(name, author, revision, impCode, impParameters, impLayout);
                        $( this ).dialog( "close" );
                    },
                    "Cancel": function() {
                        $( this ).dialog( "close" );
                    }
                }
            });
        });
    }

    askContentDifferDialog(content) {
        var mp = this;
        $( function() {
        $( "#askcontentdiffer" ).dialog({
            resizable: false,
            height: "auto",
            width: 500,
            modal: true,
            buttons: {
            "Keep editor content?": function() {
                editor.setValue(content);
                mp.storePatch();
                $( this ).dialog( "close" );
            },
            "Overwrite editor content?": function() {
                mp.loadPatch();
                $( this ).dialog( "close" );
            }
            }
        });
        });
    }

    setEditorMode(mode) {
        editor.setMode(mode);
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
                mp.askContentDifferDialog(editorContent);
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
var toolbar = new MPToolbar(midipatch);
var piano = new MPPiano(midipatch);
var analyser = new MPAnalyser();
var editor = new MPEditor(midipatch);
var log = new MPLog(editor);
var library = new MPLibrary(midipatch);
var rack = new MPRack(midipatch);

function scrollToBottom(id) {
    var element = document.getElementById(id);
    element.scrollTop = element.scrollHeight - element.clientHeight;
}

function makeLayout() {
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


$(document).ready(function() {
    makeLayout();

    piano.prepare();
    midipatch.connect();
    toolbar.bind();
    piano.bind();

    
    //alt, shift, ctrl, etc must be lowercase 
    keyboardJS.bind('alt+r', function(e) {
        midipatch.restart();
        e.preventDefault();
    });

    keyboardJS.bind('alt+s', function(e) {
        midipatch.storePatch();
        e.preventDefault();
    });

    var cpStr = localStorage.getItem("savedControlParameters"); 
    if(cpStr) {
      var cp = JSON.parse(cpStr);
      Object.keys(cp).forEach(function(key) {
        $(("#" + key).replace(".","\\.")).val(cp[key]);
        $(("#" + key).replace(".","\\.")).trigger("change");
      });
    }
});
