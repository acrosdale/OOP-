using System;
using Xamarin.Forms;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Xamarin.Forms.Xaml;
using System.Text;
using SQLite;

namespace MyBooks
{
    public class Book
    {
      
        [PrimaryKey, AutoIncrement] public int Id { get; set; }

        public string Name{ get; set; }
        public string Author { get; set; }

		
	}
}
