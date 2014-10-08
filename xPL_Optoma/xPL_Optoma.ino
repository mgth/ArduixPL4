#include <ArduHA.h>
#include <xpl.h>
#include "utility/xpl_parser.h"
#include "xpl_ENC28J60.h"

xPL_ENC28J60Adapter adapter;

int bounds(int value, int min, int max)
{
	if (value < min) value = min;
	if (value > max) value = max;
	return value;
}

String twoDigits(int n)
{
		return ((n < 10)?"0":"") + String(n);
}

class Optoma
{
	String id = "00";

	bool send(int cmd, String value) {
		Serial.print("~");
		Serial.print(id);
		Serial.print(twoDigits(cmd));
		Serial.print(' ');
		Serial.print(value);
		Serial.print((char)0x0D);
		String res = Serial.readStringUntil(0x0D);
		if (res == "OK") return true;
		return false;
	}

	bool send(int cmd, int value)
	{
		return send(cmd, String(value));
	}

	bool send(int cmd, int value, int min, int max)
	{
		if (value < min) return false;
		if (value > max) return false;
		return send(cmd, value);
	}

	bool send5(int cmd, int value) { return send(cmd, value, -5, 5); }
	bool send50(int cmd, int value) { return send(cmd, value, -50, 50); }
	bool send100(int cmd, int value) { return send(cmd, value, -100, 100); }

	int get(int cmd, int value)
	{
		if (!send(cmd, value)) return -1;

	}

public:
	bool power(bool value = true, int password = -1) {
		if (password < 0) return send(0, value);
		return send(0, String(value) + ' ' + String(password));
	}

	bool resync() { return send(1, 1); }
	bool avMute(bool value = true) { return send(2, value); }
	bool mute(bool value = true) { return send(3, value); }
	bool freeze(bool value = true) { return send(4, value); }
	bool unfreeze() { return freeze(false); }
	bool zoom_Plus() { return send(5, 1); }
	bool zoom_Minus() { return send(6, 1); }

	bool directSource(byte src) { return send(12, src); }
	bool directSource_Hdmi() { return directSource(1); }
	bool directSource_Hdmi2() { return directSource(15); }
	bool directSource_Vga() { return directSource(5); }
	bool directSource_Vga2() { return directSource(6); }
	bool directSource_Video() { return directSource(10); }

	bool displayMode(byte mode) { return send(20, mode); }
	bool displayMode_Cinema() { return displayMode(1); }
	bool displayMode_Reference() { return displayMode(4); }
	bool displayMode_Photo() { return displayMode(3); }
	bool displayMode_Bright() { return displayMode(2); }
	bool displayMode_3D() { return displayMode(9); }
	bool displayMode_User() { return displayMode(5); }

	bool brightness(char value) { return send50(21, value); }
	bool contrast(char value) { return send50(22, value); }
	bool charpness(char value) { return send(23, value, 1, 15); }

	bool color(byte num, char value) { return send50(24 + bounds(num, 0, 2), value); }
	bool color(char red, char green, char blue) { return color(0, red) && color(1, green) && color(2, blue); }

	bool colorReset() { send(33, 1); }

	bool brillantColor(char value) { return send(34, bounds(value, 1, 10)); }

	bool dynamicBlack(char value) { return send(191, value, 0, 2); }
	bool dynamicBlack_Off() { return dynamicBlack(0); }
	bool dynamicBlack_Low() { return dynamicBlack(1); }
	bool dynamicBlack_High() { return dynamicBlack(2); }

	bool gamma(char value) { return send(35, value, 1, 4); }
	bool gamma_Film() { return gamma(1); }
	bool gamma_Video() { return gamma(2); }
	bool gamma_Graphics() { return gamma(3); }
	bool gamma_Standard() { return gamma(4); }

	bool colorTemp(char value) { return send(36, bounds(value, 1, 3)); }
	bool colorTemp_Warm() { return colorTemp(1); }
	bool colorTemp_Medium() { return colorTemp(2); }
	bool colorTemp_Cold() { return colorTemp(3); }

	bool colorSpace(char value) { return send(36, bounds(value, 1, 4)); }
	bool colorSpace_Auto() { return colorSpace(1); }
	bool colorSpace_RGB() { return colorSpace(2); }
	bool colorSpace_YUV() { return colorSpace(3); }
	bool colorSpace_RGB_16_235() { return colorSpace(4); }

