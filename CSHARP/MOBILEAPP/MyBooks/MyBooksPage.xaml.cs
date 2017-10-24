using Xamarin.Forms;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Xamarin.Forms.Xaml;
using System.Text;


namespace MyBooks
{
    public partial class MyBooksPage : ContentPage
    {
        public MyBooksPage()
        {
            InitializeComponent();
        }

        protected override void OnAppearing(){

            base.OnAppearing();
           
            using (SQLite.SQLiteConnection conn = new SQLite.SQLiteConnection(App.DB_PATH)){ //create and open db conn

                conn.CreateTable<Book>(); //create table

                var books = conn.Table<Book>().ToList(); //store a list of book from DB 

                BookList.ItemsSource = books; //in myBookPage display display the list of book
            }
        }


        void Add_Activated (object sender, System.EventArgs e)
        {
           Navigation.PushAsync(new NewBookPage());

        }

         void Select_Activated(object sender, System.EventArgs e)
        {
           
            using (SQLite.SQLiteConnection conn = new SQLite.SQLiteConnection(App.DB_PATH))//create and open connection
            { 
               

                var NumRows = conn.DeleteAll<Book>(); //insert the object into the table


                if (NumRows > 0)
                {
                    DisplayAlert("Success", "Books deleted sucessfully", "Done");
                }
                else
                {
                    DisplayAlert("Failure", "Book deleted Unsucessful", "Done");
                }
                OnAppearing();
            }
           


        }
    }
}


