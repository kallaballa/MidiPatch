class MPLibrary {
    constructor(midipatch) {
        this.midipatch = midipatch;
    }
    
    makeHTML() {
        return '<div><table id="librarytable"><thead><tr><th>Name</th><th>Author</th><th>Revision</th><th>Description</th><th>Date</th><th></th></tr></thead><tbody></tbody></table></div>';
    }
    
    updatePatchList(patchlist) {
        var mp = this.midipatch;
        var tbody = $("#librarytable tbody");
        var lastName = "";
        tbody.find("tr").remove();
        
        for(var i = patchList.length - 1; i >= 0; --i) {
            if(patchList[i].name != lastName) {
            var revStr = '<select class="revisionselect">';
            for(var j = 0; j < patchList[i].revision; ++j) {
                revStr += "<option value=\"" + (patchList[i].revision - j) + "\">" + (patchList[i].revision - j) + "</option>"
            }
            revStr += '</select>'

            tbody.append("<tr><td class=\"libname\">" + patchList[i].name + "</td><td class=\"libauthor\">" + patchList[i].author + "</td><td class=\"librevision\">" + revStr + "</td><td class=\"libdescription\">" + patchList[i].description + "</td><td>" + new Date(patchList[i].date * 1000).toLocaleString() + "</td><td><button class=\"ui-button ui-corner-all ui-widget  importfromlib\">Import</button><button class=\"ui-button ui-corner-all ui-widget exporttolib\">Export</button><button class=\"ui-button ui-corner-all ui-widget deletefromlib\">Delete</button></td></tr>");
            }
            lastName = patchList[i].name;
        }

        $('#librarytable .revisionselect').each(function() {
            $(this).change(function() {
            var name = $(this).parent().parent().find(".libname").html();
            var author = $(this).parent().parent().find(".libauthor").html();
            var revision = $(this).val();
            mp.importFromLibraryDialog(name,author,revision);
            })
        });

        $('#librarytable .exporttolib').each(function() {
            $(this).click(function() {
            var name = $(this).parent().parent().find(".libname").html();
            var author = $(this).parent().parent().find(".libauthor").html();
            var desc = $(this).parent().parent().find(".libdescription").html();
            mp.exportToLibraryDialog(name,author,desc);
            })
        });
        
        $('#librarytable .importfromlib').each(function() { 
            $(this).click(function() {
            var name = $(this).parent().parent().find(".libname").html();
            var author = $(this).parent().parent().find(".libauthor").html();
            var revision = $(this).parent().parent().find(".revisionselect").val();
            mp.importFromLibraryDialog(name,author,revision);
            });
        });

        $('#librarytable .deletefromlib').each(function() {
            $(this).click(function() {
            var name = $(this).parent().parent().find(".libname").html();
            var author = $(this).parent().parent().find(".libauthor").html();
            mp.deleteFromLibraryDialog(name,author);
            });
        });


        $('#librarytable').DataTable();        
    }
}
