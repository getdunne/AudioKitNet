#import <Cocoa/Cocoa.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>

@interface NetFilter_CocoaView : NSView
{
    // IB Members
	IBOutlet NSTextField *uiPrimaryLabel;
    IBOutlet NSTextField *uiIP1TextField;
    IBOutlet NSTextField *uiIP2TextField;
    IBOutlet NSTextField *uiIP3TextField;
    IBOutlet NSTextField *uiIP4TextField;
    IBOutlet NSTextField *uiPortTextField;
	IBOutlet NSButton *uiEnableCheckbox;
	
    // Other Members
    AudioUnit 				mAU;
    AUParameterListenerRef	mParameterListener;
	
	// Shadow parameter values
	int ip1, ip2, ip3, ip4, portNum;
	BOOL on;
}

#pragma mark ____ PUBLIC FUNCTIONS ____
- (void)setAU:(AudioUnit)inAU;

#pragma mark ____ INTERFACE ACTIONS ____
- (IBAction)iaRevertClicked:(id)sender;
- (IBAction)iaUpdateClicked:(id)sender;
- (IBAction)iaEnableClicked:(id)sender;

#pragma mark ____ PRIVATE FUNCTIONS
- (void)_synchronizeUIWithParameterValues;
- (void)_addListeners;
- (void)_removeListeners;

#pragma mark ____ LISTENER CALLBACK DISPATCHEE ____
- (void)_parameterListener:(void *)inObject parameter:(const AudioUnitParameter *)inParameter value:(Float32)inValue;

#pragma mark ____ HELPERS ____
- (void)_updateDisplayedAddrPort;
@end
