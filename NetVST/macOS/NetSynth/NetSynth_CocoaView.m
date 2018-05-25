#import "NetSynth_CocoaView.h"

enum {
    kParam_IP1=0,
    kParam_IP2,
    kParam_IP3,
    kParam_IP4,
    kParam_PortNum,
    kParam_On,
	//kParam_EffectIdx,
	//kParam_ParamIdx,
	//kParam_ParamValue,
    
    kNumberOfParameters
};

#pragma mark ____ LISTENER CALLBACK DISPATCHER ____
AudioUnitParameter parameter[] = {
	{ 0, kParam_IP1, kAudioUnitScope_Global, 0 },
	{ 0, kParam_IP2, kAudioUnitScope_Global, 0 },
	{ 0, kParam_IP3, kAudioUnitScope_Global, 0 },
	{ 0, kParam_IP4, kAudioUnitScope_Global, 0 },
	{ 0, kParam_PortNum, kAudioUnitScope_Global, 0 },
	{ 0, kParam_On, kAudioUnitScope_Global, 0 },
};

void ParameterListenerDispatcher (void *inRefCon, void *inObject, const AudioUnitParameter *inParameter, Float32 inValue) {
	NetSynth_CocoaView *SELF = (NetSynth_CocoaView *)inRefCon;
    
    [SELF _parameterListener:inObject parameter:inParameter value:inValue];
}

@implementation NetSynth_CocoaView
#pragma mark ____ (INIT /) DEALLOC ____
- (void)dealloc {
    [self _removeListeners];
    [super dealloc];
}

#pragma mark ____ PUBLIC FUNCTIONS ____
- (void)setAU:(AudioUnit)inAU {
	// remove previous listeners
	if (mAU) [self _removeListeners];
	mAU = inAU;
    
	// add new listeners
	[self _addListeners];
	
	// initial setup
	[self _synchronizeUIWithParameterValues];
}

#pragma mark ____ INTERFACE ACTIONS ____
- (IBAction)iaRevertClicked:(id)sender {
	[self _synchronizeUIWithParameterValues];
	[self _updateDisplayedAddrPort];
	[uiIP1TextField setStringValue:[NSString stringWithFormat:@"%d", ip1]];
	[uiIP2TextField setStringValue:[NSString stringWithFormat:@"%d", ip2]];
	[uiIP3TextField setStringValue:[NSString stringWithFormat:@"%d", ip3]];
	[uiIP4TextField setStringValue:[NSString stringWithFormat:@"%d", ip4]];
	[uiPortTextField setStringValue:[NSString stringWithFormat:@"%d", portNum]];
}

static BOOL checkIP(NSTextField* textField, int* pOut)
{
	int ival;
	if (![[NSScanner scannerWithString:[textField stringValue]] scanInt:&ival]) return NO;
	if (ival < 0 || ival > 255) return NO;
	*pOut = ival;
	return YES;
}

static BOOL checkPort(NSTextField* textField, int* pOut)
{
	int ival;
	if (![[NSScanner scannerWithString:[textField stringValue]] scanInt:&ival]) return NO;
	if (ival < 1024 || ival > 65535) return NO;
	*pOut = ival;
	return YES;
}

- (IBAction)iaUpdateClicked:(id)sender {
	if (!checkIP(uiIP1TextField, &ip1) ||
		!checkIP(uiIP2TextField, &ip2) ||
		!checkIP(uiIP3TextField, &ip3) ||
		!checkIP(uiIP4TextField, &ip4)) {
		[uiPrimaryLabel setStringValue:@"IP numbers must be 0-255"];
	} else if (!checkPort(uiPortTextField, &portNum)) {
		[uiPrimaryLabel setStringValue:@"Port numbers must be 1024-65535"];
	} else {
		// Always disable network first
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[kParam_On], 0.0, 0) == noErr,
				  @"[CocoaView iaUpdateClicked] (on.a)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[kParam_On]) == noErr,
				  @"[CocoaView iaUpdateClicked] (on.b)");
		// Set IP address
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[kParam_IP1], ip1, 0) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip1.a)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[kParam_IP1]) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip1.b)");
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[kParam_IP2], ip2, 0) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip2.a)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[kParam_IP2]) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip2.b)");
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[kParam_IP3], ip3, 0) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip3.a)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[kParam_IP3]) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip3.b)");
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[kParam_IP4], ip4, 0) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip4.a)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[kParam_IP4]) == noErr,
				  @"[CocoaView iaUpdateClicked] (ip4.b)");
		// Set port number
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[kParam_PortNum], portNum, 0) == noErr,
				  @"[CocoaView iaUpdateClicked] (portNum.a)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[kParam_PortNum]) == noErr,
				  @"[CocoaView iaUpdateClicked] (portNum.b)");
		// Update main label to confirm
		[self _updateDisplayedAddrPort];
	}
}

