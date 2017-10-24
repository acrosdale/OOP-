using Xamarin.Forms;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Xamarin.Forms.Xaml;
using System.Text;

namespace MyBooks
{
    public partial class App : Application
    {
        public static string DB_PATH = String.Empty;
        public App()
        {
            InitializeComponent();

             //main page
            //MainPage = new MyBooksPage();

            //allow navigation from one page to another. MyBookpage is the set as first one on stack
            MainPage = new NavigationPage(new MyBooksPage());
        }


		public App(string db_path) //this construction help with db connection
		{
			InitializeComponent();

            // reason for this is that ios/android have different path to the DB/ file local
            DB_PATH = db_path;

			MainPage = new NavigationPage(new MyBooksPage());

		}

        protected override void OnStart()
        {
            // Handle when your app starts
        }

        protected override void OnSleep()
        {
            // Handle when your app sleeps
        }

        protected override void OnResume()
        {
            // Handle when your app resumes
        }
    }
}
