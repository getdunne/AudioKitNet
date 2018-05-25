#import <Cocoa/Cocoa.h>
#import <AudioUnit/AUCocoaUIView.h>

@class NetSynth_CocoaView;

@interface NetSynth_CocoaViewFactory : NSObject <AUCocoaUIBase>
{
    IBOutlet NetSynth_CocoaView *	uiFreshlyLoadedView;
}

- (NSString *) description;	// string description of the view

@end