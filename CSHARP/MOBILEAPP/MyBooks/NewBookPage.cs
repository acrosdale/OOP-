using System;

using Xamarin.Forms;

namespace MyBooks
{
    public class NewBookPage : ContentPage
    {
        public NewBookPage()
        {
            Content = new StackLayout
            {
                Children = {
                    new Label { Text = "Hello ContentPage" }
                }
            };
        }
    }
}

