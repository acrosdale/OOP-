using Xamarin.Forms;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Xamarin.Forms.Xaml;
using System.Text;

namespace MyBooks
{
    public partial class NewBookPage : ContentPage
    {
        public NewBookPage()
        {
            InitializeComponent();
        }

		void Save_isClicked(object sender, System.EventArgs e)
		{

            //create book and add value
            Book book = new Book()
            {
                Name = nameEntry.Text,

                Author =  authorEntry.Text
            
            };

			/**** THEIR CAN ONLY BE ONE CONNECTION @ A TIME TO SQL SO WE MUST CLOSE IT WHEN WE ARE DONE****/
			//one way is->>>>   conn.Dispose();
			// better way is VIA the using statement


			//save to DB

			
			using (SQLite.SQLiteConnection conn = new SQLite.SQLiteConnection(App.DB_PATH))//create and open connection
			{ 
               
                conn.CreateTable<Book>(); // create table of type book (Id,Name,Author)

                var NumRows = conn.Insert(book); //insert the object into the table
               

                if(NumRows > 0){
					DisplayAlert("Success","Book inserted sucessfully","Done");
				}
                else{
                    DisplayAlert("Failure", "Book insert Unsucessful", "Done");
                }
				//conn.Dispose();
			}
           

			
		}
    }
}
