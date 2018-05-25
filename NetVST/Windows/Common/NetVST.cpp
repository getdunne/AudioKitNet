#include "NetVST.h"
#include "NetVSTEditor.h"
#include "TRACE.h"

#define SAMPLE_RATE 44100
#define MAX_SAMPLE_PAIRS_PER_BLOCK 2048

void NetVST::Disconnect (void)
{
    server.Disconnect();
    fOn = 0.0f;
}

void NetVST::Connect (void)
{
    char ipAddr[16 + kVstMaxParamStrLen];
    memset(ipAddr, 0, 16);
    char* p = ipAddr;
    DisplayIndex(fIP1, 255, p); while (*p != 0) p++; *p++ = '.';
    DisplayIndex(fIP2, 255, p); while (*p != 0) p++; *p++ = '.';
    DisplayIndex(fIP3, 255, p); while (*p != 0) p++; *p++ = '.';
    DisplayIndex(fIP4, 255, p);

    int portNum = (int)((32 * fPort) + 27016);

    fOn = server.Connect(ipAddr, portNum) ? 1.0f : 0.0f;
    if (fOn) prevBlockBytes = 0;
}


NetVST::NetVST (audioMasterCallback audioMaster, VstInt32 numPrograms)
    : AudioEffectX (audioMaster, numPrograms, kNumParams)
    , fOn(0.0f)
    , fIP1(127/255.0f)
    , fIP2(0.0f)
    , fIP3(0.0f)
    , fIP4(1/255.0f)
    , fPort(0.0f)
    , fEffIdx(0.0f), fParamIdx(0.0f), fParamValue(0.0f)
    , midiCount(0)
    , paramCount(0)
{
    MySocketLib::Initialize();
    editor = new NetVSTEditor(this);
}

NetVST::~NetVST ()
{
    server.Disconnect();
    MySocketLib::Cleanup();
    delete (NetVSTEditor*)editor;
    editor = 0;
}

void NetVST::getParameterName (VstInt32 index, char* label)
{
	switch (index)
	{
		case kOn:   vst_strncpy (label, "On/Off", kVstMaxParamStrLen);  break;
		case kIP1:  vst_strncpy (label, "IP_1", kVstMaxParamStrLen);    break;
		case kIP2:  vst_strncpy (label, "IP_2", kVstMaxParamStrLen);    break;
		case kIP3:  vst_strncpy (label, "IP_3", kVstMaxParamStrLen);    break;
		case kIP4:  vst_strncpy (label, "IP_4", kVstMaxParamStrLen);    break;
        case kPort: vst_strncpy (label, "Port#", kVstMaxParamStrLen);   break;
	}
}

void NetVST::DisplayOnOff(float fPort, char* text)
{
    if (fPort < 0.5f)
        vst_strncpy(text, "OFF", kVstMaxParamStrLen);
    else
        vst_strncpy(text, "ON", kVstMaxParamStrLen);
}

void NetVST::DisplayIndex(float fFraction, int max, char* text)
{
    float fIndex = fFraction * max;
    if (fIndex < 1.0f) vst_strncpy(text, "0", kVstMaxParamStrLen);
    else int2string((VstInt32)(fIndex), text, kVstMaxParamStrLen);
}

void NetVST::DisplayFloat(float fValue, char* text)
{
    float2string(fValue, text, kVstMaxParamStrLen);
}

void NetVST::DisplayPort(float fPort, char* text)
{
    int2string((VstInt32)(32 * fPort) + 27016, text, kVstMaxParamStrLen);
}

void NetVST::getParameterDisplay (VstInt32 index, char* text)
{
	text[0] = 0;
	switch (index)
	{
        case kOn:       DisplayOnOff(fOn, text);        break;
	    case kIP1:      DisplayIndex(fIP1, 255, text);  break;
	    case kIP2:      DisplayIndex(fIP2, 255, text);  break;
	    case kIP3:      DisplayIndex(fIP3, 255, text);  break;
	    case kIP4:      DisplayIndex(fIP4, 255, text);  break;
	    case kPort:     DisplayPort(fPort, text);       break;
	}
}

void NetVST::setParameter (VstInt32 index, float value)
{
	switch (index)
	{
		case kOn:       fOn = value;                    break;
		case kIP1:      fIP1 = value;                   break;
		case kIP2:      fIP2 = value;                   break;
		case kIP3:      fIP3 = value;                   break;
		case kIP4:      fIP4 = value;                   break;
        case kPort:     fPort = value;                  break;
	}
    if (index == kOn && fOn >= 0.5f) Connect();
    else if (index <= kPort) Disconnect();

    if (editor) ((NetVSTEditor*)editor)->setParameter (index, value);
}

float NetVST::getParameter (VstInt32 index)
{
	float value = 0;
	switch (index)
	{
		case kOn:       value = fOn;                        break;
		case kIP1:      value = fIP1;                       break;
		case kIP2:      value = fIP2;                       break;
		case kIP3:      value = fIP3;                       break;
		case kIP4:      value = fIP4;                       break;
        case kPort:     value = fPort;                      break;
	}
	return value;
}

VstInt32 NetVST::processEvents (VstEvents* ev)
{
	if (Connected()) for (VstInt32 i = 0; i < ev->numEvents; i++)
	{
		if ((ev->events[i])->type != kVstMidiType)
			continue;

		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];

        char* md = event->midiData;
        midiData[midiCount].channel = md[0] & 0x0F;
        midiData[midiCount].status = md[0] & 0xF0;
        midiData[midiCount].data1 = md[1];
        midiData[midiCount].data2 = md[2];

        if (midiCount < MAX_MIDI_EVENTS-1)
            midiData[midiCount++].startFrame = event->deltaFrames;
	}
	return 1;
}
