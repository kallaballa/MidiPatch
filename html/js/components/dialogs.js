class MPDialogs {
    constructor(midipatch) {
        this.midipatch = midipatch;
    }
    
    makeHTML() {
        return `
            <div id="errordialog" title="">
                <p><span class="ui-icon ui-icon-alert" style="float:left; margin:12px 12px 20px 0;"></span><pre class="errormessage"></pre></p>
            </div>
            <div id="confirmoverwrite" title="Replace editor content?">
                <p><span class="ui-icon ui-icon-alert" style="float:left; margin:12px 12px 20px 0;"></span>Do you really want to replace the editors content?</p>
            </div>
            <div id="askcontentdiffer" title="Server patch and local patch differ">
                <p><span class="ui-icon ui-icon-alert" style="float:left; margin:12px 12px 20px 0;"></span>The patch stored in the server and your local patch stored in the browser diff.</p>
            </div>
            <div id="confirmconcurrentmod" title="More than one user connected">
                <p><span class="ui-icon ui-icon-alert" style="float:left; margin:12px 12px 20px 0;"></span>Concurrent modification of MidiPatch not supported yet.</p>
            </div>
            <div id="exporttolibrary" title="Export session to library">
                <label class="ui-widget" for="sessionname">Name</label><br/><input id="sessionname" type="text"></input><br/>
                <label class="ui-widget" for="sessionauthor">Author</label><br/><input id="sessionauthor" type="text"></input><br/>
                <label class="ui-widget" for="sessiondescription">Description</label><br/>
                <textarea class="ui-widget" id="sessiondescription" rows="3" cols="35"></textarea><br/>
                <input class="ui-widget" type="checkbox" id="exportcode" checked/><label for="exportcode">Export code</label><br/>
                <input class="ui-widget"  type="checkbox" id="exportparameters"/><label for="exportparameters">Export parameters</label><br/>
                <input class="ui-widget" type="checkbox" id="exportlayout"/><label for="exportlayout">Export layout</label>
            </div>
            <div id="importfromlibrary" title="Import session from library">
                <input class="ui-widget" type="checkbox" id="importcode"/><label for="importcode">Import code</label><br/>
                <input class="ui-widget"  type="checkbox" id="importparameters"/><label for="importparameters">Import parameters</label><br/>
                <input class="ui-widget" type="checkbox" id="importlayout"/><label for="importlayout">Import layout</label>
            </div>
            <div id="deletefromlibrary" title="Delete session from library">
                <p><span class="ui-icon ui-icon-alert" style="float:left; margin:12px 12px 20px 0;"></span>Do you really want to delete all revisions of this session?</p>
            </div>
            <div id="controlparametersmatch" title="Control parameters match exactly.">
                <p><span class="ui-icon ui-icon-alert" style="float:left; margin:12px 12px 20px 0;"></span>The patch you are importing has exactly the same control parameters like your current patch.<br/>What do you want to do?</p>
            </div>
    `;
    }

    errorDialog(title, msg) {
        $("#errordialog" ).attr("title", title);
        $("#errordialog .errormessage" ).html(msg);
        $( function() {
            $( "#errordialog" ).dialog({
            resizable: false,
            height: "auto",
            width: 620,
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
        var mp = this.midipatch;
        $( function() {
        $( "#confirmoverwrite" ).dialog({
            resizable: false,
            height: "auto",
            width: 400,
            modal: true,
            buttons: {
            "Overwrite": function() {
                mp.setEditorValue(content);
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
        var mp = this.midipatch;
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

        var mp = this.midipatch;
        
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

        var mp = this.midipatch;

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
        var mp = this.midipatch;
        $( function() {
        $( "#askcontentdiffer" ).dialog({
            resizable: false,
            height: "auto",
            width: 500,
            modal: true,
            buttons: {
            "Keep editor content?": function() {
                mp.setEditorValue(content);
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
    
    askControlParametersMatch(keepCallBack, loadCallback) {
        var mp = this.midipatch;
        $( function() {
        $( "#controlparametersmatch" ).dialog({
            resizable: false,
            height: "auto",
            width: 500,
            modal: true,
            buttons: {
            "Keep current parameters values": function() {
                keepCallBack();
                $( this ).dialog( "close" );
            },
            "Load defaults of imported patch": function() {
                loadCallback()
                $( this ).dialog( "close" );
            }
            }
        });
        });
    }
}
