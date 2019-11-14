var stop = false;
var frameCount = 0;
var $results = $("#results");
var fps, fpsInterval, startTime, now, then, elapsed;

function startDrawing(fps) {
    fpsInterval = 1000 / fps;
    then = Date.now();
    startTime = then;
    draw();
}

var version;
var codeMirror;
var textarea = document.getElementById("editor");
var myLayout;
var socket;
var lastControlListData;
var patchFile;
var audioCtx;
var myScriptProcessor;
var rawAudio;
var analyser;
var source;
var scopeCtx;
var spectCtx;
var patchList;
var logLevels = [ "Fatal", "Error", "Warning", "Info", "Debug", "Global" ];
var arrColors = new Array("#FF4848", "#FF68DD", "#FF62B0", "#FE67EB", "#E469FE", "#D568FD", "#9669FE", "#FF7575", "#FF79E1", "#FF73B9", "#FE67EB", "#E77AFE", "#D97BFD", "#A27AFE", "#FF8A8A", "#FF86E3", "#FF86C2", "#FE8BF0", "#EA8DFE", "#DD88FD", "#AD8BFE", "#FF9797", "#FF97E8", "#FF97CB", "#FE98F1", "#ED9EFE", "#E29BFD", "#B89AFE", "#FFA8A8", "#FFACEC", "#FFA8D3", "#FEA9F3", "#EFA9FE", "#E7A9FE", "#C4ABFE", "#FFBBBB", "#FFACEC", "#FFBBDD", "#FFBBF7", "#F2BCFE", "#EDBEFE", "#D0BCFE", "#FFCECE", "#FFC8F2", "#FFC8E3", "#FFCAF9", "#F5CAFF", "#F0CBFE", "#DDCEFF", "#FFDFDF", "#FFDFF8", "#FFDFEF", "#FFDBFB", "#F9D9FF", "#F4DCFE", "#E6DBFF", "#FFECEC", "#FFEEFB", "#FFECF5", "#FFEEFD", "#FDF2FF", "#FAECFF", "#F1ECFF", "#FFF2F2", "#FFFEFB", "#FFF9FC", "#FFF9FE", "#FFFDFF", "#FDF9FF", "#FBF9FF", "#800080", "#872187", "#9A03FE", "#892EE4", "#3923D6", "#2966B8", "#23819C", "#BF00BF", "#BC2EBC", "#A827FE", "#9B4EE9", "#6755E3", "#2F74D0", "#2897B7", "#DB00DB", "#D54FD5", "#B445FE", "#A55FEB", "#8678E9", "#4985D6", "#2FAACE", "#F900F9", "#DD75DD", "#BD5CFE", "#AE70ED", "#9588EC", "#6094DB", "#44B4D5", "#FF4AFF", "#DD75DD", "#C269FE", "#AE70ED", "#A095EE", "#7BA7E1", "#57BCD9", "#FF86FF", "#E697E6", "#CD85FE", "#C79BF2", "#B0A7F1", "#8EB4E6", "#7BCAE1", "#FFA4FF", "#EAA6EA", "#D698FE", "#CEA8F4", "#BCB4F3", "#A9C5EB", "#8CD1E6", "#FFBBFF", "#EEBBEE", "#DFB0FF", "#DBBFF7", "#CBC5F5", "#BAD0EF", "#A5DBEB", "#FFCEFF", "#F0C4F0", "#E8C6FF", "#E1CAF9", "#D7D1F8", "#CEDEF4", "#B8E2EF", "#FFDFFF", "#F4D2F4", "#EFD7FF", "#EDDFFB", "#E3E0FA", "#E0EAF8", "#C9EAF3", "#FFECFF", "#F4D2F4", "#F9EEFF", "#F5EEFD", "#EFEDFC", "#EAF1FB", "#DBF0F7", "#FFF9FF", "#FDF9FD", "#FEFDFF", "#FEFDFF", "#F7F5FE", "#F8FBFE", "#EAF7FB", "#5757FF", "#62A9FF", "#62D0FF", "#06DCFB", "#01FCEF", "#03EBA6", "#01F33E", "#6A6AFF", "#75B4FF", "#75D6FF", "#24E0FB", "#1FFEF3", "#03F3AB", "#0AFE47", "#7979FF", "#86BCFF", "#8ADCFF", "#3DE4FC", "#5FFEF7", "#33FDC0", "#4BFE78", "#8C8CFF", "#99C7FF", "#99E0FF", "#63E9FC", "#74FEF8", "#62FDCE", "#72FE95", "#9999FF", "#99C7FF", "#A8E4FF", "#75ECFD", "#92FEF9", "#7DFDD7", "#8BFEA8", "#AAAAFF", "#A8CFFF", "#BBEBFF", "#8CEFFD", "#A5FEFA", "#8FFEDD", "#A3FEBA", "#BBBBFF", "#BBDAFF", "#CEF0FF", "#ACF3FD", "#B5FFFC", "#A5FEE3", "#B5FFC8", "#CACAFF", "#D0E6FF", "#D9F3FF", "#C0F7FE", "#CEFFFD", "#BEFEEB", "#CAFFD8", "#E1E1FF", "#DBEBFF", "#ECFAFF", "#C0F7FE", "#E1FFFE", "#BDFFEA", "#EAFFEF", "#EEEEFF", "#ECF4FF", "#F9FDFF", "#E6FCFF", "#F2FFFE", "#CFFEF0", "#EAFFEF", "#F9F9FF", "#F9FCFF", "#FDFEFF", "#F9FEFF", "#FDFFFF", "#F7FFFD", "#F9FFFB", "#1FCB4A", "#59955C", "#48FB0D", "#2DC800", "#59DF00", "#9D9D00", "#B6BA18", "#27DE55", "#6CA870", "#79FC4E", "#32DF00", "#61F200", "#C8C800", "#CDD11B", "#4AE371", "#80B584", "#89FC63", "#36F200", "#66FF00", "#DFDF00", "#DFE32D", "#7CEB98", "#93BF96", "#99FD77", "#52FF20", "#95FF4F", "#FFFFAA", "#EDEF85", "#93EEAA", "#A6CAA9", "#AAFD8E", "#6FFF44", "#ABFF73", "#FFFF84", "#EEF093", "#A4F0B7", "#B4D1B6", "#BAFEA3", "#8FFF6F", "#C0FF97", "#FFFF99", "#F2F4B3", "#BDF4CB", "#C9DECB", "#CAFEB8", "#A5FF8A", "#D1FFB3", "#FFFFB5", "#F5F7C4", "#D6F8DE", "#DBEADC", "#DDFED1", "#B3FF99", "#DFFFCA", "#FFFFC8", "#F7F9D0", "#E3FBE9", "#E9F1EA", "#EAFEE2", "#D2FFC4", "#E8FFD9", "#FFFFD7", "#FAFBDF", "#E3FBE9", "#F3F8F4", "#F1FEED", "#E7FFDF", "#F2FFEA", "#FFFFE3", "#FCFCE9", "#FAFEFB", "#FBFDFB", "#FDFFFD", "#F5FFF2", "#FAFFF7", "#FFFFFD", "#FDFDF0", "#BABA21", "#C8B400", "#DFA800", "#DB9900", "#FFB428", "#FF9331", "#FF800D", "#E0E04E", "#D9C400", "#F9BB00", "#EAA400", "#FFBF48", "#FFA04A", "#FF9C42", "#E6E671", "#E6CE00", "#FFCB2F", "#FFB60B", "#FFC65B", "#FFAB60", "#FFAC62", "#EAEA8A", "#F7DE00", "#FFD34F", "#FFBE28", "#FFCE73", "#FFBB7D", "#FFBD82", "#EEEEA2", "#FFE920", "#FFDD75", "#FFC848", "#FFD586", "#FFC48E", "#FFC895", "#F1F1B1", "#FFF06A", "#FFE699", "#FFD062", "#FFDEA2", "#FFCFA4", "#FFCEA2", "#F4F4BF", "#FFF284", "#FFECB0", "#FFE099", "#FFE6B5", "#FFD9B7", "#FFD7B3", "#F7F7CE", "#FFF7B7", "#FFF1C6", "#FFEAB7", "#FFEAC4", "#FFE1C6", "#FFE2C8", "#F9F9DD", "#FFF9CE", "#FFF5D7", "#FFF2D2", "#FFF2D9", "#FFEBD9", "#FFE6D0", "#FBFBE8", "#FFFBDF", "#FFFAEA", "#FFF9EA", "#FFF7E6", "#FFF4EA", "#FFF1E6", "#FEFEFA", "#FFFEF7", "#FFFDF7", "#FFFDF9", "#FFFDF9", "#FFFEFD", "#FFF9F4", "#D1D17A", "#C0A545", "#C27E3A", "#C47557", "#B05F3C", "#C17753", "#B96F6F", "#D7D78A", "#CEB86C", "#C98A4B", "#CB876D", "#C06A45", "#C98767", "#C48484", "#DBDB97", "#D6C485", "#D19C67", "#D29680", "#C87C5B", "#D0977B", "#C88E8E", "#E1E1A8", "#DECF9C", "#DAAF85", "#DAA794", "#CF8D72", "#DAAC96", "#D1A0A0", "#E9E9BE", "#E3D6AA", "#DDB791", "#DFB4A4", "#D69E87", "#E0BBA9", "#D7ACAC", "#EEEECE", "#EADFBF", "#E4C6A7", "#E6C5B9", "#DEB19E", "#E8CCBF", "#DDB9B9", "#E9E9C0", "#EDE4C9", "#E9D0B6", "#EBD0C7", "#E4C0B1", "#ECD5CA", "#E6CCCC", "#EEEECE", "#EFE7CF", "#EEDCC8", "#F0DCD5", "#EACDC1", "#F0DDD5", "#ECD9D9", "#F1F1D6", "#F5EFE0", "#F2E4D5", "#F5E7E2", "#F0DDD5", "#F5E8E2", "#F3E7E7", "#F5F5E2", "#F9F5EC", "#F9F3EC", "#F9EFEC", "#F5E8E2", "#FAF2EF", "#F8F1F1", "#FDFDF9", "#FDFCF9", "#FCF9F5", "#FDFAF9", "#FDFAF9", "#FCF7F5", "#FDFBFB", "#F70000", "#B9264F", "#990099", "#74138C", "#0000CE", "#1F88A7", "#4A9586", "#FF2626", "#D73E68", "#B300B3", "#8D18AB", "#5B5BFF", "#25A0C5", "#5EAE9E", "#FF5353", "#DD597D", "#CA00CA", "#A41CC6", "#7373FF", "#29AFD6", "#74BAAC", "#FF7373", "#E37795", "#D900D9", "#BA21E0", "#8282FF", "#4FBDDD", "#8DC7BB", "#FF8E8E", "#E994AB", "#FF2DFF", "#CB59E8", "#9191FF", "#67C7E2", "#A5D3CA", "#FFA4A4", "#EDA9BC", "#F206FF", "#CB59E8", "#A8A8FF", "#8ED6EA", "#C0E0DA", "#FFB5B5", "#F0B9C8", "#FF7DFF", "#D881ED", "#B7B7FF", "#A6DEEE", "#CFE7E2", "#FFC8C8", "#F4CAD6", "#FFA8FF", "#EFCDF8", "#C6C6FF", "#C0E7F3", "#DCEDEA", "#FFEAEA", "#F8DAE2", "#FFC4FF", "#EFCDF8", "#DBDBFF", "#D8F0F8", "#E7F3F1", "#FFEAEA", "#FAE7EC", "#FFE3FF", "#F8E9FC", "#EEEEFF", "#EFF9FC", "#F2F9F8", "#FFFDFD", "#FEFAFB", "#FFFDFF", "#FFFFFF", "#FDFDFF", "#FAFDFE", "#F7FBFA");

