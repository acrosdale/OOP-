using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

using Foundation;
using UIKit;

namespace MyBooks.iOS
{
    [Register("AppDelegate")]
    public partial class AppDelegate : global::Xamarin.Forms.Platform.iOS.FormsApplicationDelegate
    {
        public override bool FinishedLaunching(UIApplication app, NSDictionary options)
        {
			//IOS specfic db path location/locator
			string fileName = "books_db.sqlite.";                                         //".." navigate back on folder then acces libary
            string fileLocation = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal),"..","Library");
			string full_Path = Path.Combine(fileLocation, fileName);

            global::Xamarin.Forms.Forms.Init();

            LoadApplication(new App(full_Path));

            return base.FinishedLaunching(app, options);
        }
    }
}
