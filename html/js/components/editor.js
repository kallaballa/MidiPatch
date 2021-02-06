class MPEditor {
    constructor(midipatch) {
        this.midipatch = midipatch
    }
    
    init() {
        this.codeMirror = CodeMirror.fromTextArea(document.getElementById("editor"), {
          lineNumbers: true,
          lineWrapping: "wrap",
          styleSelectedText: true,
          keyMap: "sublime",
          styleActiveLine: {nonEmpty: true},
          matchBrackets: true,
          showCursorWhenSelecting: true,
          mode: "lua",
          theme: "midnight",
          inputStyle: "contenteditable"
        });
        var mp = this.midipatch;
        this.codeMirror.setValueAsk = function(content) {
          dialogs.confirmOverwriteDialog(content);
        } 

        this.codeMirror.on("change", function(cm, change) {
           localStorage.setItem("savedEditorContent", cm.getValue());
        });
 
        var content = localStorage.getItem("savedEditorContent");
        if(content) {
          this.midipatch.compareEditorAndServerPatch(content);
        } else {
          this.midipatch.loadPatch();
        }
    }
    
    getValue() {
        return this.codeMirror.getValue();
    }
    
    setValue(v) {
        this.codeMirror.setValue(v);
    }
    
    setValueAsk(v) {
        this.codeMirror.setValueAsk(v);
    }
    
    setMode(mode) {
        if(mode == "vim") {
            this.codeMirror.setOption("vimMode", true);
        }
        this.codeMirror.setOption("keyMap", mode);  
    }
    
    makeHTML() {
        return '<section id="editorpane"><textarea id="editor"></textarea></section>';
    }
}
