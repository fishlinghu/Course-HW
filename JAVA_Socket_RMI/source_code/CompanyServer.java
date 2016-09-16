import java.net.*;
import java.rmi.*;
import java.lang.Character;
import java.rmi.server.*;
import java.util.Scanner;


public class CompanyServer
    {
    public static void main(String[] args)
        {
        try
            {
            int choice;
            char your_answer = ' ';
            String company_name = "";
            String company_name_no_space = "";

            Scanner scanner = new Scanner(System.in);
            Company company = new Company();

            // Link to the already known broker
            BrokerInterface broker = (BrokerInterface)
            Naming.lookup("rmi://localhost/broker");
            // Modify the url here

            while(true)
                {
                System.out.println("");
                System.out.println("What name do you want to use for your company?");
                while((company_name = scanner.nextLine()).equals(""));
                if(broker.isThereSuchCompany(company_name))
                    {
                    // The company name already exists
                    System.out.println("The company name already exists.");
                    System.out.println("Are you the same company that just recover from a process failure?");
                    System.out.println("If not, please use another name. ");
                    System.out.printf("Same company? (Y/N): ");
                    try
                        {
                        your_answer = (char) System.in.read();
                        }
                    catch(Exception e)
                        {
                        e.printStackTrace();
                        }
                    your_answer = Character.toLowerCase(your_answer);
                    if(your_answer == 'y')
                        {
                        // Just recover from process failure
                        company.company_name = company_name;
                        company_name_no_space = company_name.replaceAll("\\s+","");
                        company.company_location = "rmi://localhost/" + company_name_no_space;
                        // Modify the url here
                        Naming.rebind(company_name_no_space, company);
                        // Regain some information
                        company.retrieveData();
                        System.out.println("Company--" + company_name + " restart!!");
                        break;
                        }
                    }
                else
                    {
                    // Okay you can use it
                    company.company_name = company_name;
                    company_name_no_space = company_name.replaceAll("\\s+","");
                    company.company_location = "rmi://localhost/" + company_name_no_space;
                    // Modify the url here
                    Naming.rebind(company_name_no_space, company);
                    broker.addCompany(company_name);
                    System.out.println("Company--" + company_name + " start!!");
                    break;
                    }
                }
            // Register the remote object.

            while(true)
                {
                System.out.println("");
                System.out.println("What do you want to do?");
                System.out.println("1. Set a tariff. ");
                System.out.println("2. Send command to a meter. ");
                System.out.println("3. List all registered meters. ");
                choice = scanner.nextInt();
                switch(choice)
                    {
                    case 1:
                        company.setTariff();
                        break;
                    case 2:
                        company.sendCommand();
                        break;
                    case 3:
                        company.listAllRegisteredMeter();
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
