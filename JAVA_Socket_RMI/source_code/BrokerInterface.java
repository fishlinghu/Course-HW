import java.rmi.*;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap; 
import java.util.Iterator; 
import java.util.Map;

public interface BrokerInterface extends Remote 
    {
    // Methods must throw RemoteException.
    public void addCompany(String company_name) throws RemoteException;
    public void addMeter(String meter_name) throws RemoteException;

    public String getCompanyLocation(String company_name) throws RemoteException;
    public String getMeterLocation(String meter_name) throws RemoteException;
    
    public void listAllCompany() throws RemoteException;
    public void listAllMeter() throws RemoteException;
    
    public ArrayList getCompanyNameList() throws RemoteException;
    public Map<String, String> getRegisteredMeterList(String company_name) throws RemoteException;
    public void getCompanyTariff() throws RemoteException;
    
    public String findDeal(String meter_name) throws RemoteException;
    public String receiveRequestForDeal(String meter_name, String meter_host, String meter_location) throws RemoteException;
    public boolean registerMeterWithCompany(String meter_name, String meter_host, String meter_location) throws RemoteException;
    
    public boolean isThereSuchCompany(String company_name) throws RemoteException;
    public boolean isThereSuchMeter(String meter_name) throws RemoteException;

    public void updateMeterRegistrationInfo(String meter_name, String meter_location, String registered_company) throws RemoteException;
    public void deleteRegisteredMeter(String meter_name, String company_name) throws RemoteException;
    public String getRegisteredCompany(String meter_name) throws RemoteException;
    public String getRegisteredCompanyLocation(String meter_name) throws RemoteException;
    }