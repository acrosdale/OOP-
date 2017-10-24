import java.sql.*;
import java.util.Scanner;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
public class Driver {

	static final String DB = "jdbc:sqlite://Users/ALEX-C/Desktop/workspace/JDBC/bin/TPC_H.db";
	static String SQL      = null; // use for query construction
	static ResultSet rs;
	static Scanner input   = new Scanner(System.in);
	static Connection conn = null;
	static Statement stat  = null;
	static final String JDBC_DRIVER = "org.sqlite.JDBC";



	public static void main(String[] args) {
		System.out.print("CHOOSE AN OPTION\n");

		try{
			//connect to DB
			Class.forName(JDBC_DRIVER);

			System.out.println("(1)Connect to TPCH\n(2)Create Table\n(3)Insert Data\n(4)Find the supplier with the smallest number of warehouses\n(5)Find the maximum warehouse capacity across all tables\n" +
			"(6)List all warehouses in Europe with capacity smaller than what you input\n(7)Whether warehouses can fit products given supplier name\n(8)Ascending order of capacity of warhouse given by user\n" +
			"(9)Update warehouase table\n(10)Disconnect");

			int user_in =0;

			while (user_in != 10){
				user_in = input.nextInt();
				switch(user_in){
					case 1:
						option1();
						break;
					case 2:
						option2();
						break;
					case 3:
						option3();
						break;
					case 4:
						option4();
						break;
					case 5:
						option5();
						break;
					case 6:
						option6();
						break;
					case 7:
						option7();
						break;
					case 8:
						option8();
						break;
					case 9:
						option9();
						break;
					case 10:
						break;
					default:
						System.out.println("Please input a valid menu option");
						break;
				}

			}

			//clean up environment
			System.out.println("Disconnecting from TPC_H");

			if(rs != null){
				rs.close();
			}
      		stat.close();
      		conn.close();
			System.out.println("GOODBYE");


		}

		catch(Exception exe){
			exe.printStackTrace();
		}


	}


	public static void option1(){
		System.out.println("Connecting to database...");
		try {
			conn = DriverManager.getConnection(DB);
			stat = conn.createStatement();
		}
		catch (SQLException except) {
			// TODO Auto-generated catch block
			except.printStackTrace();
		}

		System.out.println("You are connected to TPC_H");
	}


	public static void option2(){
		try {
			int choice;
			stat = conn.createStatement();
			System.out.println("warehouse already exist. Do you want to create a fresh warehouse table(1). keep current table(0) ");
			choice = input.nextInt();

			if(choice ==0){
				return;
			}

			stat.executeUpdate("drop table if exists warehouse;");
		    System.out.println("Creating database table...");

		    SQL = "CREATE TABLE warehouse(" +
		      	"w_warehousekey INTEGER PRIMARY KEY AUTOINCREMENT," +
		      	"w_name char(25) not null," +
		      	"w_supplierkey decimal(2,0) not null," +
		      	"w_capacity decimal(6,2) not null," +
		      	"w_address varchar(40) not null," +
		      	"w_nationkey decimal(2,0) not null);";

		    stat.executeUpdate(SQL);

		    System.out.println("Database table created successfully...");
		}

		catch (SQLException except) {
			// TODO Auto-generated catch block
			except.printStackTrace();
		}
	}

