// Subception
// audiovisual installation for computer-generated audio and video signals, projection, 4 loudspeakers, subwοofer, etc etc

// by Marinos Koutsomichalis
// March 2013, Brussels

// settings
var masterOut = 2;       // the main audio output
var soundDriver, availableDrivers;
var samplingRate = 96000;
var remoteIP = "127.0.0.1";


// synthesis
var buf;                 // buffers for the wavetable ??
var freqs;               // an array of streams for the freqs - to change
var textureLow, textureHigh; // audio engines
var shadowSynth = nil!2;
var master;                  // the master synth

// osc and other
var netAddrSend, netAddrReceive;
var shadowsOn;     // flags

// GUI
var window;

// cleanUp, and startEverything
var cleanUp, startEverything;

// set server to internal
Server.default = s = Server.internal;

// GUI with settings
window = Window("SUBCEPTION", Rect(100,300,400,390),false).front;
window.addFlowLayout;
window.onClose_({cleanUp.value});
StaticText(window, 400@80).font_(Font("Baskerville", 40, true)).stringColor_(Color.black).align_(\center).string_("SUBCEPTION");
StaticText(window, 20@40); // to align it properly;
PopUpMenu(window,350@40).items_(
	availableDrivers=(#["Select Audio Device"]++ServerOptions.devices)).action_({
	arg i;
	soundDriver=availableDrivers[i.value];
	s.options.device_(soundDriver);
});
StaticText(window, 20@40); // to align it properly;
PopUpMenu(window,350@40).items_(
	#["Select Sampling Rate",44100,96000,192000]).action_({
	arg i;
	samplingRate=#["Select Sampling Rate",44100,96000,192000][i.value];
	s.options.sampleRate_(samplingRate);
});
StaticText(window, 20@40); // to align it properly;
PopUpMenu(window,350@40).items_(
	#["Select Output Channels","0-3","1-4","2-5","3-6","4-7","5-8"])
.action_({
	arg i;
	masterOut=#[nil,0,1,2,3,4,5][i.value]});
StaticText(window, 20@40); // to align it properly;
TextField(window,350@40).string_("IP of remote computer").action_({
	arg f; remoteIP=f.value.asString;
});
StaticText(window, 90@40); // to align it properly;
Button(window,200@40).states_([
	["start", Color.green],
	["Stop", Color.red]
]).action_({
	arg but;
	if (but.value==1) {
		startEverything.value;
	} {
		cleanUp.value;
	}
});
StaticText(window, 400@80).font_(Font("Baskerville", 14, true)).stringColor_(Color.black).align_(\center).string_("An AudioVisual Installation.\n(c) Marinos Koutsomichalis. Brussels, 2013");

// cleanUp
cleanUp = {
	textureLow.stop;
	textureHigh.stop;
	shadowSynth.do{arg i; i.free};
	s.quit;
};

startEverything = {
	s.waitForBoot({

		// netAddress
		netAddrSend = NetAddr(remoteIP, 3000);
		netAddrReceive = NetAddr(remoteIP, nil);

		// SynthDefs
		{
			SynthDef(\generator, { arg amp=0.8, freq1=50, freq2=60,
				dur=10, gate=1, out = masterOut, interp=7, ad = 1,
				dd = 1, shaper;
				var signal, gen1, gen2, env;
				env = Env([0,1,0],[dur/2,dur/2],[-6,6]);
				gen1 = Limiter.ar((SinOsc.ar(Line.kr(freq1,
					freq1*0.98,dur)).sqrt+3)
					/(SinOsc.ar(Line.kr(
						freq2,freq2*0.96,dur)).cubed+3),0.8);
				gen2 = GendyI.ar(3, 2, ad, dd,
					Line.kr(freq1,freq1*0.94,dur),
					Line.kr(freq2,freq2*0.90,dur), 0.7, 0.7, 3,
					interpolation:interp);
				signal = gen1 + gen2;
				signal = signal*EnvGen.ar(env,gate:gate, doneAction:2);
				signal = signal*amp*2;
				signal = Shaper.ar(shaper, signal);
				signal = LeakDC.ar(signal);
				Out.ar(out, signal);
			}).add;
			s.sync;
		}.fork;

		// Buffers
		buf = Array.fill(2,{Buffer.alloc(s, 1024);});
		{
			var signal, signal2;
			fork{
				signal = Signal.fill(513, {arg i;
					i.linlin(0.0, 512.0, -1.0, 1.0) });
				1.wait;
				buf[0].sendCollection(signal.asWavetableNoWrap);
				signal2 = Signal.chebyFill(513, [0.3, -0.8, 1.1]);
				1.wait;
				buf[1].sendCollection(signal2.asWavetableNoWrap);
				1.wait;
			};
		}.value;

		// flags
		shadowsOn = false;

		// freqs
		freqs = [
			Pwrand([
				Pbrown(54.0,57.0, 0.5, Prand((2..6),1)),
				Pbrown(42.0,44.0,0.5, Prand((2..6),1)),
				Pbrown(68.5,70.5,0.5, Prand((2..6),1)),
				Pbrown(45,68,0.5, Prand((2..6),1))
			],[0.25, 0.25, 0.35, 0.15],inf).asStream,
			Pwrand([
				Pbrown(54.0,57.0, 0.5, Prand((2..6),1)),
				Pbrown(42.0,44.0,0.5, Prand((2..6),1)),
				Pbrown(68.5,70.5,0.5, Prand((2..6),1)),
				Pbrown(45,68,0.5, Prand((2..6),1))
			],[0.25, 0.25, 0.35, 0.15],inf).asStream,
			Pwrand([
				Pbrown(4000,6000, 100, Prand((5..10),1)),
				Pbrown(6000,9000, 500, Prand((5..10),1)),
				Pbrown(9000,12000, 500, Prand((2..5),1))
			],[0.5,0.4,0.1],inf).asStream,
			Pwrand([
				Pbrown(4000,6000, 100, Prand((5..10),1)),
				Pbrown(6000,9000, 500, Prand((5..10),1)),
				Pbrown(9000,12000, 500, Prand((2..5),1))
			],[0.5,0.4,0.1],inf).asStream
		];

		// start audio
		{
			textureLow = fork{loop{
				var dur = if (0.8.coin) {rrand(8.0,20.0)}
				{rrand(20.0,30.0)};
				var out = rrand(masterOut,masterOut+3);
				var time = if (0.9.coin) {rrand(5.0,11.0)}
				{rrand(11.0,20.0)};
				var amp = rrand(0.3,0.7);
				var freq1 = freqs[0].next;
				var freq2 = freqs[1].next;
				Synth(\generator, [\freq1,freq1, \freq2,
					freq2, \dur, dur,\out, out, \amp, amp,
					\shaper, buf.wchoose([0.8,0.2])]);
				netAddrSend.sendMsg("/spawn", dur, freq1);
				netAddrSend.sendMsg("/spawn", dur, freq2);
				time.wait;
			}};
			textureHigh = fork{loop{
				var dur = if (0.8.coin) {rrand(8.0,20.0)}
				{rrand(20.0,30.0)};
				var out = rrand(masterOut,masterOut+3);
				var time = if (0.9.coin) {rrand(5.0,11.0)}
				{rrand(11.0,20.0)};
				var amp = rrand(0.3,0.7);
				var freq1 = freqs[2].next;
				var freq2 = freqs[3].next;
				Synth(\generator, [\freq1, freq1, \freq2,
					freq2, \dur, dur,\out, out, \amp, amp, \shaper,
					buf.wchoose([0.7,0.3])]);
				netAddrSend.sendMsg("/spawn", dur, freq1);
				netAddrSend.sendMsg("/spawn", dur, freq2);
				time.wait;
			}};
		}.defer(3);

		// respond to incoming OSC
		OSCFunc({ arg msg;
			{
				var dur = msg[1]/60;
				// msg.postln;
				shadowsOn = true;
				shadowSynth[0] = Synth(\generator, [\dur, dur,
					\out, rrand(masterOut,masterOut+1), \amp, 0.6,
					\interp, 4, \shaper, buf.wchoose([0.2,0.8])]);
				shadowSynth[1] = Synth(\generator, [\dur, dur,
					\out, rrand(masterOut+2,masterOut+3), \amp, 0.6,
					\interp, 4, \shaper, buf.wchoose([0.2,0.8])]);
				{shadowsOn = false;}.defer(dur);
			}.defer;
		}, "/shadows/duration", netAddrReceive);
		OSCFunc({ arg msg;
			{
				shadowSynth[0].setn(\freq1, msg[1]*2,
					\freq2, msg[3]*2, \ad, (msg[2]/1280)+0.01, \dd,
					(msg[4]/768)+0.01);
				shadowSynth[1].setn(\freq1, msg[2]/12,
					\freq2, msg[4]/12, \ad, (msg[1]/1280)+0.01, \dd,
					(msg[3]/768)+0.01);
			}.defer;
		}, "/shadows/freqs", netAddrReceive);
	})
};