	bool signal_Frequency(char n) { return send5(73, n); }
	bool signal_Automatic(bool n) { return send(91, n ? 1 : 0); }
	bool signal_Phase(char n) { return send(74, n, 0, 31); }
	bool signal_HPosition(char n) { return send5(75, n); }
	bool signal_VPosition(char n) { return send5(76, n); }

	bool signal_WhiteLevel(char n) { return send50(200, n); }
	bool signal_BlackLevel(char n) { return send50(201, n); }
	bool signal_Saturation(char n) { return send50(202, n); }
	bool signal_Hue(char n)        { return send50(203, n); }

	bool colorSaturation(char n) { return send50(45, n); }
	bool tint(char n)            { return send50(44, n); }

	bool format(byte n) { return send(44, n, 1, 9); };
	bool format_4_3() { return format(1); }
	bool format_16_9() { return format(2); }
	bool format_LBX() { return format(5); }
	bool format_Native() { return format(6); }
	bool format_Auto() { return format(7); }
	bool format_Superwide() { return format(9); }

	bool edgeMask(char n) { return send(61, n, 0, 30); }
	bool zoom(char n) { return send(62, n, -5, 25); }
	bool HImageShift(char n) { return send100(63, n); }
	bool VImageShift(char n) { return send100(64, n); }
	bool VKeyStone(char n) { return send(66, n, -40, 40); }

	bool _3D_Mode(char n) { return send(230, n, 1, 3); }
	bool _3D_Mode_DlpLink() { return _3D_Mode(1); }
	bool _3D_Mode_IR() { return _3D_Mode(3); }

	bool _3D2D(char n) { return send(400, n, 0, 2); }
	bool _3D2D_3D() { return _3D2D(0); }
	bool _3D2D_L() { return _3D2D(1); }
	bool _3D2D_R() { return _3D2D(2); }

	bool _3D_Format(char n) { return send(405, n, 0, 3); }
	bool _3D_Format_Auto() { return _3D_Format(0); }
	bool _3D_Format_SBS() { return _3D_Format(1); }
	bool _3D_Format_TopBottom() { return _3D_Format(2); }
	bool _3D_Format_FrameSequencial() { return _3D_Format(3); }

	bool _3D_SyncInvert(bool b) { return send(231, b); }

	bool Language(char n) { return send(70, n, 1, 27); }

	bool projection(char n) { return send(71, n, 1, 4); }
	bool projection_FrontDesktop() { return projection(1); }
	bool projection_RearDesktop() { return projection(2); }
	bool projection_FrontCeiling() { return projection(3); }
	bool projection_RearCeiling() { return projection(4); }

	bool menuLocation(char n) { return send(72, n, 1, 5); }
	bool menuLocation_TopLeft() { return menuLocation(1); }
	bool menuLocation_TopRight() { return menuLocation(2); }
	bool menuLocation_Center() { return menuLocation(3); }
	bool menuLocation_BottomLeft() { return menuLocation(4); }
	bool menuLocation_BottomRight() { return menuLocation(5); }

	bool security(char month, char day, char hour)
	{
		if (month<0 || day<0 || hour<0 || month>12 || day>30 || hour>24) return false;
		return send(77, twoDigits(month) + twoDigits(day) + twoDigits(hour));
	}

	bool securitySettings(bool b) { return send(78, b ? 1 : 0); }

	bool projectorID(char id) { return send(79, id, 0, 99); }

	bool muteAudio(bool b) { return send(80, b ? 1 : 0); }
	bool internalSpeaker(bool b) { return send(310, b ? 1 : 0); }
	bool volume(char n) { return send(81, n, 0, 10); }
	bool SRS(bool b) { return send(94, b ? 1 : 0); }

	bool treble(char n) { return send(95, n, 0, 10); }
	bool bass(char n) { return send(96, n, 0, 10); }

	bool audioInput(char n) { return send(89, n, 0, 3); }
	bool audioInput_Default() { return audioInput(0); }
	bool audioInput_Audio1() { return audioInput(1); }
	bool audioInput_Audio2() { return audioInput(3); }

	bool logo(char n) { return send(82, n, 1, 3); }
	bool logo_default() { return logo(1); }
	bool logo_User() { return logo(2); }
	bool logo_Neutral() { return logo(3); }

