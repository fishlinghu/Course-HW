import java.rmi.*;
import java.util.Scanner;

public class MeterServer
    {
    public static void main(String[] args)
        {
        try
            {
            int choice;
            char your_answer = ' ';
            String meter_name, meter_location, meter_name_no_space;
            String company_name, company_location;

            Meter meter = new Meter();
            Scanner scanner = new Scanner(System.in);

            BrokerInterface broker = (BrokerInterface)
            Naming.lookup("rmi://localhost/broker");
            // Modify the url here

            while(true)
                {
                System.out.println("");
                System.out.println("What name do you want to use for your meter?");
                while((meter_name = scanner.nextLine()).equals(""));
                if(broker.isThereSuchMeter(meter_name))
                    {
                    // The meter name already exists
                    System.out.println("The meter name already exists.");
                    System.out.println("Are you the same meter that just recover from a process failure?");
                    System.out.println("If not, please use another name. ");
                    System.out.printf("Same meter? (Y/N): ");
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
                        meter.meter_name = meter_name;
                        meter_name_no_space = meter_name.replaceAll("\\s+","");
                        meter.meter_location = "rmi://localhost/" + meter_name_no_space;
                        // Modify the url here
                        meter.retrieveData();
                        Naming.rebind(meter_name_no_space, meter);
                        System.out.println("Meter--" + meter_name + " restart!!");
                        break;
                        }
                    }
                else
                    {
                    // Okay you can use it
                    meter.meter_name = meter_name;
                    meter_name_no_space = meter_name.replaceAll("\\s+","");
                    meter.meter_location = "rmi://localhost/" + meter_name_no_space;
                    // Modify the url here
                    Naming.rebind(meter_name_no_space, meter);
                    broker.addMeter(meter_name);
                    System.out.println("Meter--" + meter_name + " ready!!");
                    break;
                    }
                }

            while(true)
                {
                System.out.println("");
                System.out.println("What do you want to do?");
                System.out.println("1. Register with a new company. ");
                System.out.println("2. Unregister with the company. ");
                System.out.println("3. Request for a better deal. ");
                System.out.println("4. Accept the provided deal. ");
                System.out.println("5. Send alert to the company. ");
                System.out.println("6. Send reading to the company. ");
                System.out.println("7. Set reading value. ");
                System.out.println("8. Which company is the meter registered to? ");
                choice = scanner.nextInt();
                company_name = scanner.nextLine();
                switch(choice)
                    {
                    case 1:
                        System.out.println("Following are all the companies: ");
                        meter.listAllCompany();

                        System.out.printf("Which company do you want to register with? Enter the name: ");
                        company_name = scanner.nextLine();
                        if(meter.registerCompany(company_name))
                            System.out.println("Register successfully!");
                        else
                            System.out.println("Error occurs when trying to register with a new company");
                        break;
                    case 2:
                        meter.unregisterCompany();
                        break;
                    case 3:
                        meter.requestDeal();
                        break;
                    case 4:
                        meter.acceptDeal();
                        break;
                    case 5:
                        meter.sendAlert();
                        break;
                    case 6:
                        meter.sendReading();
                        break;
                    case 7:
                        meter.setReading();
                        break;
                    case 8:
                        System.out.printf("%s from %s. \n", meter.company_name, meter.company_location);
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
