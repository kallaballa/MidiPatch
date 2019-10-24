    local hpNoise =  LPF12():cutoff(8000):input(HPF24():cutoff(2000.0):input(Noise() * dBToLin(-18.0)));
    local tones = SineWave():freq(180) * dBToLin(-6.0) + SineWave():freq(332) * dBToLin(-18.0);
    
    local metro = ControlMetro(120);
    local snareGate = ControlPulse(60.0*0.8/120.0):trigger(metro);
    local toneADSR = ADSR(0.0005, 0.03, 0.0, 0.01):trigger(snareGate);
    local noiseADSR = ADSR(0.001, 0.25, 0.0, 0.25):trigger(snareGate);
    
    local noiseEnv = noiseADSR * noiseADSR;
    
    local duckingComp = Compressor()
    :attack(0.001)
    :release( synth:addParameter("compRelease", 0.025) )
    :threshold( dBToLin(-52) )
    :ratio(16)
    :lookahead(0.001);
    
    local snare =  ( tones * toneADSR ) + ( hpNoise * noiseEnv );
    
    local bassScaleArr = {0,7,10,12};

    local baseFreq = (ControlMidiToFreq():input((ControlSnapToScale():setScale(bassScaleArr):input(ControlRandom():min(0):max(12):trigger(metro))) + 30)):smoothed(0.05);

    local bassFiltADSR = ADSR(0.005, 0.1, 0, 0.05):doesSustain(0):legato(0):trigger(metro);
    
    local randomBass = LPF24():cutoff( (bassFiltADSR * 2 + 1) *  300 * (((SineWave():freq(0.1) + 1) * 0.5) + 1)):Q(1.5):input(RectWave():freq( baseFreq ) * 0.5);
    
    --StereoDelay delay = StereoDelay(0.37, 0.38).feedback(0.4).dryLevel(0.8).wetLevel(0.2);
    
    synth:setOutputGen(duckingComp:audioInput(randomBass):sidechainInput(snare)  + snare * 0.5);
