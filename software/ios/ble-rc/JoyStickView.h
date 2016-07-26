#import <UIKit/UIKit.h>

@interface JoyStickView : UIView

@property (assign) bool vertical;
@property (assign) bool enabled;

-(void)setDelegate:(id)delegate;

@end

@protocol JoyStickDelegate

-(void)onStickChanged:(JoyStickView*)sender :(float)pos;

@end
