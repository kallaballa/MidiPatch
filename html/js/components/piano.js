class MPPiano {
    constructor(midipatch) {
        this.midipatch = midipatch;
    }

    makeHTML() {
        return `
        <section id="holdmodesection">
            <button class="ui-button ui-corner-all ui-widget" id="clearallnotes"> Clear all notes</button><input type="checkbox" id="holdmode" value="Hold"/> <label for="holdmode" id="holdmodelbl"> Hold</label>
        </section><br/>
        <section id="piano"> 
            <div class="keys">
                <div data-key="0" class="key border-right">C-1</div>
                <div data-key="1" class="key black">Cs</div>
                <div data-key="2" class="key border-right">D</div>
                <div data-key="3" class="key black">Ds</div>
                <div data-key="4" class="key border-right">E</div>
                <div data-key="5" class="key border-right">F</div>
                <div data-key="6" class="key black">Fs</div>
                <div data-key="7" class="key border-right">G</div>
                <div data-key="8" class="key black">Gs</div>
                <div data-key="9" class="key border-right">A</div>
                <div data-key="10" class="key black">As</div>
                <div data-key="11" class="key border-right">B</div>
                <div data-key="12" class="key border-right">C0</div>
                <div data-key="13" class="key black">Cs</div>
                <div data-key="14" class="key border-right">D</div>
                <div data-key="15" class="key black">Ds</div>
                <div data-key="16" class="key border-right">E</div>
                <div data-key="17" class="key border-right">F</div>
                <div data-key="18" class="key black">Fs</div>
                <div data-key="19" class="key border-right">G</div>
                <div data-key="20" class="key black">Gs</div>
                <div data-key="21" class="key border-right">A</div>
                <div data-key="22" class="key black">As</div>
                <div data-key="23" class="key border-right">B</div>
                <div data-key="24" class="key border-right">C1</div>
                <div data-key="25" class="key black">Cs</div>
                <div data-key="26" class="key border-right">D</div>
                <div data-key="27" class="key black">Ds</div>
                <div data-key="28" class="key border-right">E</div>
                <div data-key="29" class="key border-right">F</div>
                <div data-key="30" class="key black">Fs</div>
                <div data-key="31" class="key border-right">G</div>
                <div data-key="32" class="key black">Gs</div>
                <div data-key="33" class="key border-right">A</div>
                <div data-key="34" class="key black">As</div>
                <div data-key="35" class="key border-right">B</div>
                <div data-key="36" class="key border-right">C2</div>
                <div data-key="37" class="key black">Cs</div>
                <div data-key="38" class="key border-right">D</div>
                <div data-key="39" class="key black">Ds</div>
                <div data-key="40" class="key border-right">E</div>
                <div data-key="41" class="key border-right">F</div>
                <div data-key="42" class="key black">Fs</div>
                <div data-key="43" class="key border-right">G</div>
                <div data-key="44" class="key black">Gs</div>
                <div data-key="45" class="key border-right">A</div>
                <div data-key="46" class="key black">As</div>
                <div data-key="47" class="key border-right">B</div>
                <div data-key="48" class="key border-right">C3</div>
                <div data-key="49" class="key black">Cs</div>
                <div data-key="50" class="key border-right">D</div>
                <div data-key="51" class="key black">Ds</div>
                <div data-key="52" class="key border-right">E</div>
                <div data-key="53" class="key border-right">F</div>
                <div data-key="54" class="key black">Fs</div>
                <div data-key="55" class="key border-right">G</div>
                <div data-key="56" class="key black">Gs</div>
                <div data-key="57" class="key border-right">A</div>
                <div data-key="58" class="key black">As</div>
                <div data-key="59" class="key border-right">B</div>
                <div data-key="60" class="key border-right">C4</div>
                <div data-key="61" class="key black">Cs</div>
                <div data-key="62" class="key border-right">D</div>
                <div data-key="63" class="key black">Ds</div>
                <div data-key="64" class="key border-right">E</div>
                <div data-key="65" class="key border-right">F</div>
                <div data-key="66" class="key black">Fs</div>
                <div data-key="67" class="key border-right">G</div>
                <div data-key="68" class="key black">Gs</div>
                <div data-key="69" class="key border-right">A</div>
                <div data-key="70" class="key black">As</div>
                <div data-key="71" class="key border-right">B</div>
                <div data-key="72" class="key border-right">C5</div>
                <div data-key="73" class="key black">Cs</div>
                <div data-key="74" class="key border-right">D</div>
                <div data-key="75" class="key black">Ds</div>
                <div data-key="76" class="key border-right">E</div>
                <div data-key="77" class="key border-right">F</div>
                <div data-key="78" class="key black">Fs</div>
                <div data-key="79" class="key border-right">G</div>
                <div data-key="80" class="key black">Gs</div>
                <div data-key="81" class="key border-right">A</div>
                <div data-key="82" class="key black">As</div>
                <div data-key="83" class="key border-right">B</div>
                <div data-key="84" class="key border-right">C6</div>
                <div data-key="85" class="key black">Cs</div>
                <div data-key="86" class="key border-right">D</div>
                <div data-key="87" class="key black">Ds</div>
                <div data-key="88" class="key border-right">E</div>
                <div data-key="89" class="key border-right">F</div>
                <div data-key="90" class="key black">Fs</div>
                <div data-key="91" class="key border-right">G</div>
                <div data-key="92" class="key black">Gs</div>
                <div data-key="93" class="key border-right">A</div>
                <div data-key="94" class="key black">As</div>
                <div data-key="95" class="key border-right">B</div>
                <div data-key="96" class="key border-right">C7</div>
                <div data-key="97" class="key black">Cs</div>
                <div data-key="98" class="key border-right">D</div>
                <div data-key="99" class="key black">Ds</div>
                <div data-key="100" class="key border-right">E</div>
                <div data-key="101" class="key border-right">F</div>
                <div data-key="102" class="key black">Fs</div>
                <div data-key="103" class="key border-right">G</div>
                <div data-key="104" class="key black">Gs</div>
                <div data-key="105" class="key border-right">A</div>
                <div data-key="106" class="key black">As</div>
                <div data-key="107" class="key border-right">B</div>
                <div data-key="108" class="key border-right">C8</div>
                <div data-key="109" class="key black">Cs</div>
                <div data-key="110" class="key border-right">D</div>
                <div data-key="111" class="key black">Ds</div>
                <div data-key="112" class="key border-right">E</div>
                <div data-key="113" class="key border-right">F</div>
                <div data-key="114" class="key black">Fs</div>
                <div data-key="115" class="key border-right">G</div>
                <div data-key="116" class="key black">Gs</div>
                <div data-key="117" class="key border-right">A</div>
                <div data-key="118" class="key black">As</div>
                <div data-key="119" class="key border-right">B</div>
                <div data-key="120" class="key border-right">C9</div>
                <div data-key="121" class="key black">Cs</div>
                <div data-key="122" class="key border-right">D</div>
                <div data-key="123" class="key black">Ds</div>
                <div data-key="124" class="key border-right">E</div>
                <div data-key="125" class="key border-right">F</div>
                <div data-key="126" class="key black">Fs</div>
                <div data-key="127" class="key border-right">G</div> 
            </div>
            </section>`;
    }

    prepare() {
        //scroll C5 into view
        $(".key").get(72).scrollIntoView();
    }
    
    bind() {
        var mp = this.midipatch;
        const keys = Array.from(document.querySelectorAll('.key'));
        keys.forEach(key => key.addEventListener('mousedown', function(e) {
            e.preventDefault();

        if ($("#holdmode").prop("checked") && e.target.classList.contains("playing")) {
            e.target.classList.remove('playing');
            mp.noteOff($(e.target).data("key")); 
        } else {
            e.target.classList.add('playing');
            mp.noteOn($(e.target).data("key"), 127);
        }
        }));
        keys.forEach(key => key.addEventListener('mouseup', function(e) {
        if (!$("#holdmode").prop("checked")) {
            e.preventDefault();
            e.target.classList.remove('playing');
            mp.noteOff($(e.target).data("key"));
        }
        }));
        keys.forEach(key => key.addEventListener('mouseleave', function(e) {
        if (!$("#holdmode").prop("checked")) {
            e.preventDefault();
            e.target.classList.remove('playing');
            mp.noteOff($(e.target).data("key"));
        }
        }));        
    }
    
    resetKeys() {
        $(".key").each(function( index ) {
           $(this).removeClass('playing');    
        });
    }
}
