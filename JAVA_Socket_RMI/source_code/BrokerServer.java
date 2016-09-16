import java.rmi.*;
import java.util.Scanner;
 
public class BrokerServer 
    {
    public static void main(String[] args) 
        {
        try
            {
            Scanner scanner = new Scanner(System.in);
            Broker broker = new Broker();

            // Register the remote object.
            Naming.rebind("broker", broker);
            System.out.println("broker ready");

            //MeterInterface meter = (MeterInterface)
            //Naming.lookup("rmi://localhost/meter");

            int choice;
            int temp;
            while(true)
                {
                System.out.println("");
                System.out.println("What do you want to do?");
                System.out.println("1. List all the companies. ");
                System.out.println("2. List all the meters. ");
                choice = scanner.nextInt();
                switch(choice) 
                    {
                    case 1:
                        broker.listAllCompany();
                        break;
                    case 2:
                        broker.listAllMeter();
                        break;
                    default:
                        System.out.println("No such option!!");
                    }
                }
            } 
        catch(Exception e) 
            {
            e.printStackTrace();
            }
        }
    }