	bool logoCapture() { return send(83, 1); }

	bool closedCaptioning(char n) { return send(88, n, 0, 2); }
	bool closedCaptioning_Off() { return closedCaptioning(0); }
	bool closedCaptioning_cc1() { return closedCaptioning(1); }
	bool closedCaptioning_cc2() { return closedCaptioning(2); }

	bool sourceLock(bool b) { return send(100, b ? 1 : 0); }
	bool highAltitude(bool b) { return send(101, b ? 1 : 0); }
	bool informationHide(bool b) { return send(102, b ? 1 : 0); }
	bool keypadLock(bool b) { return send(103, b ? 1 : 0); }

	bool testPattern(char n) { return send(195, n, 0, 2); }
	bool testPattern_None() { return testPattern(0); }
	bool testPattern_Grid() { return testPattern(1); }
	bool testPattern_WhitePattern() { return testPattern(2); }

	bool irFunction(bool b) { return send(11, b ? 1 : 0); }

	bool backgroundColor(char n) { return send(104, n,1,5); }
	bool backgroundColor_Blue() { return backgroundColor(1); }
	bool backgroundColor_Black() { return backgroundColor(2); }
	bool backgroundColor_Red() { return backgroundColor(3); }
	bool backgroundColor_Green() { return backgroundColor(4); }
	bool backgroundColor_White() { return backgroundColor(5); }

	bool directPowerOn(bool b) { return send(105, b ? 1 : 0); }

	bool autoPowerOff(byte n) { return send(106, n, 0, 180); }

	bool sleepTimer(int n) { return send(107, n, 0, 995); }

	bool standbyPowerModeEco(bool b){ return send(114, b ? 1 : 0); }

	bool lampReminder(bool b){ return send(109, b ? 1 : 0); }

	bool lampMode(char n){ return send(110, n, 1, 2); }
	bool lampMode_Bright() { return lampMode(1); }
	bool lampMode_Eco() { return lampMode(2); }

	bool lampReset(bool b=true){ return send(111, b ? 1 : 0); }

	bool resetAll(bool b = true) { return send(112, b ? 1 : 0); }

	bool rs232AlertReset() { return send(99, 1); }

	bool displayMessage(const String& s) {
		if (s.length() > 30) return false;
		send(210, s);
	}

	bool remote(char key) { send(140, key, 10, 28); }
	bool remote_Up() { return remote(10); }
	bool remote_Left() { return remote(11); }
	bool remote_Enter() { return remote(12); }
	bool remote_Right() { return remote(13); }
	bool remote_Down() { return remote(14); }
	bool remote_KeystonePlus() { return remote(15); }
	bool remote_KeystoneMinus() { return remote(16); }
	bool remote_VolumePlus() { return remote(17); }
	bool remote_VolumeMinus() { return remote(18); }
	bool remote_Brightness() { return remote(19); }
	bool remote_Menu() { return remote(20); }
	bool remote_Zoom() { return remote(21); }
	bool remote_Contrast() { return remote(28); }

	char getInputSource() { return get(121,1); }
	char getSoftwareVersion() { return get(122,1); }
	char getDisplayMode() { return get(123,1); }
	char getPowerState() { return get(124,1); }
	char getBrightness() { return get(125,1); }
	char getContrast() { return get(126,1); }
	char getAspectRation() { return get(127,1); }
	char getColorTemperature() { return get(128,1); }
	char getProjectionMode() { return get(129,1); }
	char getInformation() { return get(150,1); }
	char getModelName() { return get(151,1); }
	char getLampHours() { return get(108,1); }
	char getCumulativeLampHours() { return get(108,2); }


} optoma;