- (IBAction)iaEnableClicked:(id)sender {
    BOOL checked = [sender state] == NSOnState;
	NSAssert(	AUParameterSet(mParameterListener, sender, &parameter[5], checked ? 1.0 : 0.0, 0) == noErr,
                @"[CocoaView iaEnableClicked:] AUParameterSet()");
}


#pragma mark ____ PRIVATE FUNCTIONS ____
- (void)_addListeners {
	NSAssert (	AUListenerCreate(	ParameterListenerDispatcher, self, 
                                    CFRunLoopGetCurrent(), kCFRunLoopDefaultMode, 0.100, // 100 ms
                                    &mParameterListener	) == noErr,
                @"[CocoaView _addListeners] AUListenerCreate()");
	
    int i;
    for (i = 0; i < kNumberOfParameters; ++i) {
        parameter[i].mAudioUnit = mAU;
        NSAssert (	AUListenerAddParameter (mParameterListener, NULL, &parameter[i]) == noErr,
                    @"[CocoaView _addListeners] AUListenerAddParameter()");
    }
}

- (void)_removeListeners {
    int i;
    for (i = 0; i < kNumberOfParameters; ++i) {
        NSAssert (	AUListenerRemoveParameter(mParameterListener, NULL, &parameter[i]) == noErr,
                    @"[CocoaView _removeListeners] AUListenerRemoveParameter()");
    }
    
	NSAssert (	AUListenerDispose(mParameterListener) == noErr,
                @"[CocoaView _removeListeners] AUListenerDispose()");
}

- (void)_synchronizeUIWithParameterValues {
	Float32 value;
    int i;
    
    for (i = 0; i < kNumberOfParameters; ++i) {
        // only has global parameters
        NSAssert (	AudioUnitGetParameter(mAU, parameter[i].mParameterID, kAudioUnitScope_Global, 0, &value) == noErr,
                    @"[CocoaView synchronizeUIWithParameterValues] (x.1)");
        NSAssert (	AUParameterSet (mParameterListener, self, &parameter[i], value, 0) == noErr,
                    @"[CocoaView synchronizeUIWithParameterValues] (x.2)");
        NSAssert (	AUParameterListenerNotify (mParameterListener, self, &parameter[i]) == noErr,
                    @"[CocoaView synchronizeUIWithParameterValues] (x.3)");
    }
}

#pragma mark ____ LISTENER CALLBACK DISPATCHEE ____
- (void)_parameterListener:(void *)inObject parameter:(const AudioUnitParameter *)inParameter value:(Float32)inValue {
    //inObject ignored in this case.
    
	switch (inParameter->mParameterID) {
		case kParam_IP1:
			ip1 = (int)inValue;
			[self _updateDisplayedAddrPort];
			break;
			
		case kParam_IP2:
			ip2 = (int)inValue;
			[self _updateDisplayedAddrPort];
			break;
			
		case kParam_IP3:
			ip3 = (int)inValue;
			[self _updateDisplayedAddrPort];
			break;
			
		case kParam_IP4:
			ip4 = (int)inValue;
			[self _updateDisplayedAddrPort];
			break;
			
		case kParam_PortNum:
			portNum = (int)inValue;
			[self _updateDisplayedAddrPort];
			break;
			
		case kParam_On:
			on = inValue > 0.5;
			[uiEnableCheckbox setState:(on ? NSOnState : NSOffState)];
			break;
	}
}

#pragma mark ____ HELPERS ____
- (void)_updateDisplayedAddrPort {
	[uiPrimaryLabel setStringValue:[NSString stringWithFormat:@"NetSynth  %d.%d.%d.%d:%d", ip1, ip2, ip3, ip4, portNum]];
}

@end
