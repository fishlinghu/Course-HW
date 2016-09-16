import java.rmi.*;

public interface MeterInterface extends Remote 
    {
    // Methods must throw RemoteException.
    //public void initializeMeter(String name, String location) throws RemoteException;
    public void listAllCompany() throws RemoteException;
    public boolean registerCompany(String new_company_name) throws RemoteException;
    public void unregisterCompany() throws RemoteException;
    public String getServerLocation() throws RemoteException;
    public void receiveCommand(String command) throws RemoteException;
    public void sendAlert() throws RemoteException;
    public void sendReading() throws RemoteException;
    public void setReading() throws RemoteException;
    public double sendReadingToBroker() throws RemoteException;
    public void requestDeal() throws RemoteException;
    public void acceptDeal() throws RemoteException;
    }