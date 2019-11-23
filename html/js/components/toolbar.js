class MPToolbar {
        constructor(midipatch) {
            this.midipatch = midipatch;
        }
        
        makeHTML() {
            return `<div id="toolbar">
                        <button class="ui-button ui-corner-all ui-widget" id="restart">Restart</button>
                        <button class="ui-button ui-corner-all ui-widget" id="storepatch">Upload</button>
                        <button class="ui-button ui-corner-all ui-widget" id="loadpatch">Download</button>
                        <button class="ui-button ui-corner-all ui-widget" id="exporttolibrarybtn">Export to library</button>
                        <button class="ui-button ui-corner-all ui-widget" id="resetlayout">Reset Layout</button>
                        <section id="editorselectsection">
                            <label class="ui-corner-all ui-widget" id="editorselectlbl" for="editorselect">Mode:</label>
                            <select id="editorselect" class="ui-corner-all ui-widget">
                                <option value="sublime">Sublime</option>
                                <option value="vim">Vim</option>
                                <option value="emacs">Emacs</option>
                            </select>
                        </section>
                        <input type="checkbox" id="restartonsave" value="Restart on save" checked/><label for="restartonsave" id="restartonsavelbl">Restart on store</label>
                        <section id="connectionstatus">Status:<span>Disconnected</span>
                    </div>`
        }
        
        bind() {
            $("#restart").click(function() {
                midipatch.restart();
            });

            $("#storepatch").click(function() {
                midipatch.storePatch();
            });

            $("#loadpatch").click(function() {
                midipatch.loadPatchAsk();
            });

            $("#exporttolibrarybtn").click(function() {
                dialogs.exportToLibraryDialog(); 
            });

            $("#resetlayout").click(function() {
                midipatch.resetLayout();
            });

            $("#clearallnotes").click(function() {
                midipatch.clearAllNotes();
            });

            $("#editorselect").change(function() {
                midipatch.setEditorMode($("#editorselect").val());
            });
        }
}
