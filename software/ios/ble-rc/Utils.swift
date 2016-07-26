import Foundation

@objc public class UtilsEx: NSObject
{
	static let hexChars = Array("0123456789abcdef".utf8) as [UInt8];

	class func formatFloat(value: Float, suffix: String) -> String
	{
		let v = Int(value * 10.0);
		let n = v / 10;
		let r = v % 10;

		return String(n) + "." + String(r) + suffix;
	}

	public class func imageWithImage(name: String, fixedHue hue: CGFloat, alpha: CGFloat) -> UIImage
	{
		let source: UIImage! = UIImage(named: name);
		let imageSize = source!.size;
		let imageExtent: CGRect = CGRectMake(0, 0, imageSize.width, imageSize.height);

		UIGraphicsBeginImageContext(imageSize);

		let context = UIGraphicsGetCurrentContext();

		source.drawAtPoint(CGPointMake(0, 0));

		CGContextSaveGState(context);
		CGContextTranslateCTM(context, 0, source.size.height);
		CGContextScaleCTM(context, 1.0, -1.0);
		CGContextClipToMask(context, imageExtent, source.CGImage);
		CGContextFillRect(context, imageExtent);

		CGContextDrawImage(context, imageExtent, source.CGImage);
		CGContextSetBlendMode(context, .Hue);
		UIColor(hue: hue, saturation: 1.0, brightness: 1, alpha: alpha).set();

		let imagePath: UIBezierPath = UIBezierPath(rect: imageExtent);
		imagePath.fill();

		CGContextRestoreGState(context);

		let result: UIImage = UIGraphicsGetImageFromCurrentImageContext();

		UIGraphicsEndImageContext();

		return result;
	}

	class func saveValue(key: String, value: Bool)
	{
		let userDefaults = NSUserDefaults.standardUserDefaults();

		if (value != userDefaults.boolForKey(key))
		{
			userDefaults.setBool(value, forKey: key);

			userDefaults.synchronize();
		}
	}
}

extension NSData
{
	var hexString : String
	{
		guard self.length > 0 else
		{
			return ""
		}

		let buffer = UnsafeBufferPointer<UInt8>(start: UnsafePointer(self.bytes), count: self.length);
		var output = [UInt8](count: length * 2 + 1, repeatedValue: 0);
		var i: Int = 0;

		for b in buffer
		{
			let h = Int((b & 0xf0) >> 4);
			let l = Int(b & 0x0f);

			output[i++] = UtilsEx.hexChars[h];
			output[i++] = UtilsEx.hexChars[l];
		}

		return String.fromCString(UnsafePointer(output))!;
	}
}
