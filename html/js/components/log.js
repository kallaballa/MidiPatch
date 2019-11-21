function appendLeadingZeroes(n){  
  if(n <= 9){
    return "0" + n;
  }
  return n
}

class MPLog {
    constructor(editor) {
        this.editor = editor;
        this.logLevels = [ "Fatal", "Error", "Warning", "Info", "Debug", "Global" ];
    }
    
    makeHTML() {
        return '<div id="log"></div>'
    }
    
    update(obj) {
        var current_datetime = new Date();
        var formatted_date = current_datetime.getFullYear() + "-" + appendLeadingZeroes(current_datetime.getMonth() + 1) + "-" + appendLeadingZeroes(current_datetime.getDate()) + " " + appendLeadingZeroes(current_datetime.getHours()) + ":" + appendLeadingZeroes(current_datetime.getMinutes()) + ":" + appendLeadingZeroes(current_datetime.getSeconds());
        var color = "#ffffff";
        if(obj.severity < 2)
        color = "#ff0000";
        else if(obj.severity < 3)
        color = "#ffff00";
        $("#log").append("<pre style=\"color: " + color +";\">" + formatted_date  + " ["+ this.logLevels[obj.severity] + "] " + obj.title + "</pre><br/>");
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
                    editor.focusLine(lineNum);
                });
                $("#log").append("<br/>");
                $("#log").parent().attr("id","logparent");
                var element = document.getElementById("logparent");
                element.scrollTop = element.scrollHeight - element.clientHeight;
            }
        }
    }
}