function confirmOverwriteDialog(content) {
          $( function() {
          $( "#confirmoverwrite" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
              "Overwrite": function() {
                codeMirror.setValue(content);
                $( this ).dialog( "close" );
              },
              Cancel: function() {
                $( this ).dialog( "close" );
              }
            }
          });
          });
}

function confirmConcurrentMod() {
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

function deleteFromLibraryDialog(name) {
          $( function() {
          $( "#deletefromlibrary" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
              "Delete": function() {
                deleteFromLibrary(name);
                $( this ).dialog( "close" );
              },
              "Cancel": function() {
                $( this ).dialog( "close" );
              }
            }
          });
          });
}

function exportToLibraryDialog(name, desc) {
          if(name)
            $("#sessionname").val(name);
          if(desc)
            $("#sessiondescription").val(desc);
          $( function() {
          $( "#exporttolibrary" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
              "Save": function() {
                var name = $("#sessionname" ).val();
                var desc = $("#sessiondescription").val();
                var expCode = $("#exportcode" ).is(':checked');
                var expParameters = $("#exportparameters").is(':checked');
                var expLayout = $("#exportlayout").is(':checked');
                exportToLibrary(name,desc, expCode, expParameters, expLayout);
                $( this ).dialog( "close" );
              },
              "Cancel": function() {
                $( this ).dialog( "close" );
              }
            }
          });
          });
}