	public static void option3(){

		try{
            System.out.println("Enter Warehouse name:");
            String w_name = "'"+input.next()+"'";

            System.out.println("Enter Capacity:");
            int w_capacity = input.nextInt();

            BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
            System.out.println("Enter the Address:");
            String w_address = "'"+br.readLine()+"'";

            int supp_key=0,n_key=0;
            boolean n_in=false, supp_in=false;

            while(!(n_in==true && supp_in==true)){

				System.out.println("press (1) to Input Nation \npress (2) Input Supplier");

            	switch(input.nextInt()){
                    case 1:
                      	System.out.print("Input the nation\n");
                      	String nation = input.next();

						if(!(nation.charAt(0) >='A' && nation.charAt(0)<='Z')){///the nation key

                            n_key=Integer.valueOf(input.match().group());
                            n_in=true;

                        }else{// nation name
                            String n_name="'"+input.match().group()+"'";
							rs = stat.executeQuery("SELECT n_nationkey FROM nation WHERE n_name = "+n_name+";");

							while(rs.next())
							{
								n_key = rs.getInt("n_nationkey");
							}

                            n_in=true;
                        }

						break;
                    case 2:
                      	System.out.print("Input the Supplier\n");

                      	String supplier = input.next();

                    	if(!(supplier.charAt(0)>='A' && supplier.charAt(0)<='Z')){ //if you enter the suppkey number

                            supp_key=Integer.valueOf(input.match().group());

                            supp_in=true;
                    	}
						else{// if you enter their name   Supplier\#000000005

                            String sname = "'"+input.match().group()+"'";
                        	rs = stat.executeQuery("SELECT s_suppkey FROM supplier WHERE s_name ="+ sname + ";");

							while(rs.next())
							{
								supp_key = rs.getInt("s_suppkey");
							}

                            supp_in=true;
                        }
                        break;
                }
            }

            SQL = "INSERT INTO warehouse(w_name,w_supplierkey,w_capacity,w_address,w_nationkey)" +
                "VALUES("+w_name+","+supp_key+","+w_capacity+","+w_address+","+n_key+");";

            stat.executeUpdate(SQL);
            System.out.println("DATA INSERTED");


	    }
		catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public static void option4() throws SQLException{
			System.out.println("Supplier with smallest number of warehouses:");

            rs = stat.executeQuery("SELECT s_name "+
          		"FROM supplier,(SELECT w_supplierkey,COUNT(w_name) FROM warehouse GROUP BY w_supplierkey ORDER BY COUNT(w_name) DESC LIMIT 1)a "+
          		"WHERE s_suppkey = a.w_supplierkey;");

           while(rs.next()){
               System.out.println(rs.getString("s_name"));
           }

	}

	public static void option5() throws SQLException{
		System.out.println("Maximum warehouse capacity across all the suppliers:");

		rs = stat.executeQuery("SELECT s_name, Max(w_capacity) FROM warehouse, supplier WHERE s_suppkey = w_supplierkey GROUP BY s_name;");

		while(rs.next()){
			System.out.println("MAX Capacity: "+rs.getString("MAX(w_capacity)"));
			System.out.println("Supplier Name: "+rs.getString("s_name"));
		}
	}

	public static void option6() throws SQLException{
		//List all the warehouses in EUROPE with capacity smaller than X, where X is taken as an input from the user

		System.out.println("Please input a capacity for warehouses in Europe");

        int X = input.nextInt();
        rs = stat.executeQuery("SELECT w_name FROM warehouse,nation WHERE w_nationkey = n_nationkey AND n_regionkey=3 AND w_capacity < "+ X +";");

        while(rs.next()){
            System.out.println("Warehouse Name = " + rs.getString("w_name"));
        }


	}

	public static void option7() throws SQLException{
		int w_capacity=0;
        int s_capacity=0;
        System.out.println("Please input a supplier name to check capacity:");
			//For a supplier name given by the user, ﬁnd whether all its warehouses are capable to ﬁt all its products (see ps partsupp).

        String s_name = "'"+input.next()+"'";
        rs = stat.executeQuery("SELECT SUM(w_capacity), SUM(ps_availqty) FROM warehouse,supplier,partsupp WHERE s_suppkey = w_supplierkey AND s_name="+ s_name +" AND ps_suppkey = w_supplierkey");

		while(rs.next()){
            w_capacity = rs.getInt("SUM(w_capacity)");
            s_capacity = rs.getInt("SUM(ps_availqty)");
        }

        if(w_capacity < s_capacity){

        	System.out.println("warehouse capacity can't support the parts");

        }
		else{

			System.out.println("The warehouse can support the parts");

		}

	}
	//For a nation given by the user, print all the warehouses in that country, in ascending order of their capacity.

	public static void option8() {
			String n_name;
		try {
			System.out.println("Please input a nation name :");
			n_name = "'"+input.next()+"'";

            rs = stat.executeQuery("SELECT w_name FROM warehouse,nation WHERE w_nationkey = n_nationkey AND n_name="+ n_name+" ORDER BY w_capacity ASC");

			while(rs.next()){

				System.out.print(rs.getString("w_name"));

			}

		}
		catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public static void option9() throws SQLException{

		//Supplier#000000002 is acquired by Supplier#000000001. Update the warehouse table to reﬂect this change in ownership.
		//The actual names of the suppliers are taken as input from the user, they are not constants.

		String sup1 = "";
		String sup2 = "";
		System.out.print("Please input the names of suppliers to be taken over:\n");
		sup1 = "'"+input.next()+"'";

		sup2 = "'"+input.next()+"'";

		sup1 = sup1.substring(17,19);
		sup2 = sup2.substring(17,19);

		PreparedStatement ms = conn.prepareStatement("UPDATE warehouse SET w_supplierkey = "+sup1+" WHERE w_supplierkey = "+sup2+";");
		ms.executeUpdate();
		ms.close();
		System.out.println("DATA UPDATED");

	}





}
