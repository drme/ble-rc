import Foundation

class FirmwareInformationViewController: UIViewController
{
	@IBOutlet var webView: UIWebView!;
	var logFile: String = "";

	override func viewDidLoad()
	{
		super.viewDidLoad()

		do
		{
			if (self.logFile.hasPrefix("https://"))
			{
				self.webView.loadRequest(NSURLRequest(URL: NSURL(string: self.logFile)!));
			}
			else
			{
				let htmlFile = NSBundle.mainBundle().pathForResource(self.logFile, ofType:"html")
				let htmlString = try NSString(contentsOfFile: htmlFile!, encoding: NSUTF8StringEncoding)

				self.webView.loadHTMLString(htmlString as String, baseURL:nil)
			}
		}
		catch
		{
		}
	}
}