function importFromLibraryDialog(name, revision) {
         var code;
         var parameters;
         var layout;
              for(var i = 0; i < patchList.length; ++i) {
                if(patchList[i].name == name && patchList[i].revision == revision) {
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

          $( function() {
          $( "#importfromlibrary" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
              "Save": function() {
                var impCode = $("#importcode" ).is(':checked');
                var impParameters = $("#importparameters").is(':checked');
                var impLayout = $("#importlayout").is(':checked');

                importFromLibrary(name, revision, impCode, impParameters, impLayout);
                $( this ).dialog( "close" );
              },
              "Cancel": function() {
                $( this ).dialog( "close" );
              }
            }
          });
          });
}

function askContentDifferDialog(content) {
          $( function() {
          $( "#askcontentdiffer" ).dialog({
            resizable: false,
            height: "auto",
            width: 500,
            modal: true,
            buttons: {
              "Keep editor content?": function() {
                codeMirror.setValue(content);
                storePatch();
                $( this ).dialog( "close" );
              },
              "Overwrite editor content?": function() {
                loadPatch();
                $( this ).dialog( "close" );
              }
            }
          });
          });
}

function resetLayout() {
  localStorage.removeItem('savedLayoutState');
  window.location.reload();
}

function scrollToBottom(id) {
    var element = document.getElementById(id);
    element.scrollTop = element.scrollHeight - element.clientHeight;
}

function restart() {
    $.notify("Restarting", "success")
    var obj = {
        "type": "restart"
    };
    socket.send(JSON.stringify(obj));
}

function loadPatchAsk() {
    $.get("/cgi-bin/loadPatch.cgi", function(data) {
        if (data.length == 0)
            return;
        $.notify("Loading", "success")
        codeMirror.setValueAsk(data);
    });
}


