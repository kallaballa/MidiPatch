var version;
var textarea = document.getElementById("editor");
var myLayout;
var socket;
var lastControlListData;
var patchFile;

var patchList;
var arrColors = new Array("#FF4848", "#FF68DD", "#FF62B0", "#FE67EB", "#E469FE", "#D568FD", "#9669FE", "#FF7575", "#FF79E1", "#FF73B9", "#FE67EB", "#E77AFE", "#D97BFD", "#A27AFE", "#FF8A8A", "#FF86E3", "#FF86C2", "#FE8BF0", "#EA8DFE", "#DD88FD", "#AD8BFE", "#FF9797", "#FF97E8", "#FF97CB", "#FE98F1", "#ED9EFE", "#E29BFD", "#B89AFE", "#FFA8A8", "#FFACEC", "#FFA8D3", "#FEA9F3", "#EFA9FE", "#E7A9FE", "#C4ABFE", "#FFBBBB", "#FFACEC", "#FFBBDD", "#FFBBF7", "#F2BCFE", "#EDBEFE", "#D0BCFE", "#FFCECE", "#FFC8F2", "#FFC8E3", "#FFCAF9", "#F5CAFF", "#F0CBFE", "#DDCEFF", "#FFDFDF", "#FFDFF8", "#FFDFEF", "#FFDBFB", "#F9D9FF", "#F4DCFE", "#E6DBFF", "#FFECEC", "#FFEEFB", "#FFECF5", "#FFEEFD", "#FDF2FF", "#FAECFF", "#F1ECFF", "#FFF2F2", "#FFFEFB", "#FFF9FC", "#FFF9FE", "#FFFDFF", "#FDF9FF", "#FBF9FF", "#800080", "#872187", "#9A03FE", "#892EE4", "#3923D6", "#2966B8", "#23819C", "#BF00BF", "#BC2EBC", "#A827FE", "#9B4EE9", "#6755E3", "#2F74D0", "#2897B7", "#DB00DB", "#D54FD5", "#B445FE", "#A55FEB", "#8678E9", "#4985D6", "#2FAACE", "#F900F9", "#DD75DD", "#BD5CFE", "#AE70ED", "#9588EC", "#6094DB", "#44B4D5", "#FF4AFF", "#DD75DD", "#C269FE", "#AE70ED", "#A095EE", "#7BA7E1", "#57BCD9", "#FF86FF", "#E697E6", "#CD85FE", "#C79BF2", "#B0A7F1", "#8EB4E6", "#7BCAE1", "#FFA4FF", "#EAA6EA", "#D698FE", "#CEA8F4", "#BCB4F3", "#A9C5EB", "#8CD1E6", "#FFBBFF", "#EEBBEE", "#DFB0FF", "#DBBFF7", "#CBC5F5", "#BAD0EF", "#A5DBEB", "#FFCEFF", "#F0C4F0", "#E8C6FF", "#E1CAF9", "#D7D1F8", "#CEDEF4", "#B8E2EF", "#FFDFFF", "#F4D2F4", "#EFD7FF", "#EDDFFB", "#E3E0FA", "#E0EAF8", "#C9EAF3", "#FFECFF", "#F4D2F4", "#F9EEFF", "#F5EEFD", "#EFEDFC", "#EAF1FB", "#DBF0F7", "#FFF9FF", "#FDF9FD", "#FEFDFF", "#FEFDFF", "#F7F5FE", "#F8FBFE", "#EAF7FB", "#5757FF", "#62A9FF", "#62D0FF", "#06DCFB", "#01FCEF", "#03EBA6", "#01F33E", "#6A6AFF", "#75B4FF", "#75D6FF", "#24E0FB", "#1FFEF3", "#03F3AB", "#0AFE47", "#7979FF", "#86BCFF", "#8ADCFF", "#3DE4FC", "#5FFEF7", "#33FDC0", "#4BFE78", "#8C8CFF", "#99C7FF", "#99E0FF", "#63E9FC", "#74FEF8", "#62FDCE", "#72FE95", "#9999FF", "#99C7FF", "#A8E4FF", "#75ECFD", "#92FEF9", "#7DFDD7", "#8BFEA8", "#AAAAFF", "#A8CFFF", "#BBEBFF", "#8CEFFD", "#A5FEFA", "#8FFEDD", "#A3FEBA", "#BBBBFF", "#BBDAFF", "#CEF0FF", "#ACF3FD", "#B5FFFC", "#A5FEE3", "#B5FFC8", "#CACAFF", "#D0E6FF", "#D9F3FF", "#C0F7FE", "#CEFFFD", "#BEFEEB", "#CAFFD8", "#E1E1FF", "#DBEBFF", "#ECFAFF", "#C0F7FE", "#E1FFFE", "#BDFFEA", "#EAFFEF", "#EEEEFF", "#ECF4FF", "#F9FDFF", "#E6FCFF", "#F2FFFE", "#CFFEF0", "#EAFFEF", "#F9F9FF", "#F9FCFF", "#FDFEFF", "#F9FEFF", "#FDFFFF", "#F7FFFD", "#F9FFFB", "#1FCB4A", "#59955C", "#48FB0D", "#2DC800", "#59DF00", "#9D9D00", "#B6BA18", "#27DE55", "#6CA870", "#79FC4E", "#32DF00", "#61F200", "#C8C800", "#CDD11B", "#4AE371", "#80B584", "#89FC63", "#36F200", "#66FF00", "#DFDF00", "#DFE32D", "#7CEB98", "#93BF96", "#99FD77", "#52FF20", "#95FF4F", "#FFFFAA", "#EDEF85", "#93EEAA", "#A6CAA9", "#AAFD8E", "#6FFF44", "#ABFF73", "#FFFF84", "#EEF093", "#A4F0B7", "#B4D1B6", "#BAFEA3", "#8FFF6F", "#C0FF97", "#FFFF99", "#F2F4B3", "#BDF4CB", "#C9DECB", "#CAFEB8", "#A5FF8A", "#D1FFB3", "#FFFFB5", "#F5F7C4", "#D6F8DE", "#DBEADC", "#DDFED1", "#B3FF99", "#DFFFCA", "#FFFFC8", "#F7F9D0", "#E3FBE9", "#E9F1EA", "#EAFEE2", "#D2FFC4", "#E8FFD9", "#FFFFD7", "#FAFBDF", "#E3FBE9", "#F3F8F4", "#F1FEED", "#E7FFDF", "#F2FFEA", "#FFFFE3", "#FCFCE9", "#FAFEFB", "#FBFDFB", "#FDFFFD", "#F5FFF2", "#FAFFF7", "#FFFFFD", "#FDFDF0", "#BABA21", "#C8B400", "#DFA800", "#DB9900", "#FFB428", "#FF9331", "#FF800D", "#E0E04E", "#D9C400", "#F9BB00", "#EAA400", "#FFBF48", "#FFA04A", "#FF9C42", "#E6E671", "#E6CE00", "#FFCB2F", "#FFB60B", "#FFC65B", "#FFAB60", "#FFAC62", "#EAEA8A", "#F7DE00", "#FFD34F", "#FFBE28", "#FFCE73", "#FFBB7D", "#FFBD82", "#EEEEA2", "#FFE920", "#FFDD75", "#FFC848", "#FFD586", "#FFC48E", "#FFC895", "#F1F1B1", "#FFF06A", "#FFE699", "#FFD062", "#FFDEA2", "#FFCFA4", "#FFCEA2", "#F4F4BF", "#FFF284", "#FFECB0", "#FFE099", "#FFE6B5", "#FFD9B7", "#FFD7B3", "#F7F7CE", "#FFF7B7", "#FFF1C6", "#FFEAB7", "#FFEAC4", "#FFE1C6", "#FFE2C8", "#F9F9DD", "#FFF9CE", "#FFF5D7", "#FFF2D2", "#FFF2D9", "#FFEBD9", "#FFE6D0", "#FBFBE8", "#FFFBDF", "#FFFAEA", "#FFF9EA", "#FFF7E6", "#FFF4EA", "#FFF1E6", "#FEFEFA", "#FFFEF7", "#FFFDF7", "#FFFDF9", "#FFFDF9", "#FFFEFD", "#FFF9F4", "#D1D17A", "#C0A545", "#C27E3A", "#C47557", "#B05F3C", "#C17753", "#B96F6F", "#D7D78A", "#CEB86C", "#C98A4B", "#CB876D", "#C06A45", "#C98767", "#C48484", "#DBDB97", "#D6C485", "#D19C67", "#D29680", "#C87C5B", "#D0977B", "#C88E8E", "#E1E1A8", "#DECF9C", "#DAAF85", "#DAA794", "#CF8D72", "#DAAC96", "#D1A0A0", "#E9E9BE", "#E3D6AA", "#DDB791", "#DFB4A4", "#D69E87", "#E0BBA9", "#D7ACAC", "#EEEECE", "#EADFBF", "#E4C6A7", "#E6C5B9", "#DEB19E", "#E8CCBF", "#DDB9B9", "#E9E9C0", "#EDE4C9", "#E9D0B6", "#EBD0C7", "#E4C0B1", "#ECD5CA", "#E6CCCC", "#EEEECE", "#EFE7CF", "#EEDCC8", "#F0DCD5", "#EACDC1", "#F0DDD5", "#ECD9D9", "#F1F1D6", "#F5EFE0", "#F2E4D5", "#F5E7E2", "#F0DDD5", "#F5E8E2", "#F3E7E7", "#F5F5E2", "#F9F5EC", "#F9F3EC", "#F9EFEC", "#F5E8E2", "#FAF2EF", "#F8F1F1", "#FDFDF9", "#FDFCF9", "#FCF9F5", "#FDFAF9", "#FDFAF9", "#FCF7F5", "#FDFBFB", "#F70000", "#B9264F", "#990099", "#74138C", "#0000CE", "#1F88A7", "#4A9586", "#FF2626", "#D73E68", "#B300B3", "#8D18AB", "#5B5BFF", "#25A0C5", "#5EAE9E", "#FF5353", "#DD597D", "#CA00CA", "#A41CC6", "#7373FF", "#29AFD6", "#74BAAC", "#FF7373", "#E37795", "#D900D9", "#BA21E0", "#8282FF", "#4FBDDD", "#8DC7BB", "#FF8E8E", "#E994AB", "#FF2DFF", "#CB59E8", "#9191FF", "#67C7E2", "#A5D3CA", "#FFA4A4", "#EDA9BC", "#F206FF", "#CB59E8", "#A8A8FF", "#8ED6EA", "#C0E0DA", "#FFB5B5", "#F0B9C8", "#FF7DFF", "#D881ED", "#B7B7FF", "#A6DEEE", "#CFE7E2", "#FFC8C8", "#F4CAD6", "#FFA8FF", "#EFCDF8", "#C6C6FF", "#C0E7F3", "#DCEDEA", "#FFEAEA", "#F8DAE2", "#FFC4FF", "#EFCDF8", "#DBDBFF", "#D8F0F8", "#E7F3F1", "#FFEAEA", "#FAE7EC", "#FFE3FF", "#F8E9FC", "#EEEEFF", "#EFF9FC", "#F2F9F8", "#FFFDFD", "#FEFAFB", "#FFFDFF", "#FFFFFF", "#FDFDFF", "#FAFDFE", "#F7FBFA");

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
                var cpStr = localStorage.getItem("savedControlParameters");
                var oldControlParams = JSON.parse(cpStr);
                $("#rack").html("");
                var rackDiv = "<button class=\"ui-button ui-corner-all ui-widget\" id=\"resettodefaults\">Reset to defaults</button><div id=\"rackdiv\">";
                for (var i = 0; i < obj.data.length; ++i) {
                    rackDiv += "<div class=\"control-row\" style=\"background-color: " + arrColors[Math.round((arrColors.length - 1) * Math.random())] + ";\" id=\"" + obj.data[i].name + "\"><label class=\"moduleLabel\">" + obj.data[i].name + "</label>"
                    for (var j = 0; j < obj.data[i].controls.length; ++j) {
                        var value;
                        if(oldControlParams)
                        value = oldControlParams[obj.data[i].name + "." + obj.data[i].controls[j].name];
                        else
                        value = obj.data[i].controls[j].value;
    
                        if (obj.data[i].controls[j].name.charAt(0) != '_') {
                            rackDiv += "<div class=\"control-cell\"><label style=\"white-space: nowrap;\" data-name=\"" + obj.data[i].controls[j].name + "\">" + obj.data[i].controls[j].displayName + "</label><br/><input id=\"" + obj.data[i].name + "." + obj.data[i].controls[j].name + "\" class=\"knob\" data-width=\"50\" data-fgColor=\"#000000\" data-height=\"50\" value=\"" + value + "\"></div>";
                        }
                    }
                    rackDiv += "<div style=\"width: 100%;\"></div></div><br/>";
                }
                rackDiv += "</div>";
                $("#rack").append(rackDiv);

                var knobs = document.getElementsByClassName('knob');
                $(".knob").on("input", function(e) {
                var par = $(this).parent().parent();
                if(!isNaN(parseInt($(this).val(), 10))) {
                    midipatch.sendControlChange(par.parent().find("label").html() + "." + par.find("label").data("name"), $(this).val());
                    localStorage.setItem("savedControlParameters",JSON.stringify(getControlParameters()));
                }
                });

                for (var i = 0; i < obj.data.length; ++i) {
                    for (var j = 0; j < obj.data[i].controls.length; ++j) {
                        if (obj.data[i].controls[j].name.charAt(0) != '_') {
                        $("#" + obj.data[i].name + "\\." + obj.data[i].controls[j].name).knob({
                            'min': obj.data[i].controls[j].min,
                            'max': obj.data[i].controls[j].max,
                            'step': 0.001,
                            'readOnly': false,
                            'stopper': true,
                            change: function(value) {
                                var par = this.$.parent().parent()
                                midipatch.sendControlChange(par.parent().find("label").html() + "." + par.find("label").data("name"), value);
                                localStorage.setItem("savedControlParameters",JSON.stringify(getControlParameters()));
                            }
                        });
                        }
                    }
                }
                $("#resettodefaults").click(function() {
                    localStorage.removeItem("savedControlParameters");
                    midipatch.restart();
                });
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
        container.getElement().html('<section id="rack"></section>');
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
    //toolbar.prepare();
    piano.prepare();
    //analyser.prepare();


    midipatch.connect();
    
    toolbar.bind();
    piano.bind();
   // analyser.bind();

    
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
