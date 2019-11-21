class MPAnalyser {
    constructor() {

    }
    
    isInitialized() {
        return this.audioCtx !== undefined;
    }
    
    init(sampleRate, bufferFrames, channels) {
        this.audioCtx = new AudioContext({
            sampleRate: sampleRate
        });
        if(this.audioCtx.sampleRate != sampleRate) {
            alert("Sample rate mismatch!");
        }
        this.myScriptProcessor = this.audioCtx.createScriptProcessor(bufferFrames, channels, channels);
        this.rawAudio = this.audioCtx.createBuffer(channels, bufferFrames, this.audioCtx.sampleRate);
        this.analyser = this.audioCtx.createAnalyser();
        this.source = this.audioCtx.createBufferSource();

        this.analyser.fftSize = bufferFrames * channels;
        this.source.buffer = this.rawAudio;
        var rawAudio = this.rawAudio;
        
        this.myScriptProcessor.onaudioprocess = function(audioProcessingEvent) {
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
        this.source.connect(this.myScriptProcessor);
        this.myScriptProcessor.connect(this.analyser);
        //analyser.connect(audioCtx.destination);
        this.source.start();

        this.scopeCtx = document.getElementById('scope').getContext('2d');
        this.spectCtx = document.getElementById('spectrum').getContext('2d');
        this.startDrawing(30);
    }
    
    makeHTML() {
        return "<canvas id='scope' width=400 height=200></canvas><br/><canvas id='spectrum' width=400 height=200>";
    }

    drawFrame() {
        requestAnimationFrame(()=>this.drawFrame());

        this.now = Date.now();
        this.elapsed = this.now - this.then;

        if (this.elapsed > this.fpsInterval) {
            this.then = this.now - (this.elapsed % this.fpsInterval);
            this.drawSpectrum(this.analyser, this.spectCtx);
            this.drawScope(this.analyser, this.scopeCtx);
        }
    }

    drawSpectrum(analyser, ctx) {
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

    drawScope(analyser, ctx) {
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
    
    startDrawing(fps) {
        this.stop = false;
        this.frameCount = 0;
        this.$results = $("#results");
        this.fps = fps;
        this.fpsInterval = 1000 / fps;
        this.then = Date.now();
        this.startTime = this.then;
        this.drawFrame();
    }
    
    getChannel(num) {
        return this.rawAudio.getChannelData(num);
    }
}