function loadPatch() {
    $.get("/cgi-bin/loadPatch.cgi", function(data) {
        if (data.length == 0)
            return;
        $.notify("Loading", "success")
        codeMirror.setValue(data);
    });
} 

function compareEditorAndServerPatch(editorContent) {
    $.get("/cgi-bin/loadPatch.cgi", function(data) {
        if(editorContent.localeCompare(data) != 0) {
          askContentDifferDialog(editorContent);
        } else {
          codeMirror.setValue(data);
        }
    });
}

function storePatch() {
    var code = codeMirror.getValue();
    $.post('/cgi-bin/storePatch.cgi', {
            code: code
        },
        function(data, status, jqXHR) {
            $.notify("Patch saved", "success");
            if ($("#restartonsave").prop('checked')) {
                restart();
            }
        }
    )
}


function getControlParameters() {
  var cp = {};
  $("#rackdiv input").each(function() {
    cp[$(this).attr("id")] = $(this).val();
  })
  return cp;
}

function setControl(name, value) {
    var obj = {
        "type": "set-control",
        "name": name,
        "value": parseFloat(value)
    };
    socket.send(JSON.stringify(obj));
}

function exportToLibrary(name, description, expCode, expParameters, expLayout) {
    var code = "";
    var parameters = "";
    var layout = "";
    alert(expCode);
    if(expCode)
      code = codeMirror.getValue();
    if(expParameters)
      parameters = JSON.stringify(getControlParameters());
    if(expLayout)
      layout = JSON.stringify(myLayout.toConfig());
    var obj = {
        "type": "update-patch",
        "name": name,
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

function deleteFromLibrary(name) {
    var obj = {
        "type": "delete-patch",
        "name": name,
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

function importFromLibrary(name, revision, impCode, impParameters, impLayout) {
              for(var i = 0; i < patchList.length; ++i) {
                if(patchList[i].name == name && patchList[i].revision == revision) {
                  if(impCode) {
                    codeMirror.setValue(patchList[i].code);
                    storePatch();
                  }

                  if(impParameters) {
                    var parObj = JSON.parse(patchList[i].parameters);
                    Object.keys(parObj).forEach(function(key) {
                      $(("#" + key).replace(".", "_")).val(parObj[key]);
                       setControl(key, parObj[key]);
                    });
                  }

                  if(impLayout) {
                    localStorage.setItem("savedLayoutState",  patchList[i].layout);
                    window.location.reload();
                  }
                }
              }
}

function noteOn(key, velocity) {
    var obj = {
        "type": "note-on",
        "note": key,
        "velocity": velocity
    };
    socket.send(JSON.stringify(obj));
}

function noteOff(key, velocity) {
    var obj = {
        "type": "note-off",
        "note": key
    };
    socket.send(JSON.stringify(obj));
}

function clearAllNotes() {
    $(".playing").each(function( index ) {
      $(this).removeClass('playing');
    });
    var obj = {
        "type": "clear-all-notes"
    };
    socket.send(JSON.stringify(obj));
}

function removeTransition(e) {
    if (e.propertyName !== 'transform') return;
    e.target.classList.remove('playing');

    var obj = {
        "type": "note-off",
        "note": $(e.target).data("key")
    };
    socket.send(JSON.stringify(obj));

}

function appendLeadingZeroes(n){  
  if(n <= 9){
    return "0" + n;
  }
  return n
}

function str2ab(str) {
    var buf = new ArrayBuffer(str.length); // 2 bytes for each char
    var bufView = new Int16Array(buf);
    for (var i = 0, strLen = str.length; i < strLen / 2; i++) {
        bufView[i] = 0x0000FFFF & (str.charCodeAt(i * 2) | str.charCodeAt(i * 2 + 1) << 8);
    }
    return buf;
}

function connect() {
    $("#connectionstatus span").html("Connecting");
    socket = new ReconnectingWebSocket("ws://" + window.location.hostname + ":8080");

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
          if(audioCtx)
            return;          
          patchFile = obj.data.patchFile;     
          version = obj.data.version;
          var bufferFrames = obj.data.bufferFrames;
          var channels = obj.data.channels;
          var sampleRate = obj.data.sampleRate;

          audioCtx = new AudioContext({
            sampleRate: sampleRate
          });
          if(audioCtx.sampleRate != sampleRate) {
            alert("Sample rate mismatch!");
          }
          myScriptProcessor = audioCtx.createScriptProcessor(bufferFrames, channels, channels);
          rawAudio = audioCtx.createBuffer(channels, bufferFrames, audioCtx.sampleRate);
          analyser = audioCtx.createAnalyser();
          source = audioCtx.createBufferSource();

          analyser.fftSize = bufferFrames * channels;
          source.buffer = rawAudio;
          myScriptProcessor.onaudioprocess = function(audioProcessingEvent) {
            var inputBuffer = rawAudio;
            var outputBuffer = audioProcessingEvent.outputBuffer;

            for (var channel = 0; channel < outputBuffer.numberOfChannels; channel++) {
              var inputData = inputBuffer.getChannelData(channel);
              var outputData = outputBuffer.getChannelData(channel);
              for (var sample = 0; sample < inputBuffer.length; sample++) {
                outputData[sample] = inputData[sample];
              }
            }
          };
          source.connect(myScriptProcessor);
          myScriptProcessor.connect(analyser);
          //analyser.connect(audioCtx.destination);
          source.start();
      
          scopeCtx = document.getElementById('scope').getContext('2d');
          spectCtx = document.getElementById('spectrum').getContext('2d');
          startDrawing(30);
        } else if(obj.type == "user-joined") {
          confirmConcurrentMod();
        } else if(obj.type == "patch-list") {
          patchList = obj.list;
          var tbody = $("#librarytable tbody");
          var lastName = "";
          tbody.find("tr").remove();
          
          for(var i = patchList.length - 1; i >= 0; --i) {
            if(patchList[i].name != lastName) {
              var revStr = '<select class="revisionselect">';
              for(var j = 0; j < patchList[i].revision; ++j) {
                  revStr += "<option>" + (patchList[i].revision - j) + "</option>"
              }
              revStr += '</select>'

              tbody.append("<tr><td class=\"libname\">" + patchList[i].name + "</td><td class=\"librevision\">" + revStr + "</td><td class=\"libdescription\">" + patchList[i].description + "</td><td>" + new Date(patchList[i].date * 1000).toLocaleString() + "</td><td><button class=\"ui-button ui-corner-all ui-widget  importfromlib\">Import</button><button class=\"ui-button ui-corner-all ui-widget exporttolib\">Export</button><button class=\"ui-button ui-corner-all ui-widget deletefromlib\">Delete</button></td></tr>");
            }
            lastName = patchList[i].name;
          }

          $('#librarytable .revisionselect').each(function() {
            $(this).change(function() {
              var name = $(this).parent().parent().find(".libname").html();
              var revision = $(this).val();
              importFromLibraryDialog(name,revision);
             })
          });

          $('#librarytable .exporttolib').each(function() {
            $(this).click(function() {
              var name = $(this).parent().parent().find(".libname").html();
              var desc = $(this).parent().parent().find(".libdescription").html();
              exportToLibraryDialog(name,desc);
             })
          });
          
          $('#librarytable .importfromlib').each(function() { 
            $(this).click(function() {
              var name = $(this).parent().parent().find(".libname").html();
              var revision = $(this).parent().parent().find(".librevision").html();
              importFromLibraryDialog(name,revision);
            });
          });

          $('#librarytable .deletefromlib').each(function() {
            $(this).click(function() {
              var name = $(this).parent().parent().find(".libname").html();
              deleteFromLibraryDialog(name);
            });
          });


          $('#librarytable').DataTable();
        } else if(obj.type == "audio-buffer") {
            // This gives us the actual array that contains the data
            var data = atob(obj.data);
            var channel0 = rawAudio.getChannelData(0);
            var channel1 = rawAudio.getChannelData(1);
             for (var i = 0; i < 512; i++) {
              channel0[i] = (data.charCodeAt(i * 2) / 127.0) - 1.0;
              channel1[i] = (data.charCodeAt(i * 2 + 1) / 127.0) -1.0;
             }
        } else if (obj.type == "update-control") {
            $(obj.data.parent + "_" + obj.data.child).val(obj.data.value);
        } else if (obj.type == "update-log") {
            var current_datetime = new Date();
            var formatted_date = current_datetime.getFullYear() + "-" + appendLeadingZeroes(current_datetime.getMonth() + 1) + "-" + appendLeadingZeroes(current_datetime.getDate()) + " " + appendLeadingZeroes(current_datetime.getHours()) + ":" + appendLeadingZeroes(current_datetime.getMinutes()) + ":" + appendLeadingZeroes(current_datetime.getSeconds());
            var color = "#ffffff";
            if(obj.severity < 2)
              color = "#ff0000";
            else if(obj.severity < 3)
              color = "#ffff00";

            $("#log").append("<pre style=\"color: " + color +";\">" + formatted_date  + " ["+ logLevels[obj.severity] + "] " + obj.title + "</pre><br/>");
            if(obj.msg.length > 0) {
            if(!obj.highlight) {
              $("#log").append("<pre style=\"color: " + color +";\">\t" + obj.msg + "</pre><br/>");
            } else {
              var tokens = obj.msg.split(":");
              var file = tokens[0];
              var lineNum = parseInt(tokens[1], 10) - 1;
              var rest = "";
              for(var i = 2; i < tokens.length; ++i) {
                rest += tokens[i];
                if(i < tokens.length - 1)
                  rest += ":";
              }
              $("#log").append("<pre style=\"color: " + color +";\">\t<span>" + file + ":" + (lineNum + 1) + ":" + rest + "</span></pre>").last().click(function() {
                codeMirror.focus();
                codeMirror.scrollIntoView({line: lineNum, ch:0});
                codeMirror.setCursor({line: lineNum, ch: 0})
                codeMirror.setSelection({line: lineNum, ch: 0}, {line: lineNum, ch: 200});
              });
              $("#log").append("<br/>");
            }
            }
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
            $("#rack").html("");
            var rackDiv = "<div id=\"rackdiv\">";
            for (var i = 0; i < obj.data.length; ++i) {
                rackDiv += "<div class=\"control-row\" style=\"background-color: " + arrColors[Math.round((arrColors.length - 1) * Math.random())] + ";\" id=\"" + obj.data[i].name + "\"><label class=\"moduleLabel\">" + obj.data[i].name + "</label>"
                for (var j = 0; j < obj.data[i].controls.length; ++j) {
                    if (obj.data[i].controls[j].name.charAt(0) != '_') {
                        rackDiv += "<div class=\"control-cell\"><label>" + obj.data[i].controls[j].name + "</label><br/><input id=\"" + obj.data[i].name + "_" + obj.data[i].controls[j].name + "\" class=\"knob\" data-width=\"50\" data-fgColor=\"#000000\" data-height=\"50\" value=\"" + obj.data[i].controls[j].value + "\"></div>";
                    }
                }
                rackDiv += "<div style=\"width: 100%;\"></div></div><br/>";
            }
            rackDiv += "</div>";
            $("#rack").append(rackDiv);

            var knobs = document.getElementsByClassName('knob');
            for (var i = 0; i < knobs.length; ++i) {
                $(knobs[i]).knob({
                    'min': 0.0,
                    'max': 1.0,
                    'step': 0.01,
                    'readOnly': false,
                    'stopper': true,
                    change: function(value) {
                        var par = this.$.parent().parent()
                        setControl(par.parent().find("label").html() + "." + par.find("label").html(), value);
                    }
                });
            }
        }
    };

    socket.onclose = function(event) {
      $("#connectionstatus span").html("Disconnected");
    };

    socket.onerror = function(error) {
      $("#connectionstatus span").html("Disconnected");
    };
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
        container.getElement().html('<div id="toolbar"><button class="ui-button ui-corner-all ui-widget" id="restart">Restart</button><button class="ui-button ui-corner-all ui-widget" id="loadpatch">Download</button><button class="ui-button ui-corner-all ui-widget" id="storepatch">Upload</button><button class="ui-button ui-corner-all ui-widget" id="exporttolibrarybtn">Export to library</button><button class="ui-button ui-corner-all ui-widget" id="resetlayout">Reset Layout</button><input type="checkbox" id="restartonsave" value="Restart on save" checked/><label for="restartonsave" id="restartonsavelbl">Restart on store</label><section id="connectionstatus">Status:<span>Disconnected</span></div></div>');
    });

    myLayout.registerComponent('Analyser', function(container, componentState) {
        container.getElement().html("<canvas id='scope' width=400 height=200></canvas><br/><canvas id='spectrum' width=400 height=200>");
    });

    myLayout.registerComponent('Library', function(container, componentState) {
        container.getElement().html('<div><table id="librarytable"><thead><tr><th>Name</th><th>Revision</th><th>Description</th><th>Date</th><th></th></tr></thead><tbody></tbody></table></div>');
    });

   myLayout.registerComponent('Editor', function(container, componentState) {
        container.getElement().html('<section id="editorselectsection"><label class="ui-corner-all ui-widget" id="editorselectlbl" for="editorselect">Mode:</label><select id="editorselect"><option value="sublime">Sublime</option><option value="vim">Vim</option><option value="emacs">Emacs</option></select></section><section id="editorpane"><textarea id="editor"></textarea></section>');

        window.setTimeout(function() {
        codeMirror = CodeMirror.fromTextArea(document.getElementById("editor"), {
          lineNumbers: true,
          styleSelectedText: true,
          keyMap: "sublime",
          styleActiveLine: {nonEmpty: true},
          matchBrackets: true,
          showCursorWhenSelecting: true,
          mode: "lua",
          theme: "midnight",
          inputStyle: "contenteditable"
        });

        codeMirror.setValueAsk = function(content) {
          confirmOverwriteDialog(content);
        } 

        codeMirror.on("change", function(cm, change) {
           localStorage.setItem("savedEditorContent", cm.getValue());
        });
        var content = localStorage.getItem("savedEditorContent");
        if(content) {
          compareEditorAndServerPatch(content);
        } else {
          loadPatch();
        }
        }, 100);
    });

    myLayout.registerComponent('Rack', function(container, componentState) {
        container.getElement().html('<section id="rack"></section>');
    });

    myLayout.registerComponent('Keyboard', function(container, componentState) {
        container.getElement().html('<section id="piano"> <section id="holdmodesection"> <button class="ui-button ui-corner-all ui-widget" id="clearallnotes"> Clear all notes</button><input type="checkbox" id="holdmode" value="Hold"/> <label for="holdmode" id="holdmodelbl"> Hold</label> </section><section id="spacer">spacer</section><div class="keys"> <div data-key="0" class="key border-right">C-1</div> <div data-key="1" class="key black">Cs</div> <div data-key="2" class="key border-right">D</div> <div data-key="3" class="key black">Ds</div> <div data-key="4" class="key border-right">E</div> <div data-key="5" class="key border-right">F</div> <div data-key="6" class="key black">Fs</div> <div data-key="7" class="key border-right">G</div> <div data-key="8" class="key black">Gs</div> <div data-key="9" class="key border-right">A</div> <div data-key="10" class="key black">As</div> <div data-key="11" class="key border-right">B</div> <div data-key="12" class="key border-right">C0</div> <div data-key="13" class="key black">Cs</div> <div data-key="14" class="key border-right">D</div> <div data-key="15" class="key black">Ds</div> <div data-key="16" class="key border-right">E</div> <div data-key="17" class="key border-right">F</div> <div data-key="18" class="key black">Fs</div> <div data-key="19" class="key border-right">G</div> <div data-key="20" class="key black">Gs</div> <div data-key="21" class="key border-right">A</div> <div data-key="22" class="key black">As</div> <div data-key="23" class="key border-right">B</div> <div data-key="24" class="key border-right">C1</div> <div data-key="25" class="key black">Cs</div> <div data-key="26" class="key border-right">D</div> <div data-key="27" class="key black">Ds</div> <div data-key="28" class="key border-right">E</div> <div data-key="29" class="key border-right">F</div> <div data-key="30" class="key black">Fs</div> <div data-key="31" class="key border-right">G</div> <div data-key="32" class="key black">Gs</div> <div data-key="33" class="key border-right">A</div> <div data-key="34" class="key black">As</div> <div data-key="35" class="key border-right">B</div> <div data-key="36" class="key border-right">C2</div> <div data-key="37" class="key black">Cs</div> <div data-key="38" class="key border-right">D</div> <div data-key="39" class="key black">Ds</div> <div data-key="40" class="key border-right">E</div> <div data-key="41" class="key border-right">F</div> <div data-key="42" class="key black">Fs</div> <div data-key="43" class="key border-right">G</div> <div data-key="44" class="key black">Gs</div> <div data-key="45" class="key border-right">A</div> <div data-key="46" class="key black">As</div> <div data-key="47" class="key border-right">B</div> <div data-key="48" class="key border-right">C3</div> <div data-key="49" class="key black">Cs</div> <div data-key="50" class="key border-right">D</div> <div data-key="51" class="key black">Ds</div> <div data-key="52" class="key border-right">E</div> <div data-key="53" class="key border-right">F</div> <div data-key="54" class="key black">Fs</div> <div data-key="55" class="key border-right">G</div> <div data-key="56" class="key black">Gs</div> <div data-key="57" class="key border-right">A</div> <div data-key="58" class="key black">As</div> <div data-key="59" class="key border-right">B</div> <div data-key="60" class="key border-right">C4</div> <div data-key="61" class="key black">Cs</div> <div data-key="62" class="key border-right">D</div> <div data-key="63" class="key black">Ds</div> <div data-key="64" class="key border-right">E</div> <div data-key="65" class="key border-right">F</div> <div data-key="66" class="key black">Fs</div> <div data-key="67" class="key border-right">G</div> <div data-key="68" class="key black">Gs</div> <div data-key="69" class="key border-right">A</div> <div data-key="70" class="key black">As</div> <div data-key="71" class="key border-right">B</div> <div data-key="72" class="key border-right">C5</div> <div data-key="73" class="key black">Cs</div> <div data-key="74" class="key border-right">D</div> <div data-key="75" class="key black">Ds</div> <div data-key="76" class="key border-right">E</div> <div data-key="77" class="key border-right">F</div> <div data-key="78" class="key black">Fs</div> <div data-key="79" class="key border-right">G</div> <div data-key="80" class="key black">Gs</div> <div data-key="81" class="key border-right">A</div> <div data-key="82" class="key black">As</div> <div data-key="83" class="key border-right">B</div> <div data-key="84" class="key border-right">C6</div> <div data-key="85" class="key black">Cs</div> <div data-key="86" class="key border-right">D</div> <div data-key="87" class="key black">Ds</div> <div data-key="88" class="key border-right">E</div> <div data-key="89" class="key border-right">F</div> <div data-key="90" class="key black">Fs</div> <div data-key="91" class="key border-right">G</div> <div data-key="92" class="key black">Gs</div> <div data-key="93" class="key border-right">A</div> <div data-key="94" class="key black">As</div> <div data-key="95" class="key border-right">B</div> <div data-key="96" class="key border-right">C7</div> <div data-key="97" class="key black">Cs</div> <div data-key="98" class="key border-right">D</div> <div data-key="99" class="key black">Ds</div> <div data-key="100" class="key border-right">E</div> <div data-key="101" class="key border-right">F</div> <div data-key="102" class="key black">Fs</div> <div data-key="103" class="key border-right">G</div> <div data-key="104" class="key black">Gs</div> <div data-key="105" class="key border-right">A</div> <div data-key="106" class="key black">As</div> <div data-key="107" class="key border-right">B</div> <div data-key="108" class="key border-right">C8</div> <div data-key="109" class="key black">Cs</div> <div data-key="110" class="key border-right">D</div> <div data-key="111" class="key black">Ds</div> <div data-key="112" class="key border-right">E</div> <div data-key="113" class="key border-right">F</div> <div data-key="114" class="key black">Fs</div> <div data-key="115" class="key border-right">G</div> <div data-key="116" class="key black">Gs</div> <div data-key="117" class="key border-right">A</div> <div data-key="118" class="key black">As</div> <div data-key="119" class="key border-right">B</div> <div data-key="120" class="key border-right">C9</div> <div data-key="121" class="key black">Cs</div> <div data-key="122" class="key border-right">D</div> <div data-key="123" class="key black">Ds</div> <div data-key="124" class="key border-right">E</div> <div data-key="125" class="key border-right">F</div> <div data-key="126" class="key black">Fs</div> <div data-key="127" class="key border-right">G</div> </div> </section>');
    });

    myLayout.registerComponent('Log', function(container, componentState) {
        container.getElement().html('<div id="log"></div>');
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

function draw() {
  requestAnimationFrame(draw);

  now = Date.now();
  elapsed = now - then;

  if (elapsed > fpsInterval) {
    then = now - (elapsed % fpsInterval);
    drawSpectrum(analyser, spectCtx);
    drawScope(analyser, scopeCtx);
  }
}

function drawSpectrum(analyser, ctx) {
  var width = ctx.canvas.width;
  var height = ctx.canvas.height;
  var freqData = new Uint8Array(analyser.frequencyBinCount);
  var scaling = height / 256;

  analyser.getByteFrequencyData(freqData);

  ctx.fillStyle = 'rgba(0, 20, 0, 0.3)';
  ctx.fillRect(0, 0, width, height);

  ctx.lineWidth = 2;
  ctx.strokeStyle = 'rgb(0, 200, 0)';
  ctx.beginPath();

  for (var x = 0; x < width; x++)
    ctx.lineTo(x, height - freqData[x] * scaling);

  ctx.stroke();
}

function drawScope(analyser, ctx) {
  var width = ctx.canvas.width;
  var height = ctx.canvas.height;
  var timeData = new Uint8Array(analyser.frequencyBinCount);
  var scaling = height / 256;
  var risingEdge = 0;
  var edgeThreshold = 5;

  analyser.getByteTimeDomainData(timeData);

  ctx.fillStyle = 'rgba(0, 20, 0, 0.3)';
  ctx.fillRect(0, 0, width, height);

  ctx.lineWidth = 2;
  ctx.strokeStyle = 'rgb(0, 200, 0)';
  ctx.beginPath();

  // No buffer overrun protection
  while (timeData[risingEdge++] - 128 > 0 && risingEdge <= width);
  if (risingEdge >= width) risingEdge = 0;

  while (timeData[risingEdge++] - 128 < edgeThreshold && risingEdge <= width);
  if (risingEdge >= width) risingEdge = 0;

  for (var x = risingEdge; x < timeData.length && x - risingEdge < width; x++)
    ctx.lineTo(x - risingEdge, height - timeData[x] * scaling);

  ctx.stroke();
}

$(document).ready(function() {
    makeLayout();

    //scroll C5 into view
    $(".key").get(72).scrollIntoView();

    $("#restart").click(function() {
        restart();
    });

    $("#storepatch").click(function() {
        storePatch();
    });

    $("#loadpatch").click(function() {
        loadPatchAsk();
    });

    $("#exporttolibrarybtn").click(function() {
       exportToLibraryDialog(); 
    });

    $("#resetlayout").click(function() {
        resetLayout();
    });

    $("#clearallnotes").click(function() {
        clearAllNotes();
    });

    $("#editorselect").change(function() {
      if($("#editorselect").val() == "vim") {
        codeMirror.setOption("vimMode", true);
//        $(".CodeMirror-cursors").css('visibility', 'visible');
      }
      codeMirror.setOption("keyMap", $("#editorselect").val());
    });

    connect();
    const keys = Array.from(document.querySelectorAll('.key'));
    keys.forEach(key => key.addEventListener('mousedown', function(e) {
        e.preventDefault();

      if ($("#holdmode").prop("checked") && e.target.classList.contains("playing")) {
        e.target.classList.remove('playing');
        noteOff($(e.target).data("key")); 
      } else {
        e.target.classList.add('playing');
        noteOn($(e.target).data("key"), 127);
      }
    }));
    keys.forEach(key => key.addEventListener('mouseup', function(e) {
      if (!$("#holdmode").prop("checked")) {
        e.preventDefault();
        e.target.classList.remove('playing');
        noteOff($(e.target).data("key"));
      }
    }));
    keys.forEach(key => key.addEventListener('mouseleave', function(e) {
       if (!$("#holdmode").prop("checked")) {
        e.preventDefault();
        e.target.classList.remove('playing');
        noteOff($(e.target).data("key"));
      }
    }));

    //alt, shift, ctrl, etc must be lowercase 
    keyboardJS.bind('alt+r', function(e) {
        restart();
        e.preventDefault();
    });

    keyboardJS.bind('alt+s', function(e) {
        storePatch();
        e.preventDefault();
    });


});
