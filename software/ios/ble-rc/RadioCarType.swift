import Foundation;

enum RadioCarType: UInt8, CustomStringConvertible
{
	case Unknown = 0xff;
	case MiniZ = 0;
	case MiniZBLDC = 1;
	case DNano = 2;
	case Simple = 3;

	var description: String
	{
		switch (self)
		{
			case .MiniZ:
				return "Mini-Z";
			case .MiniZBLDC:
				return "Mini-Z Brushless";
			case .DNano:
				return "dNano";
			case .Simple:
				return "Simple";
			default:
				return "Unknown";
		}
	}
}
