var codeMirror;
var textarea = document.getElementById("editor");
var myLayout;
var audioCtx = new(window.AudioContext || window.webkitAudioContext)();
var socket;
var lastControlListData;
var arrColors = new Array("#FF4848", "#FF68DD", "#FF62B0", "#FE67EB", "#E469FE", "#D568FD", "#9669FE", "#FF7575", "#FF79E1", "#FF73B9", "#FE67EB", "#E77AFE", "#D97BFD", "#A27AFE", "#FF8A8A", "#FF86E3", "#FF86C2", "#FE8BF0", "#EA8DFE", "#DD88FD", "#AD8BFE", "#FF9797", "#FF97E8", "#FF97CB", "#FE98F1", "#ED9EFE", "#E29BFD", "#B89AFE", "#FFA8A8", "#FFACEC", "#FFA8D3", "#FEA9F3", "#EFA9FE", "#E7A9FE", "#C4ABFE", "#FFBBBB", "#FFACEC", "#FFBBDD", "#FFBBF7", "#F2BCFE", "#EDBEFE", "#D0BCFE", "#FFCECE", "#FFC8F2", "#FFC8E3", "#FFCAF9", "#F5CAFF", "#F0CBFE", "#DDCEFF", "#FFDFDF", "#FFDFF8", "#FFDFEF", "#FFDBFB", "#F9D9FF", "#F4DCFE", "#E6DBFF", "#FFECEC", "#FFEEFB", "#FFECF5", "#FFEEFD", "#FDF2FF", "#FAECFF", "#F1ECFF", "#FFF2F2", "#FFFEFB", "#FFF9FC", "#FFF9FE", "#FFFDFF", "#FDF9FF", "#FBF9FF", "#800080", "#872187", "#9A03FE", "#892EE4", "#3923D6", "#2966B8", "#23819C", "#BF00BF", "#BC2EBC", "#A827FE", "#9B4EE9", "#6755E3", "#2F74D0", "#2897B7", "#DB00DB", "#D54FD5", "#B445FE", "#A55FEB", "#8678E9", "#4985D6", "#2FAACE", "#F900F9", "#DD75DD", "#BD5CFE", "#AE70ED", "#9588EC", "#6094DB", "#44B4D5", "#FF4AFF", "#DD75DD", "#C269FE", "#AE70ED", "#A095EE", "#7BA7E1", "#57BCD9", "#FF86FF", "#E697E6", "#CD85FE", "#C79BF2", "#B0A7F1", "#8EB4E6", "#7BCAE1", "#FFA4FF", "#EAA6EA", "#D698FE", "#CEA8F4", "#BCB4F3", "#A9C5EB", "#8CD1E6", "#FFBBFF", "#EEBBEE", "#DFB0FF", "#DBBFF7", "#CBC5F5", "#BAD0EF", "#A5DBEB", "#FFCEFF", "#F0C4F0", "#E8C6FF", "#E1CAF9", "#D7D1F8", "#CEDEF4", "#B8E2EF", "#FFDFFF", "#F4D2F4", "#EFD7FF", "#EDDFFB", "#E3E0FA", "#E0EAF8", "#C9EAF3", "#FFECFF", "#F4D2F4", "#F9EEFF", "#F5EEFD", "#EFEDFC", "#EAF1FB", "#DBF0F7", "#FFF9FF", "#FDF9FD", "#FEFDFF", "#FEFDFF", "#F7F5FE", "#F8FBFE", "#EAF7FB", "#5757FF", "#62A9FF", "#62D0FF", "#06DCFB", "#01FCEF", "#03EBA6", "#01F33E", "#6A6AFF", "#75B4FF", "#75D6FF", "#24E0FB", "#1FFEF3", "#03F3AB", "#0AFE47", "#7979FF", "#86BCFF", "#8ADCFF", "#3DE4FC", "#5FFEF7", "#33FDC0", "#4BFE78", "#8C8CFF", "#99C7FF", "#99E0FF", "#63E9FC", "#74FEF8", "#62FDCE", "#72FE95", "#9999FF", "#99C7FF", "#A8E4FF", "#75ECFD", "#92FEF9", "#7DFDD7", "#8BFEA8", "#AAAAFF", "#A8CFFF", "#BBEBFF", "#8CEFFD", "#A5FEFA", "#8FFEDD", "#A3FEBA", "#BBBBFF", "#BBDAFF", "#CEF0FF", "#ACF3FD", "#B5FFFC", "#A5FEE3", "#B5FFC8", "#CACAFF", "#D0E6FF", "#D9F3FF", "#C0F7FE", "#CEFFFD", "#BEFEEB", "#CAFFD8", "#E1E1FF", "#DBEBFF", "#ECFAFF", "#C0F7FE", "#E1FFFE", "#BDFFEA", "#EAFFEF", "#EEEEFF", "#ECF4FF", "#F9FDFF", "#E6FCFF", "#F2FFFE", "#CFFEF0", "#EAFFEF", "#F9F9FF", "#F9FCFF", "#FDFEFF", "#F9FEFF", "#FDFFFF", "#F7FFFD", "#F9FFFB", "#1FCB4A", "#59955C", "#48FB0D", "#2DC800", "#59DF00", "#9D9D00", "#B6BA18", "#27DE55", "#6CA870", "#79FC4E", "#32DF00", "#61F200", "#C8C800", "#CDD11B", "#4AE371", "#80B584", "#89FC63", "#36F200", "#66FF00", "#DFDF00", "#DFE32D", "#7CEB98", "#93BF96", "#99FD77", "#52FF20", "#95FF4F", "#FFFFAA", "#EDEF85", "#93EEAA", "#A6CAA9", "#AAFD8E", "#6FFF44", "#ABFF73", "#FFFF84", "#EEF093", "#A4F0B7", "#B4D1B6", "#BAFEA3", "#8FFF6F", "#C0FF97", "#FFFF99", "#F2F4B3", "#BDF4CB", "#C9DECB", "#CAFEB8", "#A5FF8A", "#D1FFB3", "#FFFFB5", "#F5F7C4", "#D6F8DE", "#DBEADC", "#DDFED1", "#B3FF99", "#DFFFCA", "#FFFFC8", "#F7F9D0", "#E3FBE9", "#E9F1EA", "#EAFEE2", "#D2FFC4", "#E8FFD9", "#FFFFD7", "#FAFBDF", "#E3FBE9", "#F3F8F4", "#F1FEED", "#E7FFDF", "#F2FFEA", "#FFFFE3", "#FCFCE9", "#FAFEFB", "#FBFDFB", "#FDFFFD", "#F5FFF2", "#FAFFF7", "#FFFFFD", "#FDFDF0", "#BABA21", "#C8B400", "#DFA800", "#DB9900", "#FFB428", "#FF9331", "#FF800D", "#E0E04E", "#D9C400", "#F9BB00", "#EAA400", "#FFBF48", "#FFA04A", "#FF9C42", "#E6E671", "#E6CE00", "#FFCB2F", "#FFB60B", "#FFC65B", "#FFAB60", "#FFAC62", "#EAEA8A", "#F7DE00", "#FFD34F", "#FFBE28", "#FFCE73", "#FFBB7D", "#FFBD82", "#EEEEA2", "#FFE920", "#FFDD75", "#FFC848", "#FFD586", "#FFC48E", "#FFC895", "#F1F1B1", "#FFF06A", "#FFE699", "#FFD062", "#FFDEA2", "#FFCFA4", "#FFCEA2", "#F4F4BF", "#FFF284", "#FFECB0", "#FFE099", "#FFE6B5", "#FFD9B7", "#FFD7B3", "#F7F7CE", "#FFF7B7", "#FFF1C6", "#FFEAB7", "#FFEAC4", "#FFE1C6", "#FFE2C8", "#F9F9DD", "#FFF9CE", "#FFF5D7", "#FFF2D2", "#FFF2D9", "#FFEBD9",

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

function refreshLog() {
    $.get("/cgi-bin/log.cgi", function(data) {
        if (data.length == 0)
            return;
        $("#log").html(data);
        scrollToBottom("log");
    });
}

function loadPatch() {
    $.get("/cgi-bin/loadPatch.cgi", function(data) {
        if (data.length == 0)
            return;
        codeMirror.setValue(data);
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


function saveToFile() {
    var file = $("#fileselect input").val();
    var code = codeMirror.getValue();
    $.post('/cgi-bin/savefile.cgi', {
            code: code,
            file: file
        },
        function(data, status, jqXHR) {
            $.notify("Saved to file: " + file, "success");
            if ($("#restartonsave").prop('checked')) {
                restart();
            }
        }
    )
}

function loadFromFile() {
    var file = $("#fileselect input").val();
    $.get('/cgi-bin/loadfile.cgi', {
            file: file
        },
        function(data, status, jqXHR) {
            codeMirror.setValue(data);
            $.notify("Loaded from file: " + file, "success");
        }
    )
}

function setControl(name, value) {
    var obj = {
        "type": "set-control",
        "name": name,
        "value": parseFloat(value)
    };
    socket.send(JSON.stringify(obj));
}

function noteOn(key, velocity) {
    var obj = {
        "type": "note-on",
        "note": key,
        "velocity": velocity
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

function str2ab(str) {
    var buf = new ArrayBuffer(str.length); // 2 bytes for each char
    var bufView = new Int16Array(buf);
    for (var i = 0, strLen = str.length; i < strLen / 2; i++) {
        bufView[i] = 0x0000FFFF & (str.charCodeAt(i * 2) | str.charCodeAt(i * 2 + 1) << 8);
    }
    return buf;
}

function connect() {
    socket = new ReconnectingWebSocket("ws://" + window.location.hostname + ":8080");
    socket.onopen = function(e) {};

    socket.onmessage = function(event) {
        var obj = jQuery.parseJSON(event.data);
        if (obj.type == "update-control") {
            $("#" + obj.data.parent + "_" + obj.data.child).val(obj.data.value).trigger('change');
        } else if (obj.type == "display") {} else if (obj.type == "control-list") {
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

    socket.onclose = function(event) {};
    socket.onerror = function(error) {};
}

function makeLayout() {
    var config = {
        content: [{
            type: 'column',
            content: [{
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'editorComponent',
                }, {
                    type: 'component',
                    componentName: 'rackComponent',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'pianoComponent',
                }]
            }, {
                type: 'row',
                content: [{
                    type: 'component',
                    componentName: 'logComponent',
                }]
            }]
        }]
    };
    var savedState = localStorage.getItem('savedLayoutState');

    if (savedState !== null) {
        myLayout = new GoldenLayout(JSON.parse(savedState));
    } else {
        myLayout = new GoldenLayout(config);
    }

    myLayout.registerComponent('editorComponent', function(container, componentState) {
        var html = $('#editorpane').wrap('<p/>').parent().html();
        $('#editorpane').unwrap();
        $("#editorpane").remove();
        container.getElement().html(html);
    });

    myLayout.registerComponent('rackComponent', function(container, componentState) {
        var html = $('#rack').wrap('<p/>').parent().html();
        $('#rack').unwrap();
        $("#rack").remove();
        container.getElement().html(html);
    });

    myLayout.registerComponent('pianoComponent', function(container, componentState) {
        var html = $('#piano').wrap('<p/>').parent().html();
        $('#piano').unwrap();
        $("#piano").remove();
        container.getElement().html(html);
    });

    myLayout.registerComponent('logComponent', function(container, componentState) {
        var html = $('#log').wrap('<p/>').parent().html();
        $('#log').unwrap();
        $("#log").remove();
        container.getElement().html(html);
    });
    myLayout.on('stateChanged', function() {
        var state = JSON.stringify(myLayout.toConfig());
        localStorage.setItem('savedLayoutState', state);
    });

    myLayout.init();
    $(window).resize(function() {
        myLayout.updateSize($(window).width(), $(window).height() - 10);
    });

$(document).ready(function() {
    if (screen.width > 640)
        makeLayout();
    window.setInterval(refreshLog, 1000);

    $("#restart").click(function() {
        restart();
    });

    $("#loadpatch").click(function() {
        loadPatch();
    });

    $("#storepatch").click(function() {
        storePatch();
    });

    //alt, shift, ctrl, etc must be lowercase 
    keyboardJS.bind('alt+r', function(e) {
        restart();
        e.preventDefault();
    });

    keyboardJS.bind('alt+s', function(e) {
        savePatch();
        e.preventDefault();
    });

    connect();
    var keys = Array.from(document.querySelectorAll('.key'));
    keys.forEach(key => key.addEventListener('mousedown', function(e) {
        e.preventDefault();
        e.target.classList.add('playing');
        noteOn($(e.target).data("key"), 127);
    }));
    keys.forEach(key => key.addEventListener('mouseup', function(e) {
        //  if (e.propertyName !== 'transform') return;
        e.target.classList.remove('playing');

        var obj = {
            "type": "note-off",
            "note": $(e.target).data("key")
        };
        socket.send(JSON.stringify(obj));
    }));
    keys.forEach(key => key.addEventListener('mouseleave', function(e) {
        //  if (e.propertyName !== 'transform') return;
        e.target.classList.remove('playing');

        var obj = {
            "type": "note-off",
            "note": $(e.target).data("key")
        };
        socket.send(JSON.stringify(obj));
    }));


    codeMirror = CodeMirror.fromTextArea(document.getElementById("editor"), {
        lineNumbers: true,
        mode: "lua",
        theme: "midnight"
    });

    loadPatch();

});
