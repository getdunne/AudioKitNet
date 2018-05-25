#import <Cocoa/Cocoa.h>
#import <AudioUnit/AUCocoaUIView.h>

@class NetFilter_CocoaView;

@interface NetFilter_CocoaViewFactory : NSObject <AUCocoaUIBase>
{
    IBOutlet NetFilter_CocoaView *	uiFreshlyLoadedView;
}

- (NSString *) description;	// string description of the view

@end