class OptomaParser : xPL_MessageParser
{
	String _command;
	String _value;

public:
	OptomaParser() :xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("control"), F("optoma")))
	{}
	bool parse(const xPL_KeyValue& key)
	{
		String cmd = key.key;
		cmd.toLowerCase();
		String val = key.value;
		val.toLowerCase();

		char v0 = val[0];

		bool valOn = (val == F("on")) || (val == F("1")) || /*(val == F("true")) ||*/ (val==F("enabled"));
		bool valOff = (val == F("off")) || (val == F("0")) || /*(val == F("false")) ||*/ (val == F("disabled"));

		if (cmd == F("power"))
			if (valOn||valOff) optoma.power(valOn);
			else optoma.power(val.toInt());

		if (cmd == F("resync") && valOn) return optoma.resync();
		if (cmd == F("avmute")) return optoma.avMute(valOn);
		if (cmd == F("mute")) return optoma.mute(valOn);
		if (cmd == F("freeze")) return optoma.freeze(valOn);

		if (cmd == F("zoom"))
			if (val == F("+")) return optoma.zoom_Plus();
			else if (val==F("-")) return optoma.zoom_Minus();
			else return false;

		if (cmd == F("source"))
			if (val == F("hdmi1")) optoma.directSource_Hdmi();
			else if (val == F("hdmi2")) return optoma.directSource_Hdmi2();
			else if (val == F("vga1")) return optoma.directSource_Vga();
			else if (val == F("vga2")) return optoma.directSource_Vga2();
			else if (val == F("video")) return optoma.directSource_Video();
			else return false;

		if (cmd == F("displaymode"))
#ifdef fulldecode
			if (val == F("cinema")) return optoma.displayMode_Cinema();
			else if (val == F("reference")) return optoma.displayMode_Reference();
			else if (val == F("photo")) return optoma.displayMode_Photo();
			else if (val == F("bright")) return optoma.displayMode_Bright();
			else if (val == F("3d")) return optoma.displayMode_3D();
			else if (val == F("user")) return optoma.displayMode_User();
#else
		if (v0 == 'c'/*cinema*/) return optoma.displayMode_Cinema();
		else if (v0 == 'r'/*reference*/) return optoma.displayMode_Reference();
		else if (v0 == 'p'/*photo*/) return optoma.displayMode_Photo();
		else if (v0 == 'b'/*bright*/) return optoma.displayMode_Bright();
		else if (v0 == '3'/*3d*/) return optoma.displayMode_3D();
		else if (v0 == 'u'/*user*/) return optoma.displayMode_User();
#endif
		else return false;

		if (cmd == F("brightness")) return optoma.brightness(val.toInt());
		if (cmd == F("contrast")) return optoma.contrast(val.toInt());
		if (cmd == F("charpness")) return optoma.charpness(val.toInt());

		if (cmd == F("color")) 
		if (key.count() == 3) return optoma.color(key.val(0).toInt(), key.val(1).toInt(), key.val(2).toInt());
			else return false;

		//if (cmd == F("colorred")) return optoma.color(0,val.toInt());
		//if (cmd == F("colorgreen")) return optoma.color(1,val.toInt());
		//if (cmd == F("colorblue")) return optoma.color(2,val.toInt());

		if (cmd == F("brillantcolor")) return optoma.brillantColor(val.toInt());

		if (cmd == F("dynamicblack"))
			if (valOff) return optoma.dynamicBlack(0);
			//else if (val == F("low")) return optoma.dynamicBlack(1);
			//else if (val == F("high")) return optoma.dynamicBlack(2);
			else if (v0 == 'l') return optoma.dynamicBlack(1);
			else if (v0 == 'h') return optoma.dynamicBlack(2);

			else return false;

		if (cmd == F("gamma"))
#ifdef FULL_DECODE
			if (val == F("film")) return optoma.gamma_Film();
			else if (val == F("video")) return optoma.gamma_Video();
			else if (val == F("graphics")) return optoma.gamma_Graphics();
			else if (val == F("standard")) return optoma.gamma_Standard();
#else
			if (v0 == 'f'/*film*/) return optoma.gamma_Film();
			else if (v0 == 'v'/*video*/) return optoma.gamma_Video();
			else if (v0 == 'g'/*graphics*/) return optoma.gamma_Graphics();
			else if (v0 == 's'/*standard*/) return optoma.gamma_Standard();
#endif
		else return false;

		if (cmd == F("colortemp"))
#ifdef FULL_DECODE
			if (val == F("warm")) return optoma.colorTemp_Warm();
			else if (val == F("medium")) return optoma.colorTemp_Medium();
			else if (val == F("cold")) return optoma.colorTemp_Cold();
#else
			if (v0 == 'w'/*warm*/) return optoma.colorTemp_Warm();
			else if (v0 == 'm'/*medium*/) return optoma.colorTemp_Medium();
			else if (v0 == 'c'/*cold*/) return optoma.colorTemp_Cold();
#endif
			else return false;

		if (cmd == F("colorspace"))
#ifdef FULL_DECODE
			if (val == F("auto")) return optoma.colorSpace_Auto();
			else if (val == F("rgb")) return optoma.colorSpace_RGB();
			else if (val == F("yuv")) return optoma.colorSpace_YUV();
			else if (val == F("rgb16-235")) return optoma.colorSpace_RGB_16_235();
#else
			if (v0 == 'a'/*auto*/) return optoma.colorSpace_Auto();
			else if (v0 == 'y'/*yuv*/) return optoma.colorSpace_YUV();
			else if (val.startsWith("rgb1")/*rgb16-235*/) return optoma.colorSpace_RGB_16_235();
			else if (v0 == 'r'/*rgb*/) return optoma.colorSpace_RGB();
#endif
			else return false;

		if (cmd == F("frequency")) return optoma.signal_Frequency(val.toInt());
		if (cmd == F("automatic")) return optoma.signal_Automatic(valOn);
		if (cmd == F("phase")) return optoma.signal_Phase(val.toInt());
		if (cmd == F("hposition")) return optoma.signal_HPosition(val.toInt());
		if (cmd == F("vposition")) return optoma.signal_VPosition(val.toInt());

		if (cmd == F("blacklevel")) return optoma.signal_BlackLevel(val.toInt());
		if (cmd == F("whitelevel")) return optoma.signal_WhiteLevel(val.toInt());
		if (cmd == F("saturation")) return optoma.signal_Saturation(val.toInt());
		if (cmd == F("hue")) return optoma.signal_Hue(val.toInt());

		if (cmd == F("colorsaturation")) return optoma.colorSaturation(val.toInt());
		if (cmd == F("tint")) return optoma.tint(val.toInt());

		if (cmd == F("format"))
#ifdef FULL_DECODE
			if (val == F("4:3")) optoma.format_4_3();
			else if (val == F("16:9")) return optoma.format_16_9();
			else if (val == F("lbx")) return optoma.format_LBX();
			else if (val == F("native")) return optoma.format_Native();
			else if (val == F("auto")) return optoma.format_Auto();
			else if (val == F("superwide")) return optoma.format_Superwide();
#else
			if (v0 == '4'/*4:3*/) optoma.format_4_3();
			else if (v0 == '1'/*16:9*/) return optoma.format_16_9();
			else if (v0 == 'l'/*lbx*/) return optoma.format_LBX();
			else if (v0 == 'n'/*native*/) return optoma.format_Native();
			else if (v0 == 'a'/*auto*/) return optoma.format_Auto();
			else if (v0 == 's'/*superwide*/) return optoma.format_Superwide();
#endif
			else return false;

		if (cmd == F("edgemask")) return optoma.edgeMask(val.toInt());
		if (cmd == F("zoom")) return optoma.zoom(val.toInt());
		if (cmd == F("himageshift")) return optoma.HImageShift(val.toInt());
		if (cmd == F("vimageshift")) return optoma.VImageShift(val.toInt());
		if (cmd == F("vkeystone")) return optoma.VKeyStone(val.toInt());

#ifdef FULL_DECODE
		if (cmd == F("3dmode"))
			if (val == F("dlp-link")) return optoma._3D_Mode_DlpLink();
			else if (val == F("ir")) return optoma._3D_Mode_IR();
#else
		if (cmd.startsWith(F("3dm")))
			if (v0 == 'd'/*dlp-link*/) return optoma._3D_Mode_DlpLink();
			else if (v0 == 'i'/*ir*/) return optoma._3D_Mode_IR();
#endif
			else return false;

#ifdef FULL_DECODE
		if (cmd == F("3dformat"))
		if (val == F("auto")) return optoma._3D_Format_Auto();
		else if (val == F("sbs")) return optoma._3D_Format_SBS();
		else if (val == F("top-bottom")) return optoma._3D_Format_TopBottom();
		else if (val == F("framesequencial")) return optoma._3D_Format_FrameSequencial();
#else
		if (cmd.startsWith(F("3df")))
		if (v0 == 'a'/*auto*/) return optoma._3D_Format_Auto();
		else if (v0 == 's'/*sbs*/) return optoma._3D_Format_SBS();
		else if (v0 == 't'/*top-bottom*/) return optoma._3D_Format_TopBottom();
		else if (v0 == 'f'/*framesequencial*/) return optoma._3D_Format_FrameSequencial();
#endif
		else return false;

		if (cmd == F("language")) return optoma.Language(val.toInt());

//		if (cmd == F("projection"))
		if (cmd.startsWith(F("proj")))
			if (val = F("front-desktop")) return optoma.projection_FrontDesktop();
			else if (val == F("rear-desktop")) return optoma.projection_RearDesktop();
			else if (val == F("front-ceiling")) return optoma.projection_FrontCeiling();
			else if (val == F("rear-ceiling")) return optoma.projection_RearCeiling();
			else return false;

		if (cmd == F("menulocation"))
			if (val = F("top-left")) return optoma.menuLocation_TopLeft();
			else if (val == F("top-right")) return optoma.menuLocation_TopRight();
			else if (val == F("center")) return optoma.menuLocation_Center();
			else if (val == F("bottom-left")) return optoma.menuLocation_TopLeft();
			else if (val == F("bottom-right")) return optoma.menuLocation_TopRight();
			else return false;

			if (cmd == F("security"))
			{
				byte n = key.count('/');
				switch (n)
				{
				case 1:
					//return optoma.security(0, 0, val.toInt());
				case 2:
					//return optoma.security(0, key.val(0).toInt(), key.val(1).toInt());
					return false;
				case 3:
					return optoma.security(key.val(0).toInt(), key.val(1).toInt(), key.val(2).toInt());
				}
				return false;
			}

			if (cmd == F("projectorid")) return optoma.projectorID(val.toInt());

			if (cmd == F("muteaudio")) return optoma.muteAudio(valOn);
			if (cmd == F("internalspeaker")) return optoma.internalSpeaker(val.toInt());
			if (cmd == F("volume")) return optoma.volume(val.toInt());
			if (cmd == F("srs")) return optoma.volume(valOn);

			if (cmd == F("treble")) return optoma.treble(val.toInt());
			if (cmd == F("bass")) return optoma.bass(val.toInt());

			if (cmd == F("audioinput"))
//				if (val == F("default")) return optoma.audioInput_Default();
				if (v0 == 'd'/*default*/) return optoma.audioInput_Default();
				else if (val == F("audio1")) return optoma.audioInput_Audio1();
				else if (val == F("audio2")) return optoma.audioInput_Audio2();

			if (cmd == F("logo"))
//				if (val == F("default")) return optoma.logo_default();
				if (v0 == 'd'/*default*/) return optoma.logo_default();
				else if (val == F("user")) return optoma.logo_User();
				else if (val == F("neutral")) return optoma.logo_Neutral();

			if (cmd == F("logocapture")) return optoma.logoCapture();

			if (cmd == F("closedcaptioning"))
				if (valOff) return optoma.closedCaptioning_Off();
				else if (val == "cc1") return optoma.closedCaptioning_cc1();
				else if (val == "cc2") return optoma.closedCaptioning_cc2();
				else return false;

			if (cmd == F("sourcelock")) return optoma.sourceLock(valOn);
			if (cmd == F("highaltitude")) return optoma.highAltitude(valOn);
			if (cmd == F("informationhide")) return optoma.informationHide(valOn);
			if (cmd == F("keypadlock")) return optoma.keypadLock(valOn);
			if (cmd == F("testpattern"))
#ifdef FULL_DECODE
				if (val==F("none")) return optoma.testPattern_None();
				else if (val==F("grid")) return optoma.testPattern_Grid();
				else if (val==F("whitepattern")) return optoma.testPattern_Grid();
#else		

				if (v0=='n'/*none*/) return optoma.testPattern_None();
				else if (v0=='g'/*grid*/) return optoma.testPattern_Grid();
				else if (v0 == 'w'/*whitepattern*/) return optoma.testPattern_WhitePattern();
				else return false;
#endif
			if (cmd == F("irfunction")) return optoma.irFunction(valOn);

			if (cmd == F("backgroundcolor"))
#ifdef FULL_DECODE
			if (val == F("blue")) return optoma.backgroundColor_Blue();
				else if (val == F("black")) return optoma.backgroundColor_Black();
				else if (val == F("red")) return optoma.backgroundColor_Red();
				else if (val == F("green")) return optoma.backgroundColor_Green();
				else if (val == F("white")) return optoma.backgroundColor_White();
#else
			if (val.startsWith("blu")) return optoma.backgroundColor_Blue();
			else if (val.startsWith("bla")) return optoma.backgroundColor_Black();
			else if (v0 == 'r'/*red*/) return optoma.backgroundColor_Red();
			else if (v0 == 'g'/*green*/) return optoma.backgroundColor_Green();
			else if (v0 == 'w'/*white*/) return optoma.backgroundColor_White();
#endif
			else return false;

			if (cmd == F("directpoweron")) return optoma.directPowerOn(valOn);
			if (cmd == F("autopoweroff")) return optoma.autoPowerOff(val.toInt());
			if (cmd == F("sleeptimer")) return optoma.sleepTimer(val.toInt());

			if (cmd == F("powermode"))
#ifdef FULL_DECODE
				if (val == F("eco")) return optoma.standbyPowerModeEco(true);
				else if (val == F("active")) return optoma.standbyPowerModeEco(false);
#else
				if (v0 == 'e'/*eco*/) return optoma.standbyPowerModeEco(true);
				else if (v0 == 'a'/*active*/) return optoma.standbyPowerModeEco(false);
#endif
				else return false;

			if (cmd == F("lampreminder")) return optoma.lampReminder(valOn);

			if (cmd == F("lampmode"))
#ifdef FULL_DECODE
				if (val == F("bright")) return optoma.lampMode_Bright();
				else if (val == F("eco")) return optoma.lampMode_Eco();
#else
				if (v0 == 'b'/*bright*/) return optoma.lampMode_Bright();
				else if (v0 == 'e'/*eco*/) return optoma.lampMode_Eco();
#endif
				else return false;

			if (cmd == F("lampreset")) return optoma.lampReset(valOn);
			if (cmd == F("resetall"))
#ifdef FULL_DECODE
			if (val == F("yes")) return optoma.resetAll(true);
				else if (val == F("cancel")) return optoma.resetAll(false);
#else
			if (v0 == 'y'/*yes*/) return optoma.resetAll(true);
				else if (v0 == 'c'/*cancel*/) return optoma.resetAll(false);
#endif
			else return false;

			if (cmd == F("alertreset")) return optoma.rs232AlertReset();

			if (cmd == F("dispaymessage")) return optoma.displayMessage(val);

			if (cmd == F("remote"))
#ifdef FULL_DECODE
				if (val==F("up")) return optoma.remote_Up();
				else if (val == F("left")) return optoma.remote_Left();
				else if (val == F("enter")) return optoma.remote_Enter();
				else if (val == F("right")) return optoma.remote_Right();
				else if (val == F("down")) return optoma.remote_Down();
				else if (val == F("keystone+")) return optoma.remote_KeystonePlus();
				else if (val == F("keystone-")) return optoma.remote_KeystoneMinus();
				else if (val == F("volume+")) return optoma.remote_VolumePlus();
				else if (val == F("volume-")) return optoma.remote_VolumeMinus();
				else if (val == F("brightness")) return optoma.remote_Brightness();
				else if (val == F("menu")) return optoma.remote_Menu();
				else if (val == F("zoom")) return optoma.remote_Zoom();
				else if (val == F("contrast")) return optoma.remote_Contrast();
#else
				if (v0=='u'/*up*/) return optoma.remote_Up();
				else if (v0 == 'l'/*left*/) return optoma.remote_Left();
				else if (v0 == 'e'/*enter*/) return optoma.remote_Enter();
				else if (v0 == 'r'/*right*/) return optoma.remote_Right();
				else if (v0 == 'd'/*down*/) return optoma.remote_Down();
				else if (val == F("keystone+")) return optoma.remote_KeystonePlus();
				else if (val == F("keystone-")) return optoma.remote_KeystoneMinus();
				else if (val == F("volume+")) return optoma.remote_VolumePlus();
				else if (val == F("volume-")) return optoma.remote_VolumeMinus();
				else if (v0 == 'b'/*brightness*/) return optoma.remote_Brightness();
				else if (v0 == 'm'/*menu*/) return optoma.remote_Menu();
				else if (v0 == 'z'/*zoom*/) return optoma.remote_Zoom();
				else if (v0 == 'c'/*contrast*/) return optoma.remote_Contrast();
#endif
				else return false;
	}


} _optomaParser;

void setup()
{

}

void loop()
{

  /* add main program code here */

}
