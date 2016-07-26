import Foundation

class FirmwareFile : NSObject
{
	var fileName : String = ""
	var displayName : String = ""
	var imageType : UInt16 = 0
	var hardwareType : String = ""
}


private let kHexChars = Array("0123456789abcdef".utf8) as [UInt8];

extension NSData {

	var hexString : String {
		guard length > 0 else {
			return ""
		}

		let buffer = UnsafeBufferPointer<UInt8>(start: UnsafePointer(bytes), count: length)
		var output = [UInt8](count: length*2 + 1, repeatedValue: 0)
		var i: Int = 0
		for b in buffer {
			let h = Int((b & 0xf0) >> 4)
			let l = Int(b & 0x0f)
			output[i++] = kHexChars[h]
			output[i++] = kHexChars[l]
		}

		return String.fromCString(UnsafePointer(output))!	}
}
